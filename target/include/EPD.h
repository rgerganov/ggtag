#pragma once
#include "DEV_Config.h"

// Display resolution
#if defined(EPD_2IN13)
    #define EPD_WIDTH       122
    #define EPD_HEIGHT      250
#elif defined(EPD_3IN52)
    #define EPD_WIDTH       240
    #define EPD_HEIGHT      360
#else
    #error "Unsupported EPD"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void EPD_Init(void);
void EPD_Clear(void);
void EPD_Display(const UBYTE *bImage, const UBYTE *rImage);
void EPD_Display_Partial(const UBYTE *Image);
void EPD_Sleep(void);

#ifdef __cplusplus
}
#endif
