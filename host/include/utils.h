#pragma once
#include <stdint.h>

void b64_decode(const uint8_t *in, int in_len, uint8_t *out);

// Returns the codepoint of the icon with the given name, or -1 if the name is not found.
int get_codepoint(const char *name, int len);
