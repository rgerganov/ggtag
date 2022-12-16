#include "utils.h"

static const uint8_t b64chars[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void b64_decode(const uint8_t *in, int in_len, uint8_t *out)
{
    uint8_t inv[256] = {0};
    for (int i = 0; i < (int)sizeof(b64chars)-1; i++) {
        inv[b64chars[i]] = (uint8_t) i;
    }
    uint8_t block[4];
    for (int i = 0; i < in_len; i += 4) {
        block[0] = inv[in[i]];
        block[1] = inv[in[i + 1]];
        block[2] = inv[in[i + 2]];
        block[3] = inv[in[i + 3]];

        *out++ = (block[0] << 2) | (block[1] >> 4);
        *out++ = (block[1] << 4) | (block[2] >> 2);
        *out++ = (block[2] << 6) | block[3];
    }
}