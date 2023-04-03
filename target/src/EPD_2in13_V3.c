/*****************************************************************************
* | File      	:  	EPD_V3.c
* | Author      :   Waveshare team
* | Function    :   2.13inch e-paper V3
* | Info        :
*----------------
* |	This version:   V1.1
* | Date        :   2021-10-30
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD.h"
#include "debug.h"

UBYTE WF_PARTIAL_V3[159] =
{
	0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x14,0x0,0x0,0x0,0x0,0x0,0x0,
	0x1,0x0,0x0,0x0,0x0,0x0,0x0,
	0x1,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,
	0x22,0x17,0x41,0x00,0x32,0x36,
};

UBYTE WS_20_30_V3[159] =
{
	0x80,	0x4A,	0x40,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x40,	0x4A,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x80,	0x4A,	0x40,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x40,	0x4A,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0xF,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0xF,	0x0,	0x0,	0xF,	0x0,	0x0,	0x2,
	0xF,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x1,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x22,	0x22,	0x22,	0x22,	0x22,	0x22,	0x0,	0x0,	0x0,
	0x22,	0x17,	0x41,	0x0,	0x32,	0x36
};

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_V3_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(20);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_V3_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_V3_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_V3_ReadBusy(void)
{
    debug("e-Paper busy\r\n");
	while(1)
	{	 //=1 BUSY
		if(DEV_Digital_Read(EPD_BUSY_PIN)==0)
			break;
		DEV_Delay_ms(10);
	}
	DEV_Delay_ms(10);
    debug("e-Paper busy release\r\n");
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_V3_TurnOnDisplay(void)
{
	EPD_V3_SendCommand(0x22); // Display Update Control
	EPD_V3_SendData(0xc7);
	EPD_V3_SendCommand(0x20); // Activate Display Update Sequence
	EPD_V3_ReadBusy();
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_V3_TurnOnDisplay_Partial(void)
{
	EPD_V3_SendCommand(0x22); // Display Update Control
	EPD_V3_SendData(0x0f);	// fast:0x0c, quality:0x0f, 0xcf
	EPD_V3_SendCommand(0x20); // Activate Display Update Sequence
	EPD_V3_ReadBusy();
}

/******************************************************************************
function :	Set lut
parameter:
    lut :   lut data
******************************************************************************/
static void EPD_V3_LUT(UBYTE *lut)
{
	UBYTE count;
	EPD_V3_SendCommand(0x32);
	for(count=0; count<153; count++)
		EPD_V3_SendData(lut[count]);
	EPD_V3_ReadBusy();
}

/******************************************************************************
function :	Send lut data and configuration
parameter:
    lut :   lut data
******************************************************************************/
static void EPD_V3_LUT_by_host(UBYTE *lut)
{
	EPD_V3_LUT((UBYTE *)lut);			//lut
	EPD_V3_SendCommand(0x3f);
	EPD_V3_SendData(*(lut+153));
	EPD_V3_SendCommand(0x03);	// gate voltage
	EPD_V3_SendData(*(lut+154));
	EPD_V3_SendCommand(0x04);	// source voltage
	EPD_V3_SendData(*(lut+155));	// VSH
	EPD_V3_SendData(*(lut+156));	// VSH2
	EPD_V3_SendData(*(lut+157));	// VSL
	EPD_V3_SendCommand(0x2c);		// VCOM
	EPD_V3_SendData(*(lut+158));
}

/******************************************************************************
function :	Setting the display window
parameter:
	Xstart : X-axis starting position
	Ystart : Y-axis starting position
	Xend : End position of X-axis
	Yend : End position of Y-axis
******************************************************************************/
static void EPD_V3_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    EPD_V3_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    EPD_V3_SendData((Xstart>>3) & 0xFF);
    EPD_V3_SendData((Xend>>3) & 0xFF);

    EPD_V3_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    EPD_V3_SendData(Ystart & 0xFF);
    EPD_V3_SendData((Ystart >> 8) & 0xFF);
    EPD_V3_SendData(Yend & 0xFF);
    EPD_V3_SendData((Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
	Xstart : X-axis starting position
	Ystart : Y-axis starting position
******************************************************************************/
static void EPD_V3_SetCursor(UWORD Xstart, UWORD Ystart)
{
    EPD_V3_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    EPD_V3_SendData(Xstart & 0xFF);

    EPD_V3_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    EPD_V3_SendData(Ystart & 0xFF);
    EPD_V3_SendData((Ystart >> 8) & 0xFF);
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_Init(void)
{
	EPD_V3_Reset();
	DEV_Delay_ms(100);

	EPD_V3_ReadBusy();
	EPD_V3_SendCommand(0x12);  //SWRESET
	EPD_V3_ReadBusy();

	EPD_V3_SendCommand(0x01); //Driver output control
	EPD_V3_SendData(0xf9);
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x00);

	EPD_V3_SendCommand(0x11); //data entry mode
	EPD_V3_SendData(0x03);

	EPD_V3_SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
	EPD_V3_SetCursor(0, 0);

	EPD_V3_SendCommand(0x3C); //BorderWavefrom
	EPD_V3_SendData(0x05);

	EPD_V3_SendCommand(0x21); //  Display update control
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x80);

	EPD_V3_SendCommand(0x18); //Read built-in temperature sensor
	EPD_V3_SendData(0x80);

	EPD_V3_ReadBusy();
	EPD_V3_LUT_by_host(WS_20_30_V3);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_Clear(void)
{
	UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_V3_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
	{
        for (UWORD i = 0; i < Width; i++)
		{
            EPD_V3_SendData(0XFF);
        }
    }

	EPD_V3_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
	image : Image data
******************************************************************************/
void EPD_Display(const UBYTE *blackImage, const UBYTE *redImage)
{
	UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

    EPD_V3_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++)
	{
        for (UWORD i = 0; i < Width; i++)
		{
            EPD_V3_SendData(blackImage[i + j * Width]);
        }
    }

	EPD_V3_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and partial refresh
parameter:
	image : Image data
******************************************************************************/
void EPD_Display_Partial(const UBYTE *Image)
{
	UWORD Width, Height;
    Width = (EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1);
    Height = EPD_HEIGHT;

	//Reset
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(1);
    DEV_Digital_Write(EPD_RST_PIN, 1);

	EPD_V3_LUT_by_host(WF_PARTIAL_V3);

	EPD_V3_SendCommand(0x37);
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x40);  ///RAM Ping-Pong enable
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x00);
	EPD_V3_SendData(0x00);

	EPD_V3_SendCommand(0x3C); //BorderWavefrom
	EPD_V3_SendData(0x80);

	EPD_V3_SendCommand(0x22); //Display Update Sequence Option
	EPD_V3_SendData(0xC0);    // Enable clock and  Enable analog
	EPD_V3_SendCommand(0x20);  //Activate Display Update Sequence
	EPD_V3_ReadBusy();

	EPD_V3_SetWindows(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
	EPD_V3_SetCursor(0, 0);

	EPD_V3_SendCommand(0x24);   //Write Black and White image to RAM
    for (UWORD j = 0; j < Height; j++)
	{
        for (UWORD i = 0; i < Width; i++)
		{
			EPD_V3_SendData(Image[i + j * Width]);
		}
	}
	EPD_V3_TurnOnDisplay_Partial();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_Sleep(void)
{
	EPD_V3_SendCommand(0x10); //enter deep sleep
	EPD_V3_SendData(0x01);
	DEV_Delay_ms(100);
}
