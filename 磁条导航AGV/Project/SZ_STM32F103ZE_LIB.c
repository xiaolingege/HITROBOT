/********************   (C) COPYRIGHT 2013 www.armjishu.com   ********************
 * �ļ���  ��SZ_STM32F103ZE_LIB.c
 * ����    ���ṩSTM32F103ZE����III�ſ�����Ŀ⺯��
 * ʵ��ƽ̨��STM32���ۿ�����
 * ����    ��www.armjishu.com 
**********************************************************************************/
  
/* Includes ------------------------------------------------------------------*/
#include "SZ_STM32F103ZE_LIB.h"



					 
uint32_t STM32DeviceSerialID[3]; /* ȫ�ֱ���IntDeviceSerial��Ŷ������豸ID */



/**-------------------------------------------------------
  * @������ NVIC_GroupConfig
  * @����   ����NVIC�ж����ȼ����麯��.
  *         Ĭ������Ϊ1���ر�ʾ�����ȼ�, 3���ر�ʾ�����ȼ�
  *         �û����Ը�����Ҫ�޸�
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/
void NVIC_GroupConfig(void)
{
    /* ����NVIC�ж����ȼ�����:
     - 1���ر�ʾ�����ȼ�  �����ȼ��Ϸ�ȡֵΪ 0 �� 1 
     - 3���ر�ʾ�����ȼ�  �����ȼ��Ϸ�ȡֵΪ 0..7
     - ��ֵԽ�����ȼ�Խ�ߣ�ȡֵ�����Ϸ���Χʱȡ��bitλ 
    */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /*==================================================================================
      NVIC_PriorityGroup   |  �����ȼ���Χ  |  �����ȼ���Χ  |   ����
      ==================================================================================
     NVIC_PriorityGroup_0  |      0         |      0-15      |   0 ���ر�ʾ�����ȼ�
                           |                |                |   4 ���ر�ʾ�����ȼ� 
     ----------------------------------------------------------------------------------
     NVIC_PriorityGroup_1  |      0-1       |      0-7       |   1 ���ر�ʾ�����ȼ�
                           |                |                |   3 ���ر�ʾ�����ȼ� 
     ----------------------------------------------------------------------------------
     NVIC_PriorityGroup_2  |      0-3       |      0-3       |   2 ���ر�ʾ�����ȼ�
                           |                |                |   2 ���ر�ʾ�����ȼ� 
     ----------------------------------------------------------------------------------
     NVIC_PriorityGroup_3  |      0-7       |      0-1       |   3 ���ر�ʾ�����ȼ�
                           |                |                |   1 ���ر�ʾ�����ȼ� 
     ----------------------------------------------------------------------------------
     NVIC_PriorityGroup_4  |      0-15      |      0         |   4 ���ر�ʾ�����ȼ�
                           |                |                |   0 ���ر�ʾ�����ȼ�   
    ==================================================================================*/
}

/**-------------------------------------------------------
  * @������ SZ_STM32_SysTickInit
  * @����   ��ʼ��ϵͳ��ʱ��SysTick
  *         �û����Ը�����Ҫ�޸�
  * @����   ÿ���жϴ���
  * @����ֵ ��
***------------------------------------------------------*/
void SZ_STM32_SysTickInit(uint32_t HzPreSecond)
{
    if (SysTick_Config(SystemCoreClock / HzPreSecond))
    { 
        /* Capture error */ 
        while (1);
    }
}


/**-------------------------------------------------------
  * @������ TimingDelay_Decrement
  * @����   ϵͳ���Ķ�ʱ�����������õ��Ӻ���
  *         ��ȫ�ֱ���TimingDelay��һ������ʵ����ʱ
  * @����   ��
  * @����ֵ ��
***------------------------------------------------------*/

//

void GetDeviceSerialID(void)
{
    STM32DeviceSerialID[0] = *(__IO uint32_t*)(0x1FFFF7E8);
    STM32DeviceSerialID[1] = *(__IO uint32_t*)(0x1FFFF7EC);
    STM32DeviceSerialID[2] = *(__IO uint32_t*)(0x1FFFF7F0);
}




/**-------------------------------------------------------
  * @������ __SZ_STM32_COMInit
  * @����   ��STM32��USART��ʼ���ײ㺯��
  * @����1  COM1  ��ӦSTM32��USART1 ��Ӧ�������ϴ���1
  *         COM2  ��ӦSTM32��USART2 ��Ӧ�������ϴ���2
  * @����2  ָ��һ����Ա�Ѹ�ֵUSART_InitTypeDef�ṹ���ָ��
  * @����ֵ ��
***------------------------------------------------------*/
void __SZ_STM32_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO clock */
    /* ʹ��STM32��USART��ӦGPIO��Clockʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);


        /* ʹ��STM32��USART��Clockʱ�� */
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE);
    

    /* Configure USART Tx as alternate function push-pull */
    /* ��ʼ��STM32��USART��TX�ܽţ�����Ϊ���ù���������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    /* ��ʼ��STM32��USART��RX�ܽţ�����Ϊ���ù������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART configuration */
    /* ���ݴ���Ĳ�����ʼ��STM32��USART���� */
    USART_Init(USART2, USART_InitStruct);

    /* Enable USART */
    /* ʹ��STM32��USART����ģ�� */
    USART_Cmd(USART2, ENABLE);
}

/**-------------------------------------------------------
  * @������ __SZ_STM32_COMInit
  * @����   �����û���STM32��USART��ʼ������
  * @����1  COM1  ��ӦSTM32��USART1 ��Ӧ�������ϴ���1
  *         COM2  ��ӦSTM32��USART2 ��Ӧ�������ϴ���2
  * @����2  BaudRate ���ڵĲ����ʣ�����"115200"
  * @����ֵ ��
***------------------------------------------------------*/
void uart_init(COM_TypeDef COM, uint32_t BaudRate)
{
  
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = BaudRate;              //���ڵĲ����ʣ�����115200 ��ߴ�4.5Mbits/s
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�����ֳ���(8λ��9λ)
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      //�����õ�ֹͣλ-֧��1��2��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;         //żУ��  
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ��������
    USART_InitStructure.USART_Mode = USART_Mode_Tx; //˫��ģʽ��ʹ�ܷ��ͺͽ���
  
    __SZ_STM32_COMInit(COM, &USART_InitStructure);  // ����STM32��USART��ʼ���ײ㺯��  
    SystemCoreClockUpdate();

}


/*
    �������´���,֧��printf����,����Ҫѡ��use MicroLIB	  
*/
#ifndef MicroLIB
//#pragma import(__use_no_semihosting)             //û��ʵ��fgetcʱ��Ҫ�����ò���   
/* ��׼����Ҫ��֧�ֺ��� ʹ��printf()���Դ�ӡ����Ҫʵ�ָú��� */               
struct __FILE 
{ 
	int handle; 
    /* Whatever you require here. If the only file you are using is */    
    /* standard output using printf() for debugging, no file handling */    
    /* is required. */
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
/* �ض���fputc���� ���ʹ��MicroLIBֻ��Ҫ�ض���fputc�������� */  
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(SZ_STM32_COM2, USART_FLAG_TC) == RESET)
    {}

    /* e.g. write a character to the USART */
    USART_SendData(SZ_STM32_COM2, (uint8_t) ch);

    return ch;
}
/*
����ֱ��ʹ��putchar
����Ҫ�ٶ��� int putchar(int ch)����Ϊstdio.h�������¶���
 #define putchar(c) putc(c, stdout)
*/

int ferror(FILE *f) {  
    /* Your implementation of ferror */  
    return EOF;  
} 
#endif 

FILE __stdin;

int fgetc(FILE *fp)
{
    int ch = 0;
	
    while(USART_GetFlagStatus(SZ_STM32_COM2, USART_FLAG_RXNE) == RESET)
    {
    }

    ch = (int)SZ_STM32_COM2->DR & 0xFF;
	
    putchar(ch); //����
	
	return ch;
}

#ifdef  USE_FULL_ASSERT
// ��Ҫ�ڹ�������Option(��ݼ�ALT+F7)C++����ҳ��define������"USE_FULL_ASSERT"
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
     
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/******************* (C) COPYRIGHT 2013 www.armjishu.com *****END OF FILE****/
