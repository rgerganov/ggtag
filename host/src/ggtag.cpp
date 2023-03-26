#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include "utils.h"
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
    int x;
    int y;
    int width;
    int height;
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

struct ImageCmd {
    int x;
    int y;
    int width;
    int height;
    const char *b64_data;
    int b64_data_len;
};

struct IconCmd {
    int codepoint;
    int x;
    int y;
    int height;
};

struct QRCodeCmd {
    int x;
    int y;
    int pixel_width;
    const char *text;
    int text_len;
};

struct RFIDCmd {
    bool is_hid; // true means HID, false means EM4102
    int id1;
    int id2;
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
        if (!addBits(cmd.x, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y, Y_BITS)) {
            return false;
        }
        // font numbers are zero-based
        if (!addBits(cmd.font-1, FONT_BITS)) {
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
    bool addCmd(const ImageCmd &cmd)
    {
        if (!addBits(IMAGE_CMD, CMD_BITS)) {
            return false;
        }
        if (!addBits(cmd.x, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y, Y_BITS)) {
            return false;
        }
        if (!addBits(cmd.width, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.height, Y_BITS)) {
            return false;
        }
        int decoded_size = cmd.b64_data_len/4*3;
        uint8_t *decoded = (uint8_t*) malloc(decoded_size);
        if (!decoded) {
            return false;
        }
        b64_decode((const uint8_t*)cmd.b64_data, cmd.b64_data_len, decoded);
        int img_size = cmd.width * cmd.height;
        for (int i = 0; i < img_size/8; i++) {
            if (!addBits(decoded[i], 8)) {
                free(decoded);
                return false;
            }
        }
        if (img_size % 8 != 0) {
            if (!addBits(decoded[img_size/8], img_size % 8)) {
                free(decoded);
                return false;
            }
        }
        free(decoded);
        return true;
    }
    bool addCmd(const IconCmd &cmd)
    {
        if (!addBits(ICON_CMD, CMD_BITS)) {
            return false;
        }
        if (!addBits(cmd.x, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y, Y_BITS)) {
            return false;
        }
        if (!addBits(cmd.height, Y_BITS)) {
            return false;
        }
        if (!addBits(cmd.codepoint, ICON_BITS)) {
            return false;
        }
        return true;
    }
    bool addCmd(const RFIDCmd &cmd)
    {
        if (!addBits(RFID_CMD, CMD_BITS)) {
            return false;
        }
        if (!addBits(cmd.is_hid, 1)) {
            return false;
        }
        if (!addBits(cmd.id1, RFID1_BITS)) {
            return false;
        }
        if (!addBits(cmd.id2, RFID2_BITS)) {
            return false;
        }
        return true;
    }
    bool addCmd(const QRCodeCmd &cmd)
    {
        if (!addBits(QRCODE_CMD, CMD_BITS)) {
            return false;
        }
        if (!addBits(cmd.x, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y, Y_BITS)) {
            return false;
        }
        // pixel_width is zero-based
        if (!addBits(cmd.pixel_width-1, QR_PIXEL_WIDTH)) {
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
    bool addCmd(const RectCmd &cmd, bool fill)
    {
        int cmd_bits = fill ? FILL_RECT_CMD : RECT_CMD;
        if (!addBits(cmd_bits, CMD_BITS)) {
            return false;
        }
        if (!addBits(cmd.x, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.y, Y_BITS)) {
            return false;
        }
        if (!addBits(cmd.width, X_BITS)) {
            return false;
        }
        if (!addBits(cmd.height, Y_BITS)) {
            return false;
        }
        return true;
    }
    bool addCmd(const CircleCmd &cmd, bool fill)
    {
        int cmd_bits = fill ? FILL_CIRCLE_CMD : CIRCLE_CMD;
        if (!addBits(cmd_bits, CMD_BITS)) {
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

static char lastError[256];

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
        case 'R':
            *cmd = FILL_RECT_CMD;
            break;
        case 'c':
            *cmd = CIRCLE_CMD;
            break;
        case 'C':
            *cmd = FILL_CIRCLE_CMD;
            break;
        case 'l':
            *cmd = LINE_CMD;
            break;
        case 'q':
            *cmd = QRCODE_CMD;
            break;
        case 'i':
            *cmd = IMAGE_CMD;
            break;
        case 'a':
            *cmd = ICON_CMD;
            break;
        case 'f':
            *cmd = RFID_CMD;
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

bool parseHex(const char *input, int *value, int *curr_offset)
{
    int offset = *curr_offset;
    if (!input[offset]) {
        return false;
    }
    int ret = 0;
    while ((input[offset] >= '0' && input[offset] <= '9') ||
           (input[offset] >= 'a' && input[offset] <= 'f') ||
           (input[offset] >= 'A' && input[offset] <= 'F')) {
        int v = 0;
        if (input[offset] >= '0' && input[offset] <= '9') {
            v = input[offset] - '0';
        } else if (input[offset] >= 'a' && input[offset] <= 'f') {
            v = input[offset] - 'a' + 10;
        } else if (input[offset] >= 'A' && input[offset] <= 'F') {
            v = input[offset] - 'A' + 10;
        }
        ret = ret * 16 + v;
        offset++;
    }
    *value = ret;
    *curr_offset = offset;
    return true;
}

// TextCommand: <x>,<y>,<font_num>,<text>
bool parseTextCmd(const char *input, TextCmd *cmd, int *curr_offset)
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
    if (!parseInt(input, &cmd->font, &offset)) {
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

// ImageCommand: <x>,<y>,<width>,<height>,<base64data>
bool parseImageCmd(const char *input, ImageCmd *cmd, int *curr_offset)
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
    if (!parseInt(input, &cmd->width, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->height, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    cmd->b64_data = input + offset;
    cmd->b64_data_len = 0;
    while (input[offset] != '\0' && input[offset] != '\\') {
        cmd->b64_data_len++;
        offset++;
    }
    if (cmd->b64_data_len % 4 != 0) {
        return false;
    }
    *curr_offset = offset;
    return true;
}

// IconCommand: <x>,<y>,<height>,<hex_codepoint>
bool parseIconCmd(const char *input, IconCmd *cmd, int *curr_offset)
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
    if (!parseInt(input, &cmd->height, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    const char* iconName = input + offset;
    int len = 0;
    while ((input[offset] >= 'a' && input[offset] <= 'z') || (input[offset] >= 'A' && input[offset] <= 'Z') || input[offset] == '-') {
        len++;
        offset++;
    }
    cmd->codepoint = get_codepoint(iconName, len);
    if (cmd->codepoint < 0) {
        return false;
    }
    *curr_offset = offset;
    return true;
}

bool parseLiteral(const char *input, const char *literal, int *curr_offset)
{
    int offset = *curr_offset;
    while (*literal) {
        if (input[offset++] != *literal++) {
            return false;
        }
    }
    *curr_offset = offset;
    return true;
}

// RFIDCommand: em|hid,<id1_hex>,<id2_hex>
bool parseRFIDCmd(const char *input, RFIDCmd *cmd, int *curr_offset)
{
    int offset = *curr_offset;
    if (!input[offset]) {
        return false;
    }
    if (!parseLiteral(input, "em,", &offset)) {
        if (!parseLiteral(input, "hid,", &offset)) {
            // expected "em," or "hid," prefix
            return false;
        } else {
            cmd->is_hid = true;
        }
    } else {
        cmd->is_hid = false;
    }

    if (!parseHex(input, &cmd->id1, &offset)) {
        return false;
    }
    if (!cmd->is_hid && (cmd->id1 < 0 || cmd->id1 > 0xff)) {
        // TODO: set error message
        return false;
    }
    if (cmd->is_hid && (cmd->id1 < 0 || cmd->id1 > 0x1fff)) {
        // TODO: set error message
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseHex(input, &cmd->id2, &offset)) {
        return false;
    }
    *curr_offset = offset;
    return true;
}

// QRCodeCommand: <x>,<y>,<pixel_width>,<text>
bool parseQRCodeCmd(const char *input, QRCodeCmd *cmd, int *curr_offset)
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
    if (!parseInt(input, &cmd->pixel_width, &offset)) {
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

// RectCommand: <x>,<y>,<width>,<height>
bool parseRectCmd(const char *input, RectCmd *cmd, int *curr_offset)
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
    if (!parseInt(input, &cmd->width, &offset)) {
        return false;
    }
    if (input[offset++] != ',') {
        return false;
    }
    if (!parseInt(input, &cmd->height, &offset)) {
        return false;
    }
    *curr_offset = offset;
    return true;
}

// CircleCommand: <x>,<y>,<radius>
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

// LineCommand: <x1>,<y1>,<x2>,<y2>
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
        sprintf(lastError, "Invalid command");
        return false;
    }
    switch (cmd) {
        case TEXT_CMD:
            TextCmd text_cmd;
            if (!parseTextCmd(input, &text_cmd, &offset)) {
                sprintf(lastError, "Failed to parse Text command");
                return false;
            }
            //printTextCmd(&text_cmd);
            if (!buf->addCmd(text_cmd)) {
                return false;
            }
            break;
        case RECT_CMD:
        case FILL_RECT_CMD:
            RectCmd rect_cmd;
            if (!parseRectCmd(input, &rect_cmd, &offset)) {
                sprintf(lastError, "Failed to parse Rect command");
                return false;
            }
            if (!buf->addCmd(rect_cmd, cmd == FILL_RECT_CMD)) {
                return false;
            }
            break;
        case CIRCLE_CMD:
        case FILL_CIRCLE_CMD:
            CircleCmd circle_cmd;
            if (!parseCircleCmd(input, &circle_cmd, &offset)) {
                sprintf(lastError, "Failed to parse Circle command");
                return false;
            }
            if (!buf->addCmd(circle_cmd, cmd == FILL_CIRCLE_CMD)) {
                return false;
            }
            break;
        case LINE_CMD:
            LineCmd line_cmd;
            if (!parseLineCmd(input, &line_cmd, &offset)) {
                sprintf(lastError, "Failed to parse Line command");
                return false;
            }
            if (!buf->addCmd(line_cmd)) {
                return false;
            }
            break;
        case QRCODE_CMD:
            QRCodeCmd qrcode_cmd;
            if (!parseQRCodeCmd(input, &qrcode_cmd, &offset)) {
                sprintf(lastError, "Failed to parse QRcode command");
                return false;
            }
            if (!buf->addCmd(qrcode_cmd)) {
                return false;
            }
            break;
        case IMAGE_CMD:
            ImageCmd image_cmd;
            if (!parseImageCmd(input, &image_cmd, &offset)) {
                sprintf(lastError, "Failed to parse Image command");
                return false;
            }
            if (!buf->addCmd(image_cmd)) {
                return false;
            }
            break;
        case ICON_CMD:
            IconCmd icon_cmd;
            if (!parseIconCmd(input, &icon_cmd, &offset)) {
                sprintf(lastError, "Failed to parse Icon command");
                return false;
            }
            if (!buf->addCmd(icon_cmd)) {
                return false;
            }
            break;
        case RFID_CMD:
            RFIDCmd rfid_cmd;
            if (!parseRFIDCmd(input, &rfid_cmd, &offset)) {
                sprintf(lastError, "Failed to parse RFID command");
                return false;
            }
            if (!buf->addCmd(rfid_cmd)) {
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
        if (parse(input, &buf, &offset)) {
            // clear the error message if parsing was successful
            sprintf(lastError, "OK");
        }
        if (buf.size() == 0) {
            return 0;
        }
        //buf.dumpBytes();
        uint8_t *ret = (uint8_t*) calloc(buf.size()+2, 1);
        if (!ret) {
            return 0;
        }
        // the first two byte are the total length of the encoded data
        ret[0] = buf.size() / 256;
        ret[1] = buf.size() % 256;
        // followed by the encoded data
        memcpy(ret+2, buf.buffer, buf.size());
        *length = buf.size() + 2;
        return ret;
    }

    // Returns the last error message from the parser.
    // The memory is owned by the parser and should not be freed.
    char* getLastError()
    {
        return lastError;
    }

    // Allocates a bitmap with dimensions [ceil(width/8), height] and renders the specified input string to it.
    // Caller is responsible for freeing the bitmap.
    uint8_t* render(const char *input, int width, int height)
    {
        if (!input || width <= 0 || height <= 0) {
            return 0;
        }
        int offset = 0;
        BitBuffer buf;
        if (parse(input, &buf, &offset)) {
            // clear the error message if parsing was successful
            sprintf(lastError, "OK");
        }
        if (buf.size() == 0) {
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

    // Allocates a bitmap with size ceil(width * height / 8) and performs dithering of the specified input image.
    // Caller is responsible for freeing the bitmap.
    uint8_t* dither(const uint8_t *rgba, int width, int height)
    {
        if (!rgba || width <= 0 || height <= 0) {
            return 0;
        }
        float *gray = (float*) malloc(width * height * sizeof(float));
        if (!gray) {
            return 0;
        }
        for (int i = 0; i < width * height; i++) {
            gray[i] = 0.2126 * rgba[i*4] + 0.7152 * rgba[i*4+1] + 0.0722 * rgba[i*4+2];
        }
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float oldpixel = gray[y * width + x];
                float newpixel = (oldpixel > 127) ? 255 : 0;
                gray[y * width + x] = newpixel;
                float quant_error = oldpixel - newpixel;
                if (x < width - 1) {
                    gray[y * width + x + 1] += quant_error * 7.0 / 16.0;
                }
                if (x > 0 && y < height - 1) {
                    gray[(y + 1) * width + x - 1] += quant_error * 3.0 / 16.0;
                }
                if (y < height - 1) {
                    gray[(y + 1) * width + x] += quant_error * 5.0 / 16.0;
                }
                if (x < width - 1 && y < height - 1) {
                    gray[(y + 1) * width + x + 1] += quant_error * 1.0 / 16.0;
                }
            }
        }
        int length = width * height / 8;
        if (width * height % 8 != 0) {
            length += 1;
        }
        uint8_t *bitmap = (uint8_t*) malloc(length);
        if (!bitmap) {
            free(gray);
            return 0;
        }
        memset(bitmap, 0, length);
        for (int i = 0; i < width * height; i++) {
            if (gray[i] < 127) {
                bitmap[i / 8] |= 1 << (7 - i % 8);
            }
        }
        free(gray);
        return bitmap;
    }
}

int main(int argc, char *argv[])
{
    int len;
    const char *input = "\\t1,30,10,Hello world\\t2,40,30,ggtag\\t3,50,103,LZ2RZG\\a63357,10,10,50";
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
    const char *b64 = "aGVsbG8=";
    uint8_t data[128] = {0};
    b64_decode((const uint8_t*) b64, strlen(b64), data);
    printf("Decoded data: %s\n", data);
    return 0;
}