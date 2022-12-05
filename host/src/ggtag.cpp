#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include "protocol.h"
#include "GUI_Paint.h"

struct TextCmd {
    int x;
    int y;
    int font;
    const char *text;
    int text_len;
};

struct RectCmd {
    int x1;
    int y1;
    int x2;
    int y2;
};

struct CircleCmd {
    int x;
    int y;
    int r;
};

struct LineCmd {
    int x1;
    int y1;
    int x2;
    int y2;
};

struct QRCodeCmd {
    int x;
    int y;
    int pixel_width;
    const char *text;
    int text_len;
};

struct BitBuffer {
    uint8_t *buffer;
    int capacity; // capacity in bytes
    int ind; // current bit index

    BitBuffer() {
        ind = 0;
        this->capacity = 4;
        buffer = (uint8_t*) calloc(this->capacity, 1);
    }
    ~BitBuffer() {
        free(buffer);
        buffer = 0;
    }

    // Return the size of the buffer in bytes
    int size() {
        int ret = ind / 8;
        if (ind % 8 != 0) {
            ret++;
        }
        return ret;
    }

    bool addBits(uint32_t value, int num_bits)
    {
        if (!buffer || num_bits > 32) {
            return false;
        }
        if (ind + num_bits > capacity * 8) {
            buffer = (uint8_t*) realloc(buffer, capacity * 2);
            if (!buffer) {
                return false;
            }
            memset(buffer + capacity, 0, capacity);
            capacity *= 2;
        }
        for (int i = 0; i < num_bits; i++) {
            if (value & (1 << (num_bits - i - 1))) {
                buffer[ind / 8] |= (1 << (7 - (ind % 8)));
            }
            ind++;
        }
        return true;
    }
    bool addCmd(const TextCmd &cmd)
    {
        if (!addBits(TEXT_CMD, CMD_BITS)) {
            return false;
        }
        // font numbers are zero-based
        if (!addBits(cmd.font-1, FONT_BITS)) {
            return false;
        }
        if (!addBits(cmd.x, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y, Y_BITS)) {
            return false;
        }
        if (!addBits(cmd.text_len, LENGTH_BITS)) {
            return false;
        }
        const char *ptr = cmd.text;
        for (int i = 0; i < cmd.text_len; i++) {
            if (!addBits(*ptr, CHAR_BITS)) {
                return false;
            }
            if (*ptr == '\\') {
                ptr++;
            }
            ptr++;
        }
        return true;
    }
    bool addCmd(const QRCodeCmd &cmd)
    {
        if (!addBits(QRCODE_CMD, CMD_BITS)) {
            return false;
        }
        // pixel_width is zero-based
        if (!addBits(cmd.pixel_width-1, QR_PIXEL_WIDTH)) {
            return false;
        }
        if (!addBits(cmd.x, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y, Y_BITS)) {
            return false;
        }
        if (!addBits(cmd.text_len, LENGTH_BITS)) {
            return false;
        }
        const char *ptr = cmd.text;
        for (int i = 0; i < cmd.text_len; i++) {
            if (!addBits(*ptr, CHAR_BITS)) {
                return false;
            }
            if (*ptr == '\\') {
                ptr++;
            }
            ptr++;
        }
        return true;
    }
    bool addCmd(const RectCmd &cmd)
    {
        if (!addBits(RECT_CMD, CMD_BITS)) {
            return false;
        }
        if (!addBits(cmd.x1, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y1, Y_BITS)) {
            return false;
        }
        if (!addBits(cmd.x2, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y2, Y_BITS)) {
            return false;
        }
        return true;
    }
    bool addCmd(const CircleCmd &cmd)
    {
        if (!addBits(CIRCLE_CMD, CMD_BITS)) {
            return false;
        }
        if (!addBits(cmd.x, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y, Y_BITS)) {
            return false;
        }
        if (!addBits(cmd.r, R_BITS)) {
            return false;
        }
        return true;
    }
    bool addCmd(const LineCmd &cmd)
    {
        if (!addBits(LINE_CMD, CMD_BITS)) {
            return false;
        }
        if (!addBits(cmd.x1, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y1, Y_BITS)) {
            return false;
        }
        if (!addBits(cmd.x2, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y2, Y_BITS)) {
            return false;
        }
        return true;
    }
    void dumpBits()
    {
        printf("Bit count: %d\n", ind);
        printf("Bits: ");
        for (int i = 0; i < ind; i++) {
            if (i % 8 == 0) {
                printf(" ");
            }
            if (buffer[i / 8] & (1 << (7 - (i % 8)))) {
                printf("1");
            } else {
                printf("0");
            }
        }
        printf("\n");
    }

    void dumpBytes()
    {
        int size = this->size();
        printf("Bytes count: %d\n", size);
        printf("Bytes: ");
        for (int i = 0; i < size; i++) {
            printf("%02x ", buffer[i]);
        }
        printf("\n");
    }
};

bool parseCommand(const char *input, int *cmd, int *curr_offset)
{
    int offset = *curr_offset;
    if (!input[offset]) {
        *cmd = EOF_CMD;
        return true;
    }
    if (input[offset++] != '\\') {
        return false;
    }
    switch (input[offset]) {
        case 't':
            *cmd = TEXT_CMD;
            break;
        case 'r':
            *cmd = RECT_CMD;
            break;
        case 'c':
            *cmd = CIRCLE_CMD;
            break;
        case 'l':
            *cmd = LINE_CMD;
            break;
        case 'q':
            *cmd = QRCODE_CMD;
            break;
        default:
            return false;
    }
    *curr_offset = offset + 1;
    return true;
}

bool parseInt(const char *input, int *value, int *curr_offset)
{
    int offset = *curr_offset;
    if (!input[offset] || input[offset] < '0' || input[offset] > '9') {
        return false;
    }
    int ret = 0;
    while (input[offset] >= '0' && input[offset] <= '9') {
        ret = ret * 10 + (input[offset] - '0');
        offset++;
    }
    *value = ret;
    *curr_offset = offset;
    return true;
}

bool parseTextCmd(const char *input, TextCmd *cmd, int *curr_offset)
{
    int offset = *curr_offset;
    if (!input[offset]) {
        return false;
    }
    if (!parseInt(input, &cmd->font, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->x, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->y, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    cmd->text = input + offset;
    cmd->text_len = 0;
    while (input[offset] != '\0') {
        if (input[offset] == '\\') {
            if (input[offset+1] == '\\') {
                offset += 1;
            } else {
                break;
            }
        }
        cmd->text_len++;
        offset++;
    }
    *curr_offset = offset;
    return true;
}

bool parseQRCodeCmd(const char *input, QRCodeCmd *cmd, int *curr_offset)
{
    int offset = *curr_offset;
    if (!input[offset]) {
        return false;
    }
    if (!parseInt(input, &cmd->pixel_width, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->x, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->y, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    cmd->text = input + offset;
    cmd->text_len = 0;
    while (input[offset] != '\0') {
        if (input[offset] == '\\') {
            if (input[offset+1] == '\\') {
                offset += 1;
            } else {
                break;
            }
        }
        cmd->text_len++;
        offset++;
    }
    *curr_offset = offset;
    return true;
}

bool parseRectCmd(const char *input, RectCmd *cmd, int *curr_offset)
{
    int offset = *curr_offset;
    if (!input[offset]) {
        return false;
    }
    if (!parseInt(input, &cmd->x1, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->y1, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->x2, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->y2, &offset)) {
        return false;
    }
    *curr_offset = offset;
    return true;
}

bool parseCircleCmd(const char *input, CircleCmd *cmd, int *curr_offset)
{
    int offset = *curr_offset;
    if (!input[offset]) {
        return false;
    }
    if (!parseInt(input, &cmd->x, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->y, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->r, &offset)) {
        return false;
    }
    *curr_offset = offset;
    return true;
}

bool parseLineCmd(const char *input, LineCmd *cmd, int *curr_offset)
{
    int offset = *curr_offset;
    if (!input[offset]) {
        return false;
    }
    if (!parseInt(input, &cmd->x1, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->y1, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->x2, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->y2, &offset)) {
        return false;
    }
    *curr_offset = offset;
    return true;
}

void printTextCmd(TextCmd *cmd)
{
    printf("TextCmd: font=%d, x=%d, y=%d, text=", cmd->font, cmd->x, cmd->y);
    for (int i = 0; i < cmd->text_len; i++) {
        printf("%c", cmd->text[i]);
    }
    printf("\n");
}

bool parse(const char *input, BitBuffer *buf, int *curr_offset)
{
    int cmd;
    int offset = *curr_offset;
    if (!parseCommand(input, &cmd, &offset)) {
        return false;
    }
    switch (cmd) {
        case TEXT_CMD:
            TextCmd text_cmd;
            if (!parseTextCmd(input, &text_cmd, &offset)) {
                return false;
            }
            printTextCmd(&text_cmd);
            if (!buf->addCmd(text_cmd)) {
                return false;
            }
            break;
        case RECT_CMD:
            RectCmd rect_cmd;
            if (!parseRectCmd(input, &rect_cmd, &offset)) {
                return false;
            }
            if (!buf->addCmd(rect_cmd)) {
                return false;
            }
            break;
        case CIRCLE_CMD:
            CircleCmd circle_cmd;
            if (!parseCircleCmd(input, &circle_cmd, &offset)) {
                return false;
            }
            if (!buf->addCmd(circle_cmd)) {
                return false;
            }
            break;
        case LINE_CMD:
            LineCmd line_cmd;
            if (!parseLineCmd(input, &line_cmd, &offset)) {
                return false;
            }
            if (!buf->addCmd(line_cmd)) {
                return false;
            }
            break;
        case QRCODE_CMD:
            QRCodeCmd qrcode_cmd;
            if (!parseQRCodeCmd(input, &qrcode_cmd, &offset)) {
                return false;
            }
            if (!buf->addCmd(qrcode_cmd)) {
                return false;
            }
            break;
        case EOF_CMD:
            return true;
    }
    return parse(input, buf, &offset);
}

extern "C" {
    // Allocates a buffer and encodes the input string into it. The buffer length is returned in 'length'.
    // Caller is responsible for freeing the buffer.
    uint8_t* encode(const char *input, int *length)
    {
        BitBuffer buf;
        int offset = 0;
        if (!parse(input, &buf, &offset)) {
            return 0;
        }
        //buf.dumpBytes();
        uint8_t *ret = (uint8_t*) calloc(buf.size()+1, 1);
        if (!ret) {
            return 0;
        }
        // the first byte is the length of the encoded data
        ret[0] = buf.size();
        // followed by the encoded data
        memcpy(ret+1, buf.buffer, buf.size());
        *length = buf.size() + 1;
        return ret;
    }

    // Allocates a bitmap with dimensions [ceil(width/8), height] and renders the specified input string to it.
    // Caller is responsible for freeing the bitmap.
    uint8_t* render(const char *input, int width, int height)
    {
        int offset = 0;
        BitBuffer buf;
        if (!parse(input, &buf, &offset)) {
            return 0;
        }
        printf("Bit buffer length: %d\n", buf.ind);
        int w = width / 8;
        if (width % 8 != 0) {
            w += 1;
        }
        int total = w * height;
        uint8_t *bitmap = (uint8_t*) malloc(total);
        if (!bitmap) {
            return 0;
        }
        memset(bitmap, 0, total);
        Paint_NewImage(bitmap, width, height, 0, WHITE);
        Paint_Clear(WHITE);
        renderBits(buf.buffer, buf.ind);
        return bitmap;
    }
}

int main(int argc, char *argv[])
{
    int len;
    const char *input = "\\t1,30,10,Hello world\\t2,40,30,ggtag\\t3,50,103,LZ2RZG";
    uint8_t *ptr = encode(input, &len);
    printf("Encoded data length: %d\n", len);
    printf("Encoded data: ");
    for (int i = 0; i < len; i++) {
        printf("%02x ", ptr[i]);
    }
    printf("\n");
    free(ptr);
    ptr = render(input, 250, 122);
    free(ptr);
    return 0;
}