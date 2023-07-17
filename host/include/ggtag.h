#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Allocates a buffer and encodes the input string into it. The buffer length is returned in 'length'.
// Caller is responsible for freeing the buffer.
uint8_t* encode(const char *input, int *length);

// Returns the last error message from the parser.
// The memory is owned by the parser and should not be freed.
char* getLastError();

// Allocates a bitmap with dimensions [ceil(width/8), height] and renders the tag produced by the specified input string.
// Caller is responsible for freeing the bitmap.
uint8_t* render(const char *input, int width, int height);

// Allocates a bitmap with dimensions [ceil(width/8), height] and renders the tag produced by the specified bit buffer.
// Caller is responsible for freeing the bitmap.
uint8_t* renderBuffer(const uint8_t *buf, int bits_count, int width, int height);

// Converts the specified RGBA image to a monochrome bitmap.
// Allocates a bitmap with size ceil(width * height / 8) and renders the image into it.
// If dither is true, the image is dithered using the Floyd-Steinberg algorithm.
// Caller is responsible for freeing the bitmap.
uint8_t* monoimage(const uint8_t *rgba, int width, int height, bool dither);

#ifdef __cplusplus
}
#endif