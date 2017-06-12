#ifndef RS485COMMUNICATE_H
#define RS485COMMUNICATE_H
#include    "stm32f10x.h"
#include    "stdbool.h"

#define LEFTCTRL 0
#define RIGHTCTRL 1

#define INITLEFTMOTOCTRL { 0x03, 0x06, 0x00, 0x43, 0x00 , 0x00, 0x00, 0x00 }
#define INITRIGHTMOTOCTRL { 0x02, 0x06, 0x00, 0x43, 0x00 , 0x00, 0x00, 0x00 }


//static bool isSendTime(void);
extern void SendSpeedToCtrl(float vx, float w);
static u16 CRC16_table(u8 *puchMsg, u16 usDataLen );

#endif

