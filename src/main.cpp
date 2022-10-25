#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ggwave/ggwave.h"

//#include "tusb.h"
#include "DEV_Config.h"
#include "EPD_2in13_V3.h"
#include "GUI_Paint.h"

#include "pico/stdlib.h"
#include "pico/sleep.h"
#include "pico/pdm_microphone.h"

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/rosc.h"
#include "hardware/rtc.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"
#include "hardware/structs/scb.h"

#define LED_PIN 25
#define WAKE_PIN 15

#define SAMPLE_RATE 24000
#define BUF_SIZE 528
#define SAMPLES_PER_FRAME 512

// this seems to be too much for real-time processing
//#define SAMPLE_RATE 48000
//#define BUF_SIZE 1056
//#define SAMPLES_PER_FRAME 1024

// how many seconds to run for after waking up
#define AWAKE_RUN_S 30

volatile int samplesRead = 0;

int16_t sampleBufferCur[BUF_SIZE];        // we read new samples in this buffer
int16_t sampleBuffer[3*SAMPLES_PER_FRAME]; // after that, we queue them in this bigger buffer

GGWave ggwave;
GGWave::TxRxData result;

// Check if BOOTSSEL is pressed
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

// microphone configuration
const struct pdm_microphone_config config = {
    // GPIO pin for the PDM DAT signal
    .gpio_data = 0,

    // GPIO pin for the PDM CLK signal
    .gpio_clk = 1,

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
// E-ink display helpers
//

void eink_print_text(const char * text, UBYTE *img, bool partial) {
    Paint_SelectImage(img);
    if (!partial) {
        Paint_Clear(WHITE);
        Paint_DrawString_EN(15, 15, text, &Font16, WHITE, BLACK);
        EPD_2in13_V3_Display(img);
    } else {
        Paint_Clear(WHITE);
        Paint_DrawString_EN(15, 31, text, &Font16, WHITE, BLACK);
        EPD_2in13_V3_Display(img);
        //EPD_2in13_V3_Display_Partial(img);
    }
}

//
// deep sleep functionality
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
    stdio_init_all();

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
    sleep_run_from_rosc();

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

    uart_default_tx_wait_blocking();

    //reset processor and clocks back to defaults
    recover_from_sleep(scb_orig, clock0_orig, clock1_orig);

    //clocks should be restored
    measure_freqs();
}

int main(void) {
    // initialize stdio and wait for USB CDC connect
    stdio_init_all();
    // while (!tud_cdc_connected()) {
    //     tight_loop_contents();
    // }

    gpio_init(LED_PIN) ;
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(WAKE_PIN, GPIO_IN);
    gpio_put(LED_PIN,1);

    // initialize the e-ink display
    UBYTE *img;
    {
        printf("EPD module init\n");
        if(DEV_Module_Init() != 0) {
            printf("EPD module init failed\n");
            while (1) { tight_loop_contents(); }
        }
        printf("e-Paper Init and Clear...\n");
        EPD_2in13_V3_Init();
        EPD_2in13_V3_Clear();

        //Create a new image cache
        UWORD imgSize = ((EPD_2in13_V3_WIDTH % 8 == 0)? (EPD_2in13_V3_WIDTH / 8 ): (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
        if((img = (UBYTE *)malloc(imgSize)) == NULL) {
            printf("Failed to allocate memory\n");
            while (1) { tight_loop_contents(); }
        }

        printf("Drawing\n");
        Paint_NewImage(img, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 90, WHITE);
        eink_print_text("Hello, this is ggtag!", img, false);
        DEV_Delay_ms(1000);
        EPD_2in13_V3_Sleep();
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

    int niter = 0;
    int sampleCount = 0;
    uint8_t lastData[32];

    // needed for the deep sleep functionality
    rtc_init();
    measure_freqs();

    // I need this for debugging on the non-ggtag pico
    printf("Blinking for 1 second\n");

    for (int i = 0; i < 10; ++i) {
        gpio_put(LED_PIN, 1);
        sleep_ms(50);
        gpio_put(LED_PIN, 0);
        sleep_ms(50);
    }

    while (1) {
        // go to deep sleep for 3 seconds
        {
            uart_default_tx_wait_blocking();
            gpio_put(LED_PIN, 0);

            rtc_sleep(45, 3);
        }

        // needed to debug using the Pico RP LED
        for (int i = 0; i < 3; ++i) {
            gpio_put(LED_PIN, 1);
            sleep_ms(50);
            gpio_put(LED_PIN, 0);
            sleep_ms(50);
        }

        gpio_put(LED_PIN, 1);

        // bootsel is pressed - init mic and listen for AWAKE_RUN_S seconds
        bool pressed = get_bootsel_button();
        if (pressed) {
            gpio_put(LED_PIN, 1);
            printf("Button pressed\n");

            // the display should turn on when Display() is called
            eink_print_text("Listening ...", img, true);

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
                        if (memcmp(result.data(), lastData, nr) != 0) {
                            printf("Received data with length %d bytes\n", nr); // should be equal to p.payloadLength
                            printf("%s\n", result.data());
                            eink_print_text((const char *) result.data(), img, false);
                            memcpy(lastData, result.data(), nr);

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

            printf("stopping mic\n");

            // stop capturing data from the PDM microphone
            pdm_microphone_stop();
            pdm_microphone_deinit();

            // put the display to sleep
            EPD_2in13_V3_Sleep();

            printf("done\n");
        }

        gpio_put(LED_PIN, 0);
    }

    return 0;
}
