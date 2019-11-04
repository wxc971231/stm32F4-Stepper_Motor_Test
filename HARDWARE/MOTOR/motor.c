#include "motor.h"
#include "key.h"
#include "led.h"
#include "pid.h"
#include "adc.h"
#include "uart.h"	

extern u16 Pluse_High;
extern u16 Pluse_Period;
extern u8 Start_Flag;


//通用定时器4中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器4!
void TIM4_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  ///使能TIM4时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //设置时钟分割
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);//初始化TIM4
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //允许定时器4更新中断

	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4,ENABLE); //使能定时器4
}

void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //设置时钟分割
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断

	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
}



//定时器中断3  按键扫描////////////////////////////////////////////////
u8 key;
u16 Key_Count=0;
u8 acc_flag=0;
float ACC=2;
void TIM3_IRQHandler(void)//100ms
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
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
					case KEY0_PRES:	//电机解锁
						LED1=0,Start_Flag=1;
						break;
					case KEY1_PRES:	//电机锁定
						LED1=1,Start_Flag=0;
						break;
					case WKUP_PRES://	
//						acc_flag=1;
						dirction=1;
						break;
					case KEY2_PRES://减速
						dirction=0;
//						acc_flag=0;
						break;
				}
			}
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
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

//定时器4中断服务函数 脉冲输出////////////////////////////////////////////
u32 TimeCount=0;
int PluseCount=0;//脉冲计数，800个一圈
u32 AccCount=0;

void TIM4_IRQHandler(void)//10us
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
	{		
		if(Start_Flag==0)
			motor_enable=1;
		else if(Start_Flag==1)
		{
			motor_enable=0;
			TimeCount++;
			if(acc_flag==1)
				AccCount++;
			
			if(TimeCount<Pluse_High)//脉冲高电平持续
				Pluse=1;
			else if(TimeCount>Pluse_High)
				Pluse=0;
			if(TimeCount>Pluse_Period)//周期
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
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志位
}
void Motor_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6; //DRIVER_DIR DRIVER_OE对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE5,6
	
	dirction=1;//PE5输出高 顺时针方向  DRIVER_DIR
	motor_enable=0;//PE6输出低 使能输出  DRIVER_OE
//---------------------------------------------------------------------------	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //STEP_PULSE 对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//速度100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB8
}




