#include    "stm32f10x.h"
#include    "hardware.h"

/*---------------------------------------------------------------------------------------------*/
void hardware_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	GPIO_SetBits(GPIOF, GPIO_Pin_11);   //����485Ϊ���״̬���ݲ��ı���״̬
	USART_Config(USART1, 115200);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	            //USART1  TX
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	            // USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ���ÿ�©����
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*---------------------------------------------------------------------------------------------*/
void NVIC_configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			     	//���ô����ж�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*---------------------------------------------------------------------------------------------*/
static void USART_Config(USART_TypeDef* USARTx, u32 rate)
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = rate;					//����115200bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
	USART_Init(USARTx, &USART_InitStructure);							//���ô��ڲ�������
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);                      //ʹ�ܽ����ж�
	USART_Cmd(USARTx, ENABLE);
}







