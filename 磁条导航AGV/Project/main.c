#include "SZ_STM32F103ZE_LIB.h"
#include "rs485config.h"
#include "hardware.h"
#include "rs485communicate.h"
#include "magserv.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "commtype.h"

#define SPEEDSENDTASKPRIO   1
#define MAGSENSEQUERYPRIO   2
#define SPEEDSENDTASKSTK    500
#define MAGSENSEQUERYSTK    200
#define MAGQUERYMSG	{ 0x52, 0x4D, 0x67, 0x73, 0x77 , 0x5e, 0x69 }

void speed_send_task(void *pvParameters);
void mag_sense_query_task(void *pvParameter);

u8 DEBUGtxbuffer[5] = { 1, 2, 3, 4, 5 };
TaskHandle_t    SpeedSendTaskHandle;//�ٶȷ���������
TaskHandle_t    MagSenseQueryHandle;//�Ŵ�������ѯ���

/*---------------------------------------------------------------------------------------------*/
int main(void)
{
	hardware_init();
	uart_init(COM2, 115200);
	AGV_control_com_config();
	NVIC_configuration();

	/*�����ٶȷ�������*/
	xTaskCreate((TaskFunction_t)speed_send_task,
		(const char*)"SpeedSendTask",
		(u16)SPEEDSENDTASKSTK,
		(void*)NULL,
		(UBaseType_t)SPEEDSENDTASKPRIO,
		(TaskHandle_t *)&SpeedSendTaskHandle);

	/*�����Ŵ�������ѯ����*/
	xTaskCreate((TaskFunction_t)mag_sense_query_task,
		(const char*)"MagSenseQuery",
		(u16)MAGSENSEQUERYSTK,
		(void*)NULL,
		(UBaseType_t)MAGSENSEQUERYPRIO,
		(TaskHandle_t *)&MagSenseQueryHandle);
	vTaskStartScheduler();
}

/*---------------------------------------------------------------------------------------------*/
//�ٶȿ�������
void speed_send_task(void *pvParameters)
{
	pvParameters = (void *)pvParameters;
	while (1)
	{
		mag_to_speed();
		vTaskDelay(50);
	}
}
//�Ŵ�������ѯ����

/*---------------------------------------------------------------------------------------------*/
void mag_sense_query_task(void *pvParameters)
{
	static u8 TxBufferToQueryMagsense[7] = MAGQUERYMSG;  //�ŵ�����������ѯ����
	pvParameters = (void *)pvParameters;
	while (1)
	{
		usart_out(USART1, TxBufferToQueryMagsense, 7);
		vTaskDelay(200);
	}
}

