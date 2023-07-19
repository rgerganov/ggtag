# ggtag protocol

`ggtag` is using the same data protocol for sound and USB programming. The protocol encodes a series of commands into binary stream which is prefixed by two bytes which specify the size of the stream. The entire payload has the following format:

```
[SIZE (2 bytes)] [CMD1] [CMD2] [CMD3] ... [CMDn]
```

The `SIZE` is encoded as a big-endian 16-bit unsigned integer. At most `SIZE` bytes follow which represent the commands.

## Commands

Each command has the following format:

```
[CMD_CODE (4 bits)] [ARG1] [ARG2] ... [ARGn]
```

| `CMD_CODE` | Name             |   Description        |
| ---------- |------------------|----------------------|
| `0000`     | `TEXT_CMD`       |Draw text             |
| `0001`     | `RECT_CMD`       |Draw rectangle        |
| `0010`     | `FILL_RECT_CMD`  |Draw filled rectangle |
| `0011`     | `CIRCLE_CMD`     |Draw circle           |
| `0100`     | `FILL_CIRCLE_CMD`|Draw filled circle    |
| `0101`     | `LINE_CMD`       |Draw line             |
| `0110`     | `QRCODE_CMD`     |Draw QR code          |
| `0111`     | `IMAGE_CMD`      |Draw image            |
| `1000`     | `ICON_CMD`       |Draw icon             |
| `1001`     | `RFID_CMD`       |Program RFID          |
| `1010`     | `RLE_IMAGE_CMD`  |Draw RLE image        |

The commands are executed in the order they are received. The display is cleared before the first command is executed.

### `TEXT_CMD`

| ARG# | Description        |Size (bits) |
| ---- |--------------------|------------|
| 1    | X coordinate       | 9          |
| 2    | Y coordinate       | 8          |
| 3    | Font size          | 3          |
| 4    | Text length (N)    | 7          |
| 5    | Char1 (7bit ASCII) | 7          |
| ...  | ...                | ...        |
| 4+N  | CharN (7bit ASCII) | 7          |


__Example:__ Draw "ABC" at position (120,95) with fontSize=3

| CMD_CODE | ARG1      | ARG2     | ARG3| ARG4    | ARG5    | ARG6    | ARG7    |
|----------|-----------|----------|-----|---------|---------|---------|---------|
|`0000`    |`001111000`|`01011111`|`010`|`0000011`|`1000001`|`1000010`|`1000011`|


### `RECT_CMD`

| ARG# | Description        |Size (bits) |
| ---- |--------------------|------------|
| 1    | X coordinate       | 9          |
| 2    | Y coordinate       | 8          |
| 3    | Width              | 9          |
| 4    | Height             | 8          |

__Example:__ Draw rectangle at position (50,30) with width=280 and height=170

| CMD_CODE | ARG1      | ARG2     | ARG3      | ARG4     |
|----------|-----------|----------|-----------|----------|
|`0001`    |`000110010`|`00011110`|`100011000`|`10101010`|

### `FILL_RECT_CMD`

Same as `RECT_CMD`.

### `CIRCLE_CMD`

| ARG# | Description        |Size (bits) |
| ---- |--------------------|------------|
| 1    | X coordinate       | 9          |
| 2    | Y coordinate       | 8          |
| 3    | Radius             | 7          |

__Example:__ Draw circle at position (50,30) with radius=100

| CMD_CODE | ARG1      | ARG2     | ARG3      |
|----------|-----------|----------|-----------|
|`0011`    |`000110010`|`00011110`|`1100100`  |


### `FILL_CIRCLE_CMD`

Same as `CIRCLE_CMD`.

### `LINE_CMD`

| ARG# | Description        |Size (bits) |
| ---- |--------------------|------------|
| 1    | X1 coordinate      | 9          |
| 2    | Y1 coordinate      | 8          |
| 3    | X2 coordinate      | 9          |
| 4    | Y2 coordinate      | 8          |

__Example:__ Draw line from (50,30) to (280,170)

| CMD_CODE | ARG1      | ARG2     | ARG3      | ARG4     |
|----------|-----------|----------|-----------|----------|
|`0101`    |`000110010`|`00011110`|`100011000`|`10101010`|

### `QRCODE_CMD`

| ARG# | Description        |Size (bits) |
| ---- |--------------------|------------|
| 1    | X coordinate       | 9          |
| 2    | Y coordinate       | 8          |
| 3    | Pixel width        | 2          |
| 4    | Text length (N)    | 7          |
| 5    | Char1 (7bit ASCII) | 7          |
| ...  | ...                | ...        |
| 4+N  | CharN (7bit ASCII) | 7          |

__Example:__ Draw QR code at position (50,30) with pixel width=2 and text="ABC"

| CMD_CODE | ARG1      | ARG2     | ARG3      | ARG4    | ARG5    | ARG6    | ARG7    |
|----------|-----------|----------|-----------|---------|---------|---------|---------|
|`0110`    |`000110010`|`00011110`|`10`       |`0000011`|`1000001`|`1000010`|`1000011`|

### `IMAGE_CMD`

| ARG# | Description        |Size (bits) |
| ---- |--------------------|------------|
| 1    | X coordinate       | 9          |
| 2    | Y coordinate       | 8          |
| 3    | Image width (W)    | 9          |
| 4    | Image height (H)   | 8          |
| 5    | Bitmap             | W*H        |

Consider the following 7x9 image (`*` = black, `.` = white)

```
*******
*.....*
*.....*
*.***.*
*.....*
*.***.*
*.....*
*.....*
*******
```

__Example:__ Draw the image above at position (50,30)

| CMD_CODE | ARG1      | ARG2     | ARG3      | ARG4     | ARG5                                                            |
|----------|-----------|----------|-----------|----------|-----------------------------------------------------------------|
|`0111`    |`000110010`|`00011110`|`000000111`|`00001001`|`111111110000011000001101110110000011011101100000110000011111111`|


### `ICON_CMD`

| ARG# | Description        |Size (bits) |
| ---- |--------------------|------------|
| 1    | X coordinate       | 9          |
| 2    | Y coordinate       | 8          |
| 3    | Icon height        | 8          |
| 4    | Icon codepoint     | 16         |

__Example:__ Draw the FontAwesome icon with codepoint `0xf552` at position (50,30) with height=40

| CMD_CODE | ARG1      | ARG2     | ARG3      | ARG4             |
|----------|-----------|----------|-----------|------------------|
|`1000`    |`000110010`|`00011110`|`00101000` |`1111010111010010`|

### `RFID_CMD`

| ARG# | Description                |Size (bits) |
| ---- |----------------------------|------------|
| 1    | Type (`0`=EM4102, `1`=HID) | 1          |
| 2    | ID                         | 45         |

If type is EM4102, the ID argument is interpreted in the following way:

```
| unused (5 bits) | manufacturer ID (8 bits) | unique ID (32 bits) |
```

If type is HID, the ID argument is interpreted in the following way:

```
| manufacturer code (20 bits) | site code (8 bits) | unique id (16 bits) | parity (1 bit) |
```

__Example:__ Program RFID to be EM4102 with manufacturer ID=0x07 and unique ID=0xad30b1

| CMD_CODE | ARG1 | ARG2                                          |
|----------|------|-----------------------------------------------|
|`1001`    | `0`  |`000000000011100000000101011010011000010110001`|


### `RLE_IMAGE_CMD`

| ARG# | Description        |Size (bits) |
| ---- |--------------------|------------|
| 1    | X coordinate       | 9          |
| 2    | Y coordinate       | 8          |
| 3    | Image width (W)    | 9          |
| 4    | Image height (H)   | 8          |
| 5    | RLE bitmap         | variable   |

The image is encoded using run-length encoding (RLE). The bitmap is interpreted as a sequence of black and white runs,
left to right, top to bottom. Consider the following 7x9 image (`*` = black, `.` = white):

```
.......
.......
*******
****...
.......
.......
....***
*******
*******
```

We have 14 white pixels, 11 black pixels, 21 white pixels and 17 black pixels. We encode these numbers in base 3:
```
14 = 112(3)
11 = 102(3)
21 = 210(3)
17 = 122(3)
```

We use 2 bits to encode each digit in base 3, so 112(3) is encoded as `010110`.
We use `11` to separate the runs, and we always start with a white run.
The image above is encoded as `010110 11 010010 11 100100 11 011010`.

__Example:__ Draw the image above at position (50,30)

| CMD_CODE | ARG1      | ARG2     | ARG3      | ARG4     | ARG5                           |
|----------|-----------|----------|-----------|----------|--------------------------------|
|`1010`    |`000110010`|`00011110`|`000000111`|`00001001`|`010110110100101110010011011010`|
