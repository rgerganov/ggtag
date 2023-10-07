#include <stdio.h>
#include <stdlib.h>
#include "rfid.h"
#include "debug.h"
#include "protocol.h"
#include "GUI_Paint.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

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

static void draw_bits(int x, int y, int offset, int w, int val, int count)
{
    for (int i = 0; i < count; i++) {
        int col = x + (i + offset) % w;
        int row = y + (i + offset) / w;
        Paint_SetPixel(col, row, val ? BLACK : WHITE);
    }
}

static void rle_img_render(BitReader *br, int x, int y, int width, int height)
{
    int divider = (1 << RLE_BITS) - 1;
    uint8_t curr_value = 0;
    int count = 0;
    int offset = 0;
    int img_size = width * height;
    while (count+offset < img_size) {
        int val = br->read(RLE_BITS);
        if (val == divider) {
            draw_bits(x, y, offset, width, curr_value, count);
            curr_value = 1 - curr_value;
            offset += count;
            count = 0;
        } else {
            count = count * divider + val;
        }
    }
    draw_bits(x, y, offset, width, curr_value, count);
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
                if (!text) {
                    printf("Failed to allocate memory for text\n");
                    return;
                }
                for (int i = 0; i < length; i++) {
                    int ch = br.read(CHAR_BITS);
                    text[i] = ch;
                }
                text[length] = 0;
                debug("Render x=%d y=%d font=%d text=%s\n", x, y, fontNum, text);
                sFONT* font = &Font8;
                if (fontNum == 1) {
                    font = &Font12;
                } else if (fontNum == 2) {
                    font = &Font16;
                } else if (fontNum == 3) {
                    font = &Font20;
                } else if (fontNum == 4) {
                    font = &Font24;
                } else if (fontNum == 5) {
                    font = &Font28;
                } else if (fontNum == 6) {
                    font = &Font32;
                } else if (fontNum == 7) {
                    font = &Font36;
                }
                Paint_DrawString_EN(x, y, text, font, WHITE, BLACK);
                free(text);
                break;
            }
            case RECT_CMD:
            case FILL_RECT_CMD: {
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int w = br.read(X_BITS);
                int h = br.read(Y_BITS);
                if (x < 0 || y < 0 || w < 0 || h < 0) {
                    return;
                }
                DRAW_FILL fill = (cmd == RECT_CMD) ? DRAW_FILL_EMPTY : DRAW_FILL_FULL;
                debug("Render rect x=%d y=%d w=%d h=%d fill=%d\n", x, y, w, h, fill);
                Paint_DrawRectangle(x, y, x+w, y+h, BLACK, DOT_PIXEL_1X1, fill);
                break;
            }
            case ELLIPSE_CMD: {
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int rx = br.read(R_BITS);
                int ry = br.read(R_BITS);
                if (x < 0 || y < 0 || rx < 0 || ry < 0) {
                    return;
                }
                debug("Render ellipse x=%d y=%d rx=%d ry=%d\n", x, y, rx, ry);
                Paint_DrawEllipse(x, y, rx, ry, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
                break;
            }
            case CIRCLE_CMD:
            case FILL_CIRCLE_CMD: {
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int r = br.read(R_BITS);
                if (x < 0 || y < 0 || r < 0) {
                    return;
                }
                DRAW_FILL fill = (cmd == CIRCLE_CMD) ? DRAW_FILL_EMPTY : DRAW_FILL_FULL;
                debug("Render circle x=%d y=%d r=%d fill=%d\n", x, y, r, fill);
                Paint_DrawCircle(x, y, r, BLACK, DOT_PIXEL_1X1, fill);
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
                debug("Render line x1=%d y1=%d x2=%d y2=%d\n", x1, y1, x2, y2);
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
                debug("Render qrcode x=%d y=%d text=%s\n", x, y, text);
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
                debug("Render image x=%d y=%d width=%d height=%d\n", x, y, width, height);
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
            case RLE_IMAGE_CMD: {
                int x = br.read(X_BITS);
                int y = br.read(Y_BITS);
                int width = br.read(X_BITS);
                int height = br.read(Y_BITS);
                if (x < 0 || y < 0 || width < 0 || height < 0) {
                    return;
                }
                debug("Render RLE image x=%d y=%d width=%d height=%d\n", x, y, width, height);
                rle_img_render(&br, x, y, width, height);
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
                debug("Render icon codepoint=%x x=%d y=%d height=%d\n", codepoint, x, y, height);
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
            case RFID_CMD: {
                uint8_t is_hid = br.read(1);
                uint16_t id1 = br.read(RFID1_BITS);
                uint32_t id2 = br.read(RFID2_BITS);
                if (is_hid) {
                    debug("Programming HID id1=%x id2=%x\n", id1, id2);
                    program_hid_rfid(id1, id2);
                } else {
                    debug("Programming EM4102 mfr_id=%x uid=%x\n", id1, id2);
                    program_em_rfid(id1, id2);
                }
                break;
            }
            default:
                return;
        }
    }
}