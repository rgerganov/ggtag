#include <stdio.h>
#include <string.h>
#include "rfid.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/clocks.h"
#include "blink.pio.h"

const int PROG_SIZE = 596;
const int HEADER_SIZE = 48;
const int FOOTER_SIZE = 36;

// copied from avrfid.S
#define ROW_PARITY(n)  ( (((n) & 0xF) << 1) | \
                         (((n) ^ ((n) >> 1) ^ ((n) >> 2) ^ ((n) >> 3)) & 1) )

#define COLUMN_PARITY(EM4102_MFR_ID, EM4102_UNIQUE_ID)  ( (EM4102_MFR_ID >> 4) ^        \
                                                          (EM4102_MFR_ID) ^             \
                                                          (EM4102_UNIQUE_ID >> 28) ^    \
                                                          (EM4102_UNIQUE_ID >> 24) ^    \
                                                          (EM4102_UNIQUE_ID >> 20) ^    \
                                                          (EM4102_UNIQUE_ID >> 16) ^    \
                                                          (EM4102_UNIQUE_ID >> 12) ^    \
                                                          (EM4102_UNIQUE_ID >> 8) ^     \
                                                          (EM4102_UNIQUE_ID >> 4) ^     \
                                                          (EM4102_UNIQUE_ID) )


static uint8_t header[HEADER_SIZE] = {
  0x0e, 0xc0, 0x15, 0xc0, 0x14, 0xc0, 0x13, 0xc0, 0x12, 0xc0, 0x11, 0xc0,
  0x10, 0xc0, 0x0f, 0xc0, 0x0e, 0xc0, 0x0d, 0xc0, 0x0c, 0xc0, 0x0b, 0xc0,
  0x0a, 0xc0, 0x09, 0xc0, 0x08, 0xc0, 0x11, 0x24, 0x1f, 0xbe, 0xcf, 0xe5,
  0xd2, 0xe0, 0xde, 0xbf, 0xcd, 0xbf, 0x02, 0xd0, 0x11, 0xc1, 0xe8, 0xcf
};

static uint8_t footer[FOOTER_SIZE] = {
  0x08, 0xe1, 0x02, 0xc0, 0x00, 0xe0, 0x00, 0xc0, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0,
  0x00, 0xc0, 0x00, 0xc0, 0x00, 0x00, 0x08, 0x95, 0xf8, 0x94, 0xff, 0xcf
};

static uint8_t prog[PROG_SIZE];

static void rcall(int *addr, int dest)
{
    int offset = (dest - *addr - 2) / 2;
    uint16_t opcode = 0xd000;
    opcode |= (offset & 0x0fff);
    prog[(*addr)++] = opcode & 0xff;
    prog[(*addr)++] = (opcode >> 8) & 0xff;
}

static void rjmp0(int *addr)
{
    // rjmp .+0
    prog[(*addr)++] = 0x00;
    prog[(*addr)++] = 0xc0;
}

static void rjmp_main(int *addr)
{
    // rjmp main
    prog[(*addr)++] = 0x00;
    prog[(*addr)++] = 0xcf;
}

static void manchester(int *addr, uint32_t value, int bit_count)
{
    const int baseband30_0_addr = 0x230;
    const int baseband30_1_addr = 0x234;
    uint32_t mask = 1 << (bit_count - 1);
    for (int i = 0 ; i < bit_count ; i++) {
        if (value & mask) {
            rcall(addr, baseband30_1_addr);
            rjmp0(addr);
            rcall(addr, baseband30_0_addr);
            rjmp0(addr);
        } else {
            rcall(addr, baseband30_0_addr);
            rjmp0(addr);
            rcall(addr, baseband30_1_addr);
            rjmp0(addr);
        }
        mask >>= 1;
    }
}

static void stop_bit(int *addr)
{
    const int baseband30_0_addr = 0x230;
    const int baseband30_1_addr = 0x234;
    rcall(addr, baseband30_0_addr);
    rjmp0(addr);
    rcall(addr, baseband30_1_addr);
    rjmp_main(addr);
}

static void reset_target(bool reset)
{
    gpio_put(5, reset ? 0 : 1);
}

static void spi_write_byte(uint8_t val)
{
    spi_write_blocking(spi0, &val, 1);
}

static uint8_t spi_read_byte()
{
    uint8_t val;
    spi_read_blocking(spi0, 0, &val, 1);
    return val;
}

static void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq) {
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    printf("Blinking pin %d at %d Hz\n", pin, freq);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    pio->txf[sm] = (clock_get_hz(clk_sys) / (2 * freq)) - 3;
}

static void flash(uint8_t hilo, unsigned int addr, uint8_t data)
{
    uint8_t out_buf[4] = {uint8_t(0x40 + 8 * hilo), uint8_t((addr >> 8) & 0xFF), uint8_t(addr & 0xFF), data};
    uint8_t in_buf[4];
    spi_write_read_blocking(spi0, out_buf, in_buf, 4);
}

static void commit(unsigned int addr)
{
    uint8_t out_buf[4] = {0x4C, uint8_t((addr >> 8) & 0xFF), uint8_t(addr & 0xFF), 0};
    uint8_t in_buf[4];
    spi_write_read_blocking(spi0, out_buf, in_buf, 4);
}

#define LOW  0x00
#define HIGH 0x01

static void write_flash_page(unsigned int page, const uint8_t *buff, int buf_size)
{
    int ind = 0;
    unsigned int addr = page;
    if (buf_size > 64) {
        printf("Buffer too large, page is 32 words (64 bytes)\n");
        return;
    }
    for (int i = 0 ; i < buf_size/2; i++) {
        flash(LOW, addr, buff[ind++]);
        flash(HIGH, addr, buff[ind++]);
        addr++;
    }
    // fill the rest of the page with 0xFF
    for (int i = buf_size/2; i < 32; i++) {
        flash(LOW, addr, 0xFF);
        flash(HIGH, addr, 0xFF);
        addr++;
    }
    commit(page);
}

static void write_flash(const uint8_t *buff, int buf_size)
{
    unsigned int page = 0;
    int ind = 0;
    while (ind < buf_size) {
        int chunk_size = buf_size - ind > 64 ? 64 : buf_size - ind;
        write_flash_page(page, buff + ind, chunk_size);
        sleep_ms(5); // Twd_flash = 4.5ms, page 153 of datasheet
        ind += 64;
        page += 32;
    }
}

static uint8_t flash_read(uint8_t hilo, unsigned int addr)
{
    uint8_t out_buf[4] = {uint8_t(0x20 + hilo * 8), uint8_t((addr >> 8) & 0xFF), uint8_t(addr & 0xFF), 0};
    //printf("out_buf: %02x %02x %02x %02x\n", out_buf[0], out_buf[1], out_buf[2], out_buf[3]);
    uint8_t in_buf[4];
    spi_write_read_blocking(spi0, out_buf, in_buf, 4);
    //printf("in_buf: %02x %02x %02x %02x\n", in_buf[0], in_buf[1], in_buf[2], in_buf[3]);
    return in_buf[3];
}

static void flash_read_page(unsigned int addr)
{
    for (int i = 0; i < 32; i ++) {
        uint8_t low = flash_read(LOW, addr);
        uint8_t high = flash_read(HIGH, addr);
        printf("%02x%02x\n", low, high);
        addr++;
    }
    printf("\n");
}

static void write_lfuse(uint8_t val)
{
    uint8_t write_lfuse[4] = {0xAC, 0xA0, 0x00, val};
    uint8_t in_buf[4];
    spi_write_read_blocking(spi0, write_lfuse, in_buf, 4);
    sleep_ms(5); // Twd_fuse = 4.5ms, page 153 of datasheet
}

static uint8_t read_lfuse()
{
    uint8_t read_lfuse[4] = {0x50, 0x00, 0x00, 0x00};
    uint8_t in_buf[4];
    spi_write_read_blocking(spi0, read_lfuse, in_buf, 4);
    return in_buf[3];
}

static void erase_chip()
{
    reset_target(true);
    uint8_t erase_chip[4] = {0xAC, 0x80, 0x00, 0x00};
    uint8_t in_buf[4];
    spi_write_read_blocking(spi0, erase_chip, in_buf, 4);
    sleep_ms(9); // Twd_erase = 9ms, page 153 of datasheet
    // pulse reset line
    reset_target(false);
    sleep_ms(20);
    reset_target(true);
}

static void read_signature(uint8_t *out_sig)
{
    uint8_t read_sig[4] = {0x30, 0x00, 0x00, 0x00};
    uint8_t in_buf[4];
    spi_write_read_blocking(spi0, read_sig, in_buf, 4);
    //printf("in_buf: %02x %02x %02x %02x\n", in_buf[0], in_buf[1], in_buf[2], in_buf[3]);
    out_sig[0] = in_buf[3];
    read_sig[2] = 0x01;
    spi_write_read_blocking(spi0, read_sig, in_buf, 4);
    //printf("in_buf: %02x %02x %02x %02x\n", in_buf[0], in_buf[1], in_buf[2], in_buf[3]);
    out_sig[1] = in_buf[3];
    read_sig[2] = 0x02;
    spi_write_read_blocking(spi0, read_sig, in_buf, 4);
    //printf("in_buf: %02x %02x %02x %02x\n", in_buf[0], in_buf[1], in_buf[2], in_buf[3]);
    out_sig[2] = in_buf[3];
}

static void prog_enable()
{
    reset_target(true);
    sleep_ms(50); // wait at least 20ms after reset
    uint8_t prog_enable[4] = {0xAC, 0x53, 0x00, 0x00};
    uint8_t in_buf[4];
    spi_write_read_blocking(spi0, prog_enable, in_buf, 4);
}

void generate_prog(uint8_t mfr_id, uint32_t unique_id)
{
    memcpy(prog, header, HEADER_SIZE);
    memcpy(prog + PROG_SIZE - FOOTER_SIZE, footer, FOOTER_SIZE);

    int addr = HEADER_SIZE;
    manchester(&addr, 0x1FF, 9);
    manchester(&addr, ROW_PARITY(mfr_id >> 4), 5);
    manchester(&addr, ROW_PARITY(mfr_id >> 0), 5);
    manchester(&addr, ROW_PARITY(unique_id >> 28), 5);
    manchester(&addr, ROW_PARITY(unique_id >> 24), 5);
    manchester(&addr, ROW_PARITY(unique_id >> 20), 5);
    manchester(&addr, ROW_PARITY(unique_id >> 16), 5);
    manchester(&addr, ROW_PARITY(unique_id >> 12), 5);
    manchester(&addr, ROW_PARITY(unique_id >> 8), 5);
    manchester(&addr, ROW_PARITY(unique_id >> 4), 5);
    manchester(&addr, ROW_PARITY(unique_id >> 0), 5);
    manchester(&addr, COLUMN_PARITY(mfr_id, unique_id), 4);
    stop_bit(&addr);
}

bool program_em_rfid(uint8_t mfr_id, uint32_t uid)
{
    generate_prog(mfr_id, uid);
    PIO pio = pio1;
    uint offset = pio_add_program(pio, &blink_program);
    printf("Loaded program at %d\n", offset);
    blink_pin_forever(pio, 0, offset, 6, 1000000);

    printf("Programming attiny85\n");
    spi_init(spi0, 100000);
    // SCK = GP2, MOSI = GP3, MISO = GP4, RESET = GP5
    gpio_set_function(2, GPIO_FUNC_SPI);
    gpio_set_function(3, GPIO_FUNC_SPI);
    gpio_set_function(4, GPIO_FUNC_SPI);
    gpio_init(5);
    gpio_set_dir(5, GPIO_OUT);

    printf("start pmode\n");
    prog_enable();
    printf("erasing chip\n");
    erase_chip();

    printf("start pmode\n");
    prog_enable();

    uint8_t sig[3];
    printf("read device signature\n");
    read_signature(sig);
    printf("sig: %02x %02x %02x\n", sig[0], sig[1], sig[2]);

    printf("writing lfuse\n");
    write_lfuse(0xC0);

    printf("reading lfuse\n");
    printf("lfuse: %02x\n", read_lfuse());

    printf("writing flash\n");
    write_flash(prog, PROG_SIZE);

    printf("reading flash\n");
    flash_read_page(0);

    printf("end pmode\n");
    gpio_init(3);
    gpio_set_dir(3, GPIO_IN);
    reset_target(false);
    return true;
}