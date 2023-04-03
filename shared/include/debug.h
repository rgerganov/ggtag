#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void debugEnable(bool enable);

int debug(const char *format, ...);

#ifdef __cplusplus
}
#endif