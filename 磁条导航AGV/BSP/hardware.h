#ifndef HARDWARE_H
#define HARDWARE_H
#include "stm32f10x.h"

extern void hardware_init(void);//Ӳ����ʼ��
extern void NVIC_configuration(void);//�ж�����
static void USART_Config(USART_TypeDef* USARTx, u32 rate);//��������

#endif



