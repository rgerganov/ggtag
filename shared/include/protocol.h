#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include <stdint.h>

struct BitReader {
    const uint8_t *buffer;
    int bits_count;
    int ind;

    BitReader(const uint8_t *buffer, int bits_count)
        : buffer(buffer), bits_count(bits_count), ind(0)
    {
    }

    uint32_t read(int num_bits)
    {
        if (num_bits > 32) {
            return -1;
        }
        if (ind + num_bits > bits_count) {
            return -1;
        }
        uint32_t result = 0;
        for (int i = 0; i < num_bits; i++) {
            result <<= 1;
            if (buffer[ind / 8] & (1 << (7 - (ind % 8)))) {
                result |= 1;
            }
            ind++;
        }
        return result;
    }
};

// run length encoding bits
#define RLE_BITS        2
// draw text command
#define TEXT_CMD        0
// draw rectangle command
#define RECT_CMD        1
// draw filled rectangle command
#define FILL_RECT_CMD   2
// draw circle command
#define CIRCLE_CMD      3
// draw filled circle command
#define FILL_CIRCLE_CMD 4
// draw line command
#define LINE_CMD        5
// draw qrcode command
#define QRCODE_CMD      6
// draw image command
#define IMAGE_CMD       7
// draw font awesome icon
#define ICON_CMD        8
// program rfid
#define RFID_CMD        9
// draw image (run length encoded)
#define RLE_IMAGE_CMD   10
// draw ellipse
#define ELLIPSE_CMD     11

#define EOF_CMD         12

// bits for command
#define CMD_BITS       4
// bits for font number
#define FONT_BITS      3
// bits for X coordinate
#define X_BITS         9
// bits for Y coordinate
#define Y_BITS         8
// bits for radius
#define R_BITS         7
// bits for length of text
#define LENGTH_BITS    7
// bits for single character
#define CHAR_BITS      7
// bits for qrcode pixel width
#define QR_PIXEL_WIDTH 2
// bits for icon codepoint
#define ICON_BITS      16
// bits for RFID (first part)
#define RFID1_BITS     13
// bits for RFID (second part)
#define RFID2_BITS     32

extern "C" {
void renderBits(const uint8_t *input, int bits_count);
}

#endif