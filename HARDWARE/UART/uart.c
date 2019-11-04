#include "uart.h" 

#if 1
#pragma import(__use_no_semihosting)                            
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
  
void _sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//????,??????   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
  
 
 
//USART1->PA9,PA10
 
void My_USART1_Init(u32 BaudRate)
{
	GPIO_InitTypeDef GPIO_Initstructure;//定义初始化IO,串口和中断优先级所用的结构体
	USART_InitTypeDef USART_Initstructure;
//	NVIC_InitTypeDef NVIC_Initstrcuture;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE );//使能串口和IO时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE );
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//PA9,PA10复用为串口
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
  GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;//IO初始化
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_10;//IO初始化
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	USART_Initstructure.USART_BaudRate = BaudRate;//串口参数初始化
	USART_Initstructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Initstructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Initstructure.USART_Parity = USART_Parity_No;
	USART_Initstructure.USART_StopBits = USART_StopBits_1;
	USART_Initstructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_Initstructure);
	
	USART_Cmd(USART1,ENABLE);//串口使能
/*	
	USART_ITConfig (USART1,USART_IT_RXNE ,ENABLE);//中断使能
	
	NVIC_Initstrcuture.NVIC_IRQChannel = USART1_IRQn;//中断参数初始化
  NVIC_Initstrcuture.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_Initstrcuture.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Initstrcuture.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_Initstrcuture);
*/	
}
/*
void USART1_IRQHandler(void)//中断服务函数
{
	u8 res;
	if(USART_GetITStatus(USART1,USART_IT_RXNE))//如果串口非空标志位不为0，即串口非空（收到数据）
	{
		res = USART_ReceiveData(USART1);//把收到的数据交给res,再把res通过串口1发送（给电脑）
		USART_SendData(USART1,res );
	}
}
*/
