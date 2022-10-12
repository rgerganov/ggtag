#include <stdio.h>
#include <string.h>

#include "ggwave/ggwave.h"
#include "pico/stdlib.h"
#include "pico/pdm_microphone.h"
#include "tusb.h"

const int sampleRate = 24000;
const int bufsize = 528;
const int samplesPerFrame = 512;

// this seems to be too much for real-time processing
//const int sampleRate = 48000;
//const int bufsize = 1056;
//const int samplesPerFrame = 1024;

int16_t sampleBufferCur[bufsize];        // we read new samples in this buffer
int16_t sampleBuffer[3*samplesPerFrame]; // after that, we queue them in this bigger buffer
volatile int samplesRead = 0;
GGWave ggwave;
GGWave::TxRxData result;

// microphone configuration
const struct pdm_microphone_config config = {
    // GPIO pin for the PDM DAT signal
    .gpio_data = 22,

    // GPIO pin for the PDM CLK signal
    .gpio_clk = 23,

    // PIO instance to use
    .pio = pio0,

    // PIO State Machine instance to use
    .pio_sm = 0,

    // sample rate in Hz
    .sample_rate = sampleRate,

    // number of samples to buffer
    .sample_buffer_size = bufsize,
};

void on_pdm_samples_ready()
{
    // callback from library when all the samples in the library
    // internal sample buffer are ready for reading
    samplesRead = pdm_microphone_read(sampleBufferCur, bufsize);
}

int main(void)
{
    // initialize stdio and wait for USB CDC connect
    stdio_init_all();
    while (!tud_cdc_connected()) {
        tight_loop_contents();
    }

    printf("Hello, this is ggtag!\n");
    ggwave.setLogFile(nullptr);
    auto p = GGWave::getDefaultParameters();
    p.payloadLength   = 8;
    p.sampleRateInp   = sampleRate;
    p.sampleRateOut   = sampleRate;
    p.sampleRate      = sampleRate;
    p.samplesPerFrame = samplesPerFrame;
    p.sampleFormatInp = GGWAVE_SAMPLE_FORMAT_I16;
    p.sampleFormatOut = GGWAVE_SAMPLE_FORMAT_U8;
    p.operatingMode   = GGWAVE_OPERATING_MODE_RX | GGWAVE_OPERATING_MODE_TX | GGWAVE_OPERATING_MODE_USE_DSS | GGWAVE_OPERATING_MODE_TX_ONLY_TONES;

    GGWave::Protocols::tx().disableAll();
    GGWave::Protocols::rx().disableAll();
    //GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_MT_NORMAL, true);
    //GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_MT_FAST, true);
    GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_MT_FASTEST, true);
    //GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_DT_NORMAL, true);
    //GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_DT_FAST, true);
    GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_DT_FASTEST, true);
    // Print the memory required for the "ggwave" instance:
    ggwave.prepare(p, false);
    printf("Required memory by the ggwave instance: %d\n", ggwave.heapSize());
    if (!ggwave.prepare(p, true)) {
        printf("ggwave initialization failed\n");
        while (1) { tight_loop_contents(); }
    }
    printf("ggwave initialized successfully\n");

    // initialize the PDM microphone
    if (pdm_microphone_init(&config) < 0) {
        printf("PDM microphone initialization failed!\n");
        while (1) { tight_loop_contents(); }
    }

    // set callback that is called when all the samples in the library
    // internal sample buffer are ready for reading
    pdm_microphone_set_samples_ready_handler(on_pdm_samples_ready);

     // start capturing data from the PDM microphone
    if (pdm_microphone_start() < 0) {
        printf("PDM microphone start failed!\n");
        while (1) { tight_loop_contents(); }
    }
    int niter = 0;
    uint32_t totalSamples = 0;

    int sampleCount = 0; // the number of samples we have in the sampleBuffer
    while (1) {
        // wait for new samples
        while (samplesRead == 0) { tight_loop_contents(); }
        // store and clear the samples read from the callback
        for (int i = 0; i < samplesRead; i++) {
            sampleBuffer[sampleCount + i] = sampleBufferCur[i];
        }
        sampleCount += samplesRead;
        samplesRead = 0;

        while (sampleCount >= samplesPerFrame) {
            auto tStart = to_ms_since_boot(get_absolute_time());
            //printf("trying to decode... sampleCount = %d\n", sampleCount);
            ggwave.decode(sampleBuffer, samplesPerFrame*sizeof(int16_t));
            auto tEnd = to_ms_since_boot(get_absolute_time());
            if (++niter % 10 == 0) {
                //printf("time: %d\n", tEnd - tStart);
                // Check if decode() is taking too long
                if (tEnd - tStart > 1000*(float(samplesPerFrame)/sampleRate)) {
                    printf("Warning: decode() took too long to execute!\n");
                }
            }

            // Check if we have successfully decoded any data:
            int nr = ggwave.rxTakeData(result);
            if (nr > 0) {
                printf("Received data with length %d bytes\n", nr); // should be equal to p.payloadLength
                printf("%s\n", result.data());
            }

            // move samples to the front of the buffer
            for (int i = samplesPerFrame; i < sampleCount; i++) {
                sampleBuffer[i - samplesPerFrame] = sampleBuffer[i];
            }
            sampleCount -= samplesPerFrame;
        }
    }
    return 0;
}
