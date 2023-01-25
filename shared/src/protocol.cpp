#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"
#include "GUI_Paint.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

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

// Renders the specified codepoint on a 1D bitmap with size ceil(out_width*out_height/8).
// Caller is responsible for freeing the bitmap.
static uint8_t* renderCodepoint(int codepoint, int height, int *out_width, int *out_height)
{
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, fa_solid_900_ttf, 0)) {
        printf("failed to load ttf font\n");
        return 0;
    }
    float scale = stbtt_ScaleForPixelHeight(&info, height);
    int xoff, yoff;
    uint8_t *bytemap = stbtt_GetCodepointBitmap(&info, scale, scale, codepoint, out_width, out_height, &xoff, &yoff);
    if (!bytemap) {
        return 0;
    }
    int bytemap_size = (*out_width) * (*out_height);
    int bitmap_size = bytemap_size / 8;
    if (bytemap_size % 8 != 0) {
        bitmap_size += 1;
    }
    // convert grayscale bytemap to monochrome bitmap
    uint8_t *bitmap = (uint8_t*) calloc(bitmap_size, 1);
    for (int i = 0; i < bytemap_size; i++) {
        int bit = bytemap[i] > 127 ? 1 : 0;
        int byte_n = i / 8;
        int bit_n = 7 - (i % 8);
        bitmap[byte_n] |= bit << bit_n;
    }
    free(bytemap);
    return bitmap;
}

void renderBits(const uint8_t *input, int bits_count)
{
    BitReader br(input, bits_count);
    while (true) {
        int cmd = br.read(CMD_BITS);
        switch (cmd) {
            case TEXT_CMD: {
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int fontNum = br.read(FONT_BITS);
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
                printf("Render x=%d y=%d font=%d text=%s\n", x, y, fontNum, text);
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
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int w = br.read(X_BITS);
                int h = br.read(Y_BITS);
                if (x < 0 || y < 0 || w < 0 || h < 0) {
                    return;
                }
                printf("Render rect x=%d y=%d w=%d h=%d\n", x, y, w, h);
                Paint_DrawRectangle(x, y, x+w, y+h, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
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
            case QRCODE_CMD: {
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int pixel_width = br.read(QR_PIXEL_WIDTH);
                int length = br.read(LENGTH_BITS);
                if (x < 0 || y < 0 || length < 0) {
                    return;
                }
                char *text = (char*) malloc(length+1);
                for (int i = 0; i < length; i++) {
                    int ch = br.read(CHAR_BITS);
                    text[i] = ch;
                }
                text[length] = 0;
                printf("Render qrcode x=%d y=%d text=%s\n", x, y, text);
                DOT_PIXEL dot_pixel = DOT_PIXEL_1X1;
                if (pixel_width == 1) {
                    dot_pixel = DOT_PIXEL_2X2;
                } else if (pixel_width == 2) {
                    dot_pixel = DOT_PIXEL_3X3;
                } else if (pixel_width == 3) {
                    dot_pixel = DOT_PIXEL_4X4;
                }
                Paint_DrawQRCode(x, y, text, dot_pixel, BLACK);
                free(text);
                break;
            }
            case IMAGE_CMD: {
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int width = br.read(X_BITS);
                int height = br.read(Y_BITS);
                if (x < 0 || y < 0 || width < 0 || height < 0) {
                    return;
                }
                printf("Render image x=%d y=%d width=%d height=%d\n", x, y, width, height);
                for (int i = 0; i < height; i++) {
                    for (int j = 0; j < width; j++) {
                        int color = br.read(1);
                        if (color < 0) {
                            return;
                        }
                        Paint_SetPixel(x + j, y + i, color ? BLACK : WHITE);
                    }
                }
                break;
            }
            case ICON_CMD: {
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int height = br.read(Y_BITS);
                int codepoint = br.read(ICON_BITS);
                if (codepoint < 0 || x < 0 || y < 0 || height < 0) {
                    return;
                }
                printf("Render icon codepoint=%x x=%d y=%d height=%d\n", codepoint, x, y, height);
                int w, h;
                uint8_t *cp = renderCodepoint(codepoint, height, &w, &h);
                BitReader br(cp, w * h);
                for (int i = 0; i < h; i++) {
                    for (int j = 0; j < w; j++) {
                        int color = br.read(1);
                        if (color < 0) {
                            free(cp);
                            return;
                        }
                        Paint_SetPixel(x + j, y + i, color ? BLACK : WHITE);
                    }
                }
                free(cp);
                break;
            }
            default:
                return;
        }
    }
}