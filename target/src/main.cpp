#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"
#include "ggwave/ggwave.h"

//#include "tusb.h"
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "pdm_microphone.h"

#include "pico/stdlib.h"
#include "pico/sleep.h"

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/rosc.h"
#include "hardware/rtc.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/scb.h"

// deep sleep implementation
// 0 - no deep sleep at all
// 1 - deep sleep with RTC wakeup
// 2 - deep sleep with GPIO wakeup
//#define GGTAG_DEEP_SLEEP 1 (configured from CMake)

#define WAKE_PIN 18

// if there is 1 on this pin then we are running from USB
#define USB_CHECK_PIN 16

#define SAMPLE_RATE 24000
#define BUF_SIZE 528
#define SAMPLES_PER_FRAME 512

// this seems to be too much for real-time processing
//#define SAMPLE_RATE 48000
//#define BUF_SIZE 1056
//#define SAMPLES_PER_FRAME 1024

// how many seconds to run for after waking up
#define AWAKE_RUN_S 30

UBYTE *img; // EPD image buffer

volatile int samplesRead = 0;

int16_t sampleBufferCur[BUF_SIZE];         // we read new samples in this buffer
int16_t sampleBuffer[3*SAMPLES_PER_FRAME]; // after that, we queue them in this bigger buffer

GGWave ggwave;
GGWave::TxRxData result;

#if GGTAG_DEEP_SLEEP == 1
// In this mode, we use RTC to wake up from deep sleep every few seconds and check if the BOOTSEL button is pressed

// Check if BOOTSEL is pressed
// ref: https://github.com/raspberrypi/pico-examples/blob/master/picoboard/button/button.c
bool __no_inline_not_in_flash_func(get_bootsel_button)() {
    const uint CS_PIN_INDEX = 1;

    // Must disable interrupts, as interrupt handlers may be in flash, and we
    // are about to temporarily disable flash access!
    uint32_t flags = save_and_disable_interrupts();

    // Set chip select to Hi-Z
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    // Note we can't call into any sleep functions in flash right now
    for (volatile int i = 0; i < 1000; ++i);

    // The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
    // Note the button pulls the pin *low* when pressed.
    bool button_state = !(sio_hw->gpio_hi_in & (1u << CS_PIN_INDEX));

    // Need to restore the state of chip select, else we are going to have a
    // bad time when we return to code in flash!
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    restore_interrupts(flags);

    return button_state;
}
#endif

//
// microphone
//

const struct pdm_microphone_config config = {
    // GPIO pin for the PDM DAT signal
    .gpio_data = 2,

    // GPIO pin for the PDM CLK signal
    .gpio_clk = 3,

    // PIO instance to use
    .pio = pio0,

    // PIO State Machine instance to use
    .pio_sm = 0,

    // sample rate in Hz
    .sample_rate = SAMPLE_RATE,

    // number of samples to buffer
    .sample_buffer_size = BUF_SIZE,
};

void on_pdm_samples_ready() {
    // callback from library when all the samples in the library
    // internal sample buffer are ready for reading
    samplesRead = pdm_microphone_read(sampleBufferCur, BUF_SIZE);
}

//
// Pico RP deep sleep functionality
// ref: https://github.com/ghubcoder/PicoSleepDemo
//

void recover_from_sleep(uint scb_orig, uint clock0_orig, uint clock1_orig){
    //Re-enable ring Oscillator control
    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS);

    //reset procs back to default
    scb_hw->scr = scb_orig;
    clocks_hw->sleep_en0 = clock0_orig;
    clocks_hw->sleep_en1 = clock1_orig;

    //reset clocks
    clocks_init();
    stdio_uart_init();

    return;
}

void measure_freqs(void) {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);

    uart_default_tx_wait_blocking();
    // Can't measure clk_ref / xosc as it is the ref
}

static void sleep_callback(void) {
    uart_default_tx_wait_blocking();
    return;
}

static void rtc_sleep(int8_t minute_to_sleep_to, int8_t second_to_sleep_to) {
    static uint scb_orig = scb_hw->scr;
    static uint clock0_orig = clocks_hw->sleep_en0;
    static uint clock1_orig = clocks_hw->sleep_en1;

#if GGTAG_DEEP_SLEEP == 1
    // RTC-based deep sleep, wake up every few seconds
    {
        datetime_t t = {
            .year  = 2010,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = 45,
            .sec   = 00
        };

        uart_default_tx_wait_blocking();
        sleep_run_from_xosc();

        rtc_set_datetime(&t);

        datetime_t t_alarm = {
            .year  = 2010,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = minute_to_sleep_to,
            .sec   = second_to_sleep_to
        };

        uart_default_tx_wait_blocking();

        sleep_goto_sleep_until(&t_alarm, &sleep_callback);
    }
#elif GGTAG_DEEP_SLEEP == 2
    // GPIO deep sleep
    {
        uart_default_tx_wait_blocking();
        sleep_run_from_xosc();
        sleep_goto_dormant_until_pin(WAKE_PIN, false, false);
    }
#endif

    uart_default_tx_wait_blocking();

    //reset processor and clocks back to defaults
    recover_from_sleep(scb_orig, clock0_orig, clock1_orig);

    //clocks should be restored
    measure_freqs();
}

void run_from_battery()
{
    gpio_set_dir(WAKE_PIN, GPIO_IN);

    // initialize the e-ink display
    {
        printf("EPD module init\n");
        if(DEV_Module_Init() != 0) {
            printf("EPD module init failed\n");
            while (1) { tight_loop_contents(); }
        }

        //Create a new image cache
        UWORD imgSize = ((EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT;
        if((img = (UBYTE *)malloc(imgSize)) == NULL) {
            printf("Failed to allocate memory\n");
            while (1) { tight_loop_contents(); }
        }
    }

    // initialize ggwave
    {
        printf("Hello, this is ggtag!\n");
        ggwave.setLogFile(nullptr);
        auto p = GGWave::getDefaultParameters();
        p.payloadLength   = 16;
        p.sampleRateInp   = SAMPLE_RATE;
        p.sampleRateOut   = SAMPLE_RATE;
        p.sampleRate      = SAMPLE_RATE;
        p.samplesPerFrame = SAMPLES_PER_FRAME;
        p.sampleFormatInp = GGWAVE_SAMPLE_FORMAT_I16;
        p.sampleFormatOut = GGWAVE_SAMPLE_FORMAT_U8;
        p.operatingMode   = GGWAVE_OPERATING_MODE_RX | GGWAVE_OPERATING_MODE_TX | GGWAVE_OPERATING_MODE_USE_DSS | GGWAVE_OPERATING_MODE_TX_ONLY_TONES;

        GGWave::Protocols::tx().disableAll();
        GGWave::Protocols::rx().disableAll();
        GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_AUDIBLE_FASTEST, true);
        //GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_MT_NORMAL, true);
        //GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_MT_FAST, true);
        GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_MT_FASTEST, true);
        //GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_DT_NORMAL, true);
        //GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_DT_FAST, true);
        GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_DT_FASTEST, true);
        //GGWave::Protocols::rx().toggle(GGWAVE_PROTOCOL_ULTRASOUND_FASTEST, true);
        // Print the memory required for the "ggwave" instance:
        ggwave.prepare(p, false);
        printf("Required memory by the ggwave instance: %d\n", ggwave.heapSize());
        if (!ggwave.prepare(p, true)) {
            printf("ggwave initialization failed\n");
            while (1) { tight_loop_contents(); }
        }
        printf("ggwave initialized successfully\n");
    }

#if GGTAG_DEEP_SLEEP == 1
    rtc_init();
#endif

#if GGTAG_DEEP_SLEEP != 0
    // needed for the deep sleep functionality
    measure_freqs();
#endif

    // keep the lastData between runs so we can detect if new data is being received
    // the assumption is that two consecutive data buffers will not be the same
    uint8_t lastData[16] = {0};
    bool listen = true;
    while (1) {
        // init mic and listen for AWAKE_RUN_S seconds
        if (listen) {
            printf("Listening for %d seconds\n", AWAKE_RUN_S);

            // initialize the PDM microphone
            if (pdm_microphone_init(&config) < 0) {
                printf("PDM microphone initialization failed!\n");
                while (1) { tight_loop_contents(); }
            }

            // set callback that is called when all the samples in the library
            // internal sample buffer are ready for reading
            pdm_microphone_set_samples_ready_handler(on_pdm_samples_ready);

            if (pdm_microphone_start() < 0) {
                printf("PDM microphone start failed!\n");
                while (1) { tight_loop_contents(); }
            }

            int totalSamples = 0;
            int niter = 0;
            int sampleCount = 0;
            int offset = 0;
            uint8_t data[256] = {0};

            while (totalSamples < AWAKE_RUN_S*SAMPLE_RATE) {
                // wait for new samples
                while (samplesRead == 0) { tight_loop_contents(); }
                // store and clear the samples read from the callback
                for (int i = 0; i < samplesRead; i++) {
                    sampleBuffer[sampleCount + i] = sampleBufferCur[i];
                }
                sampleCount += samplesRead;
                totalSamples += samplesRead;
                samplesRead = 0;

                while (sampleCount >= SAMPLES_PER_FRAME) {
                    auto tStart = to_ms_since_boot(get_absolute_time());
                    //printf("trying to decode... sampleCount = %d\n", sampleCount);
                    ggwave.decode(sampleBuffer, SAMPLES_PER_FRAME*sizeof(int16_t));
                    auto tEnd = to_ms_since_boot(get_absolute_time());
                    if (++niter % 10 == 0) {
                        printf("time: %d\n", tEnd - tStart);
                        // Check if decode() is taking too long
                        if (tEnd - tStart > 1000*(float(SAMPLES_PER_FRAME)/SAMPLE_RATE)) {
                            printf("Warning: decode() took too long to execute!\n");
                        }
                    }

                    // Check if we have successfully decoded any data:
                    int nr = ggwave.rxTakeData(result);
                    if (nr > 0) {
                        // check if the received data is different from the previous one
                        if (memcmp(lastData, result.data(), nr) != 0) {
                            printf("Received %d bytes\n", nr);
                            memcpy(data + offset, result.data(), nr);
                            offset += nr;
                            memcpy(lastData, result.data(), nr);
                        }
                        // the first two bytes are the total length of the data
                        // but we don't support more than 255 bytes when using sound
                        int dataLength = data[1];
                        // if we have received all the data, don't wait for more
                        if (offset >= dataLength + 2) {
                            printf("Received all %d bytes\n", dataLength);
                            totalSamples = AWAKE_RUN_S*SAMPLE_RATE;
                            break;
                        }
                    }

                    // move samples to the front of the buffer
                    for (int i = SAMPLES_PER_FRAME; i < sampleCount; i++) {
                        sampleBuffer[i - SAMPLES_PER_FRAME] = sampleBuffer[i];
                    }
                    sampleCount -= SAMPLES_PER_FRAME;
                }
            }

            // display the data that has been received
            if (offset > 0) {
                printf("Drawing\n");
                EPD_Init();
                Paint_NewImage(img, EPD_WIDTH, EPD_HEIGHT, 90, WHITE);
                Paint_Clear(WHITE);
                renderBits(data+2, data[1]*8);
                EPD_Display(img, NULL);
                // put the display to sleep
                EPD_Sleep();
            }

            printf("stopping mic\n");
            // stop capturing data from the PDM microphone
            pdm_microphone_stop();
            pdm_microphone_deinit();
            printf("done\n");
        }

#if GGTAG_DEEP_SLEEP != 0
        // go to deep sleep
        {
            uart_default_tx_wait_blocking();

            rtc_sleep(45, 3);
        }
#endif

#if GGTAG_DEEP_SLEEP == 1
        // bootsel is pressed
        listen = get_bootsel_button();
#endif
    }
}

void run_from_usb()
{
    // wait for USB CDC connect
    while (!stdio_usb_connected()) {
        tight_loop_contents();
    }
    printf("EPD module init\n");
    if(DEV_Module_Init() != 0) {
        printf("EPD module init failed\n");
        while (1) { tight_loop_contents(); }
    }

    //Create a new image cache
    UWORD imgSize = ((EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT;
    if((img = (UBYTE *)malloc(imgSize)) == NULL) {
        printf("Failed to allocate memory\n");
        while (1) { tight_loop_contents(); }
    }

    while (1) {
        int l1 = getchar_timeout_us(100000); // 100ms
        if (l1 < 0) {
            continue;
        }
        int l2 = getchar_timeout_us(100000); // 100ms
        if (l2 < 0) {
            continue;
        }
        int length = l2 + l1*256;
        uint8_t *data = (uint8_t *)malloc(length);
        if (data == NULL) {
            printf("Failed to allocate memory\nDone.\n");
            stdio_flush();
            continue;
        }
        for (int i = 0; i < length; i++) {
            data[i] = getchar_timeout_us(1000);
        }
        EPD_Init();
        printf("Drawing\n");
        Paint_NewImage(img, EPD_WIDTH, EPD_HEIGHT, 90, WHITE);
        Paint_Clear(WHITE);
        renderBits(data, length*8);
        free(data);
        EPD_Display(img, NULL);
        // TODO: putting display to sleep is causing problems
        // EPD_Sleep();
        printf("Done.\n");
        stdio_flush();
    }
}

int main(void) {
    gpio_init(USB_CHECK_PIN);
    gpio_set_dir(USB_CHECK_PIN, GPIO_IN);
    if (gpio_get(USB_CHECK_PIN)) {
        stdio_usb_init();
        run_from_usb();
    } else {
        stdio_uart_init();
        run_from_battery();
    }
    return 0;
}
