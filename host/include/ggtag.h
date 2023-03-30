#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Allocates a buffer and encodes the input string into it. The buffer length is returned in 'length'.
// Caller is responsible for freeing the buffer.
uint8_t* encode(const char *input, int *length);

// Returns the last error message from the parser.
// The memory is owned by the parser and should not be freed.
char* getLastError();

// Allocates a bitmap with dimensions [ceil(width/8), height] and renders the specified input string to it.
// Caller is responsible for freeing the bitmap.
uint8_t* render(const char *input, int width, int height);

// Allocates a bitmap with size ceil(width * height / 8) and performs dithering of the specified input image.
// Caller is responsible for freeing the bitmap.
uint8_t* dither(const uint8_t *rgba, int width, int height);

#ifdef __cplusplus
}
#endif