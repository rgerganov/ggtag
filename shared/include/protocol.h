#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include <stdint.h>

#define TEXT_CMD     1
#define RECT_CMD     2
#define EOF_CMD      3

#define CMD_BITS     4
#define FONT_BITS    3
#define X_BITS       9
#define Y_BITS       9
#define LENGTH_BITS  7
#define CHAR_BITS    7

extern "C" {
void renderBits(const uint8_t *input, int bits_count);
}

#endif