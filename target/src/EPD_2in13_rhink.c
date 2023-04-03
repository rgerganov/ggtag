#include "EPD.h"
#include "debug.h"

static void EPD_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

static void EPD_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

static void EPD_ReadBusy(void)
{
    debug("e-Paper busy\r\n");
    while(DEV_Digital_Read(EPD_BUSY_PIN) == 0) {      //LOW: busy, HIGH: ready
        DEV_Delay_ms(10);
    }
    debug("e-Paper busy release\r\n");
}

static void EPD_Reset()
{
    for (int i = 0; i < 3; i++) {
        DEV_Digital_Write(EPD_RST_PIN, 0);
        DEV_Delay_ms(20);
        DEV_Digital_Write(EPD_RST_PIN, 1);
        DEV_Delay_ms(20);
    }
}

static void EPD_Refresh()
{
    EPD_SendCommand(0x12); //refresh
    DEV_Delay_ms(10); // the delay here is necessary, 200uS at least
    EPD_ReadBusy();
}

void EPD_Init()
{
    EPD_Reset();
    DEV_Delay_ms(100);

    EPD_SendCommand(0x4D);
    EPD_SendData(0x55);
    EPD_SendCommand(0xA9);
    EPD_SendData(0x25);
    EPD_SendCommand(0xF3);
    EPD_SendData(0x0A);

    EPD_SendCommand(0x04); //power on
    DEV_Delay_ms(10);
    EPD_ReadBusy();

    EPD_SendCommand(0x00); // panel setting register
    EPD_SendData(0xCF);
    DEV_Delay_ms(10);

    // EPD_SendCommand(0x61); // RESOLUTION_SETTING
    // EPD_SendData(EPD_RED_WIDTH);
    // EPD_SendData(EPD_RED_HEIGHT >> 8);
    // EPD_SendData(EPD_RED_HEIGHT & 0xFF);

    // EPD_SendCommand(0x50);
    // EPD_SendData(0x97);
}

void EPD_Clear()
{
    UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_SendCommand(0x10);
    EPD_ReadBusy();
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
        }
    }
    EPD_SendCommand(0x11);
    EPD_SendCommand(0x13);
    EPD_ReadBusy();
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0x00);
        }
    }
    EPD_SendCommand(0x11);
    EPD_Refresh();
}

void EPD_Display(const UBYTE *blackImage, const UBYTE *redImage)
{
    UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_SendCommand(0x10);
    EPD_ReadBusy();
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(blackImage[i + j * Width]);
        }
    }
    EPD_SendCommand(0x11);
    EPD_SendCommand(0x13);
    EPD_ReadBusy();
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(redImage ? redImage[i + j * Width] : 0x00);
        }
    }
    EPD_SendCommand(0x11);
    EPD_Refresh();
}

void EPD_Display_Partial(const UBYTE *Image)
{
    // TODO
}

void EPD_Sleep()
{
    EPD_SendCommand(0X02);  	//power off
    EPD_ReadBusy();
    EPD_SendCommand(0X07);  	//deep sleep
    EPD_SendData(0xA5);
}