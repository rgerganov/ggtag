#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include <stdint.h>

// draw text command
#define TEXT_CMD     0
// draw rectangle command
#define RECT_CMD     1
// draw circle command
#define CIRCLE_CMD   2
// draw line command
#define LINE_CMD     3

#define EOF_CMD      4

// bits for command
#define CMD_BITS     4
// bits for font number
#define FONT_BITS    3
// bits for X coordinate
#define X_BITS       9
// bits for Y coordinate
#define Y_BITS       9
// bits for radius
#define R_BITS       7
// bits for length of text
#define LENGTH_BITS  7
// bits for single character
#define CHAR_BITS    7

extern "C" {
void renderBits(const uint8_t *input, int bits_count);
}

#endif