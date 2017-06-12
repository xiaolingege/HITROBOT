#ifndef RS485CONFIG_H
#define RS485_CONFIG_H
/*-----------------------------*/
#include    "stm32f10x.h"
extern void AGV_control_com_config(void);
extern void usart_out(USART_TypeDef* USARTx, uint8_t *Data, uint16_t Len);;


/*-----------------------------*/
#endif
