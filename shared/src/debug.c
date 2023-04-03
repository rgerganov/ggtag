#include "debug.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

static bool DEBUG_ENABLED = false;

void debugEnable(bool enable)
{
    DEBUG_ENABLED = enable;
}

int debug(const char *format, ...)
{
    bool log = DEBUG_ENABLED;
#ifdef DEBUG
    log = true;
#endif
    if (!log) {
        return 0;
    }
    int ret = 0;
    va_list args;
    va_start(args, format);
    ret = vprintf(format, args);
    va_end(args);
    return ret;
}