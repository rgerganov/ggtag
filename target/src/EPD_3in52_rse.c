#include "EPD.h"
#include "Debug.h"

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
    Debug("e-Paper busy\r\n");
    while(DEV_Digital_Read(EPD_BUSY_PIN) == 1) {      //LOW: ready, HIGH: busy
        DEV_Delay_ms(10);
    }
    Debug("e-Paper busy release\r\n");
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

    // EPD_SendCommand(0x01);     // POWER SETTING   PWR
    // EPD_SendData(0x03);      //  x x x x x x VDS_EN VDG_EN
    // EPD_SendData(0x10);      //  x x x VCOM_SLWE VGH[3:0]   VGH=20V, VGL=-20V
    // EPD_SendData(0x3F);      //  x x VSH[5:0]  VSH = 15V
    // EPD_SendData(0x3F);      //  x x VSL[5:0]  VSL=-15V
    // EPD_SendData(0x14);      //  OPTEN VDHR[6:0]  VHDR=6.4V

    // EPD_SendCommand(0x03);     // POWER OFF sequence setting    PFS
    // EPD_SendData(0x00);      // x x T_VDS_OFF[1:0] x x x x                 // T_VDS_OFF[1:0] 00=1 frame; 01=2 frame; 10=3 frame; 11=4 fram

    // EPD_SendCommand(0x06);     // booster soft start   BTST
    // EPD_SendData(0x17);      //  BT_PHA[7:0]
    // EPD_SendData(0x17);      //  BT_PHB[7:0]
    // EPD_SendData(0x17);      //  x x BT_PHC[5:0]

    EPD_SendCommand(0x04); //power on
    DEV_Delay_ms(10);
    EPD_ReadBusy();

    EPD_SendCommand(0x00); // panel setting register
    EPD_SendData(0x1F);
    EPD_SendData(0x0D);
    DEV_Delay_ms(10);

    EPD_SendCommand(0x61); // RESOLUTION_SETTING
    EPD_SendData(EPD_WIDTH);
    EPD_SendData(EPD_HEIGHT >> 8);
    EPD_SendData(EPD_HEIGHT & 0xFF);

    // EPD_SendCommand(0x50);
    // EPD_SendData(0x97);
}

void EPD_Clear()
{
    UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
        }
    }
    EPD_SendCommand(0x11);
    EPD_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
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
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(0xFF);
        }
    }
    EPD_SendCommand(0x11);
    EPD_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_SendData(blackImage[i + j * Width]);
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