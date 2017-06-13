#ifndef HARDWARE_H
#define HARDWARE_H
#include "stm32f10x.h"

extern void hardware_init(void);//硬件初始化
extern void NVIC_configuration(void);//中断配置
static void USART_Config(USART_TypeDef* USARTx, u32 rate);//串口设置

#endif



