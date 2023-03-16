#include <stdio.h>
#include <string.h>
#include "rfid.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/clocks.h"
#include "blink.pio.h"

const int EM_PROG_SIZE = 596;
const int HID_PROG_SIZE = 7496;
const int MAX_PROG_SIZE = HID_PROG_SIZE;

const int EM_HEADER_SIZE = 48;
const int EM_FOOTER_SIZE = 36;
const int HID_HEADER_SIZE = 518;
const int HID_FOOTER_SIZE = 6;

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


static uint8_t em_header[EM_HEADER_SIZE] = {
  0x0e, 0xc0, 0x15, 0xc0, 0x14, 0xc0, 0x13, 0xc0, 0x12, 0xc0, 0x11, 0xc0,
  0x10, 0xc0, 0x0f, 0xc0, 0x0e, 0xc0, 0x0d, 0xc0, 0x0c, 0xc0, 0x0b, 0xc0,
  0x0a, 0xc0, 0x09, 0xc0, 0x08, 0xc0, 0x11, 0x24, 0x1f, 0xbe, 0xcf, 0xe5,
  0xd2, 0xe0, 0xde, 0xbf, 0xcd, 0xbf, 0x02, 0xd0, 0x11, 0xc1, 0xe8, 0xcf
};

static uint8_t em_footer[EM_FOOTER_SIZE] = {
  0x08, 0xe1, 0x02, 0xc0, 0x00, 0xe0, 0x00, 0xc0, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0,
  0x00, 0xc0, 0x00, 0xc0, 0x00, 0x00, 0x08, 0x95, 0xf8, 0x94, 0xff, 0xcf
};

static uint8_t hid_header[HID_HEADER_SIZE] = {
  0x0e, 0xc0, 0x15, 0xc0, 0x14, 0xc0, 0x13, 0xc0, 0x12, 0xc0, 0x11, 0xc0,
  0x10, 0xc0, 0x0f, 0xc0, 0x0e, 0xc0, 0x0d, 0xc0, 0x0c, 0xc0, 0x0b, 0xc0,
  0x0a, 0xc0, 0x09, 0xc0, 0x08, 0xc0, 0x11, 0x24, 0x1f, 0xbe, 0xcf, 0xe5,
  0xd2, 0xe0, 0xde, 0xbf, 0xcd, 0xbf, 0x02, 0xd0, 0x8b, 0xce, 0xe8, 0xcf,
  0x00, 0x27, 0x18, 0xe1, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0,
  0x00, 0x00, 0x01, 0x27, 0x07, 0xbb, 0x00, 0xc0, 0x00, 0x00, 0x01, 0x27,
  0x07, 0xbb
};

static uint8_t hid_footer[HID_FOOTER_SIZE] = {
  0x78, 0xc1, 0xf8, 0x94, 0xff, 0xcf
};

// the HID header is "000111", so the next zero is "even"
static bool evenzero = true;

static uint8_t prog[MAX_PROG_SIZE];

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

static void stop_bit(int *addr)
{
    const int baseband30_0_addr = 0x230;
    const int baseband30_1_addr = 0x234;
    rcall(addr, baseband30_0_addr);
    rjmp0(addr);
    rcall(addr, baseband30_1_addr);
    rjmp_main(addr);
}

static void manchester_ask(int *addr, uint32_t value, int bit_count)
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

static void hid_baseband_0(int *addr)
{
    int count = 12;
    if (!evenzero) {
        count++;
    }
    for (int i = 0; i < count; i++) {
        // rjmp .+0
        prog[(*addr)++] = 0x00;
        prog[(*addr)++] = 0xc0;
        // eor r16, r17
        prog[(*addr)++] = 0x01;
        prog[(*addr)++] = 0x27;
        // out 0x17, r16
        prog[(*addr)++] = 0x07;
        prog[(*addr)++] = 0xbb;
    }
    evenzero = !evenzero;
}

static void hid_baseband_1(int *addr)
{
    for (int i = 0; i < 10; i++) {
        // rjmp .+0
        prog[(*addr)++] = 0x00;
        prog[(*addr)++] = 0xc0;
        // nop
        prog[(*addr)++] = 0x00;
        prog[(*addr)++] = 0x00;
        // eor r16, r17
        prog[(*addr)++] = 0x01;
        prog[(*addr)++] = 0x27;
        // out 0x17, r16
        prog[(*addr)++] = 0x07;
        prog[(*addr)++] = 0xbb;
    }
}

static void manchester_fsk(int *addr, uint32_t value, int bit_count)
{
    uint32_t mask = 1 << (bit_count - 1);
    for (int i = 0 ; i < bit_count ; i++) {
        if (value & mask) {
            //printf("1, addr=%d\n", *addr);
            hid_baseband_1(addr);
            hid_baseband_0(addr);
        } else {
            //printf("0, addr=%d\n", *addr);
            hid_baseband_0(addr);
            hid_baseband_1(addr);
        }
        mask >>= 1;
    }
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

static void blink_pin_start(PIO pio, uint sm, uint offset, uint pin, uint freq) {
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    printf("Blinking pin %d at %d Hz\n", pin, freq);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    pio->txf[sm] = (clock_get_hz(clk_sys) / (2 * freq)) - 3;
}

static void blink_pin_stop(PIO pio, uint sm) {
    pio_sm_set_enabled(pio, sm, false);
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

static bool flash_program(int prog_size)
{
    PIO pio = pio1;
    uint offset = pio_add_program(pio, &blink_program);
    printf("Loaded program at %d\n", offset);
    blink_pin_start(pio, 0, offset, 6, 1000000);

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
    if (sig[0] != 0x1E || sig[1] != 0x93 || sig[2] != 0x0B) {
        printf("signature mismatch\n");
        return false;
    }

    printf("writing lfuse\n");
    write_lfuse(0xC0);
    //write_lfuse(0x62);

    printf("reading lfuse\n");
    printf("lfuse: %02x\n", read_lfuse());

    printf("writing flash\n");
    write_flash(prog, prog_size);

    //printf("reading flash\n");
    //flash_read_page(0);

    printf("end pmode\n");
    gpio_init(3);
    gpio_set_dir(3, GPIO_IN);
    reset_target(false);
    blink_pin_stop(pio, 0);
    return true;
}

void generate_em_prog(uint8_t mfr_id, uint32_t unique_id)
{
    memcpy(prog, em_header, EM_HEADER_SIZE);
    memcpy(prog + EM_PROG_SIZE - EM_FOOTER_SIZE, em_footer, EM_FOOTER_SIZE);

    int addr = EM_HEADER_SIZE;
    manchester_ask(&addr, 0x1FF, 9);
    manchester_ask(&addr, ROW_PARITY(mfr_id >> 4), 5);
    manchester_ask(&addr, ROW_PARITY(mfr_id >> 0), 5);
    manchester_ask(&addr, ROW_PARITY(unique_id >> 28), 5);
    manchester_ask(&addr, ROW_PARITY(unique_id >> 24), 5);
    manchester_ask(&addr, ROW_PARITY(unique_id >> 20), 5);
    manchester_ask(&addr, ROW_PARITY(unique_id >> 16), 5);
    manchester_ask(&addr, ROW_PARITY(unique_id >> 12), 5);
    manchester_ask(&addr, ROW_PARITY(unique_id >> 8), 5);
    manchester_ask(&addr, ROW_PARITY(unique_id >> 4), 5);
    manchester_ask(&addr, ROW_PARITY(unique_id >> 0), 5);
    manchester_ask(&addr, COLUMN_PARITY(mfr_id, unique_id), 4);
    stop_bit(&addr);
}

bool program_em_rfid(uint8_t mfr_id, uint32_t uid)
{
    generate_em_prog(mfr_id, uid);
    return flash_program(EM_PROG_SIZE);
}

void generate_hid_prog(uint16_t id1, uint32_t id2)
{
    memcpy(prog, hid_header, HID_HEADER_SIZE);
    memcpy(prog + HID_PROG_SIZE - HID_FOOTER_SIZE, hid_footer, HID_FOOTER_SIZE);

    int addr = HID_HEADER_SIZE;
    // the HID header is "000111", so the next zero is "even"
    evenzero = true;
    manchester_fsk(&addr, id1, 13);
    manchester_fsk(&addr, id2, 32);
}

bool program_hid_rfid(uint16_t id1, uint32_t id2)
{
    generate_hid_prog(id1, id2);
    return flash_program(HID_PROG_SIZE);
}