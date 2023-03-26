#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include <stdint.h>

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

#define EOF_CMD         10

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