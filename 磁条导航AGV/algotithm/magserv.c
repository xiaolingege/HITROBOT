#include "magserv.h"
#include "stm32f10x.h"
#include "stdbool.h"
#include "rs485communicate.h"

#define MAGSERVPI   0.05f
#define MAGSENSEHEADER {0x52, 0x4D, 0x67, 0x73, 0x77}
static float StationCalc(const u8 * MagSense);
static  bool isCheckHead(const u8 * head);
static float mag_center_calc(u32 magsenseBuffer);
static float PIDCalc(float err);

const struct _magSenseHeader
{
	u8 pos1;
	u8 pos2;
	u8 pos3;
	u8 pos4;
	u8 pos5;
}magSenseHeader = {0x52, 0x4D, 0x67, 0x73, 0x77};

static float CarSpeedVx = 0;
static float CarSpeedVw = 0;

u8 MagReceiveDate[MAGRCVSIZE] = { 0 };
u8 MagReceiveCount = 0;

/*---------------------------------------------------------------------------------------------*/
static float StationCalc(const u8 * MagSense)
{
	static float StationForReturn = 0;
	u32 magDate = 0;
	if (MagReceiveCount >= 10)
	{
		if (isCheckHead(MagSense))
		{
			magDate = (MagSense[5] << 16) | (MagSense[6] << 8) | (MagSense[7]);
			MagReceiveCount = 0;
			if (0 != magDate)
			{
				StationForReturn = mag_center_calc(magDate);
			}
			else
			{
				CarSpeedVx = 0;
				StationForReturn = 0;
			}
		}
		MagReceiveCount = 0;
	}
	return StationForReturn;
}

/*---------------------------------------------------------------------------------------------*/
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		MagReceiveDate[MagReceiveCount++] = USART_ReceiveData(USART1);
		if (MagReceiveDate[0] != magSenseHeader.pos1)
		{
			MagReceiveCount = 0;
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

/*---------------------------------------------------------------------------------------------*/
static  bool isCheckHead(const u8 * head)
{
	u8 hedbuffer[5] = MAGSENSEHEADER;
	u8 i = 0;
	for (i = 0; i < 5; i++)
	{
		if (head[i] != hedbuffer[i])
		{
			return false;
		}
	}
	return true;
}

/*---------------------------------------------------------------------------------------------*/
void mag_to_speed(void)
{
	CarSpeedVw = PIDCalc(StationCalc(MagReceiveDate));
	SendSpeedToCtrl(CarSpeedVx, CarSpeedVw);
	if (CarSpeedVx < 0.60f)
	{
		CarSpeedVx += 0.01f;
	}
}

/*---------------------------------------------------------------------------------------------*/
static float mag_center_calc(u32 magsenseBuffer)
{
	float weight[16];
	float magCenterForReturn = 0;
	float signal[16];
	u8 count = 0;
	u8 i = 0;
	signal[15] = 1 & magsenseBuffer;
	for (i = 0; i < 16; i++)
	{
		weight[i] = (float)i - 7.5f;
	}
	for (i = 8; i < 16; i++)
	{
		signal[i - 1] = 1 & (magsenseBuffer >> (i));
	}
	for (i = 1; i < 8; i++)
	{
		signal[i - 1] = 1 & (magsenseBuffer >> (16 + i));
	}
	for (i = 0; i < 16; i++)
	{
		if (signal[i])
		{
			count++;
		}
		magCenterForReturn += (weight[i] * signal[i]);
	}
	return (magCenterForReturn / (float)count* 10.0f);
}

/*---------------------------------------------------------------------------------------------*/
static float PIDCalc(float err)
{
	float Control = 0;
	Control = MAGSERVPI * err;
	return  Control;
}






