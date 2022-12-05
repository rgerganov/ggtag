#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"
#include "GUI_Paint.h"

struct BitReader {
    const uint8_t *buffer;
    int bits_count;
    int ind;

    BitReader(const uint8_t *buffer, int bits_count)
        : buffer(buffer), bits_count(bits_count), ind(0)
    {
    }

    int read(int num_bits)
    {
        if (num_bits > 16) {
            return -1;
        }
        if (ind + num_bits > bits_count) {
            return -1;
        }
        int result = 0;
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

void renderBits(const uint8_t *input, int bits_count)
{
    BitReader br(input, bits_count);
    while (true) {
        int cmd = br.read(CMD_BITS);
        switch (cmd) {
            case TEXT_CMD: {
                int fontNum = br.read(FONT_BITS);
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int length = br.read(LENGTH_BITS);
                if (fontNum < 0 || x < 0 || y < 0 || length < 0) {
                    return;
                }
                char *text = (char*) malloc(length+1);
                for (int i = 0; i < length; i++) {
                    int ch = br.read(CHAR_BITS);
                    text[i] = ch;
                }
                text[length] = 0;
                printf("Render x=%d y=%d font=%d test=%s\n", x, y, fontNum, text);
                sFONT* font = &Font8;
                if (fontNum == 1) {
                    font = &Font12;
                } else if (fontNum == 2) {
                    font = &Font16;
                } else if (fontNum == 3) {
                    font = &Font20;
                } else if (fontNum == 4) {
                    font = &Font24;
                }
                Paint_DrawString_EN(x, y, text, font, WHITE, BLACK);
                free(text);
                break;
            }
            case RECT_CMD: {
                int x1 = br.read(X_BITS);
                int y1 = br.read(Y_BITS);
                int x2 = br.read(X_BITS);
                int y2 = br.read(Y_BITS);
                if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0) {
                    return;
                }
                printf("Render rect x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2);
                Paint_DrawRectangle(x1, y1, x2, y2, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
                break;
            }
            case CIRCLE_CMD: {
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int r = br.read(R_BITS);
                if (x < 0 || y < 0 || r < 0) {
                    return;
                }
                printf("Render circle x=%d y=%d r=%d\n", x, y, r);
                Paint_DrawCircle(x, y, r, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
                break;
            }
            case LINE_CMD: {
                int x1 = br.read(X_BITS);
                int y1 = br.read(Y_BITS);
                int x2 = br.read(X_BITS);
                int y2 = br.read(Y_BITS);
                if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0) {
                    return;
                }
                printf("Render line x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2);
                Paint_DrawLine(x1, y1, x2, y2, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
                break;
            }
            default:
                return;
        }
    }
}