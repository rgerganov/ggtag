#include "EPD_2in13.h"
#include "Debug.h"

static void EPD_Rhink_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

static void EPD_Rhink_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

static void EPD_Rhink_ReadBusy(void)
{
    Debug("e-Paper busy\r\n");
    while(DEV_Digital_Read(EPD_BUSY_PIN) == 0) {      //LOW: busy, HIGH: ready
        DEV_Delay_ms(10);
    }
    Debug("e-Paper busy release\r\n");
}

static void EPD_Rhink_Reset()
{
    for (int i = 0; i < 3; i++) {
        DEV_Digital_Write(EPD_RST_PIN, 0);
        DEV_Delay_ms(20);
        DEV_Digital_Write(EPD_RST_PIN, 1);
        DEV_Delay_ms(20);
    }
}

static void EPD_Rhink_Refresh()
{
    EPD_Rhink_SendCommand(0x12); //refresh
    DEV_Delay_ms(10); // the delay here is necessary, 200uS at least
    EPD_Rhink_ReadBusy();
}

void EPD_2in13_Init()
{
    EPD_Rhink_Reset();
    DEV_Delay_ms(100);

    EPD_Rhink_SendCommand(0x04); //power on
    DEV_Delay_ms(10);
    EPD_Rhink_ReadBusy();

    EPD_Rhink_SendCommand(0x00); // panel setting register
    EPD_Rhink_SendData(0xCF);
    DEV_Delay_ms(10);

    // EPD_Rhink_SendCommand(0x61); // RESOLUTION_SETTING
    // EPD_Rhink_SendData(EPD_RED_WIDTH);
    // EPD_Rhink_SendData(EPD_RED_HEIGHT >> 8);
    // EPD_Rhink_SendData(EPD_RED_HEIGHT & 0xFF);

    // EPD_Rhink_SendCommand(0x50);
    // EPD_Rhink_SendData(0x97);
}

void EPD_2in13_Clear()
{
    UWORD Width, Height;
    Width = (EPD_2in13_WIDTH % 8 == 0)? (EPD_2in13_WIDTH / 8 ): (EPD_2in13_WIDTH / 8 + 1);
    Height = EPD_2in13_HEIGHT;

    EPD_Rhink_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_Rhink_SendData(0xFF);
        }
    }
    EPD_Rhink_SendCommand(0x11);
    EPD_Rhink_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_Rhink_SendData(0x00);
        }
    }
    EPD_Rhink_SendCommand(0x11);
    EPD_Rhink_Refresh();
}

void EPD_2in13_Display(const UBYTE *blackImage, const UBYTE *redImage)
{
    UWORD Width, Height;
    Width = (EPD_2in13_WIDTH % 8 == 0)? (EPD_2in13_WIDTH / 8 ): (EPD_2in13_WIDTH / 8 + 1);
    Height = EPD_2in13_HEIGHT;

    EPD_Rhink_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_Rhink_SendData(blackImage[i + j * Width]);
        }
    }
    EPD_Rhink_SendCommand(0x11);
    EPD_Rhink_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_Rhink_SendData(redImage ? redImage[i + j * Width] : 0x00);
        }
    }
    EPD_Rhink_SendCommand(0x11);
    EPD_Rhink_Refresh();
}

void EPD_2in13_Display_Partial(const UBYTE *Image)
{
    // TODO
}

void EPD_2in13_Sleep()
{
    EPD_Rhink_SendCommand(0X02);  	//power off
    EPD_Rhink_ReadBusy();
    EPD_Rhink_SendCommand(0X07);  	//deep sleep
    EPD_Rhink_SendData(0xA5);
}