#include "motor.h"
#include "key.h"
#include "led.h"
#include "pid.h"
#include "adc.h"
#include "uart.h"	

extern u16 Pluse_High;
extern u16 Pluse_Period;
extern u8 Start_Flag;


//ͨ�ö�ʱ��4�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��4!
void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///ʹ��TIM4ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //����ʱ�ӷָ�
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//��ʼ��TIM4
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //����ʱ��4�����ж�

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //��ʱ��4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4,ENABLE); //ʹ�ܶ�ʱ��4
}

void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //����ʱ�ӷָ�
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�

	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
}



//��ʱ���ж�3  ����ɨ��////////////////////////////////////////////////
u8 key;
u16 Key_Count=0;
u8 acc_flag=0;
float ACC=2;
void TIM3_IRQHandler(void)//100ms
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{		
		Key_Count++;
		if(Key_Count>2)//200ms
		{
			Key_Count=0;
			key=KEY_Scan(0);
			if(key)
			{
				switch(key)
				{				 
					case KEY0_PRES:	//�������
						LED1=0,Start_Flag=1;
						break;
					case KEY1_PRES:	//�������
						LED1=1,Start_Flag=0;
						break;
					case WKUP_PRES://	
//						acc_flag=1;
						dirction=1;
						break;
					case KEY2_PRES://����
						dirction=0;
//						acc_flag=0;
						break;
				}
			}
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}

float temp_x=1;
void acc(float a)
{
	temp_x+=a;
	if(temp_x>0)
		dirction=1,Pluse_Period=10000/temp_x;
	else
		dirction=0,Pluse_Period=-10000/temp_x;
	
	if(Pluse_Period<15)
		Pluse_Period=15;
}

//��ʱ��4�жϷ����� �������////////////////////////////////////////////
u32 TimeCount=0;
int PluseCount=0;//���������800��һȦ
u32 AccCount=0;

void TIM4_IRQHandler(void)//10us
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //����ж�
	{		
		if(Start_Flag==0)
			motor_enable=1;
		else if(Start_Flag==1)
		{
			motor_enable=0;
			TimeCount++;
			if(acc_flag==1)
				AccCount++;
			
			if(TimeCount<Pluse_High)//����ߵ�ƽ����
				Pluse=1;
			else if(TimeCount>Pluse_High)
				Pluse=0;
			if(TimeCount>Pluse_Period)//����
			{
				TimeCount=0;
				
				if(dirction==1)
					PluseCount++;
				else
					PluseCount--;
			}
		}
		
		if(AccCount>1000)
			AccCount=0,acc(ACC);

	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //����жϱ�־λ
}
void Motor_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOEʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6; //DRIVER_DIR DRIVER_OE��Ӧ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE5,6
	
	dirction=1;//PE5����� ˳ʱ�뷽��  DRIVER_DIR
	motor_enable=0;//PE6����� ʹ�����  DRIVER_OE
//---------------------------------------------------------------------------	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //STEP_PULSE ��Ӧ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//�ٶ�100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��PB8
}




