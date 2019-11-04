#include "sys.h"
#include "delay.h"
#include "uart.h"
#include "led.h"
#include "motor.h"
#include "key.h"
#include "adc.h"
#include "pid.h"

u8 Start_Flag=0; //电机锁定/解锁标志
u16 Pluse_High=10; //脉冲高电平持续 1000->10ms (网上电机例程里，此值为1us)         1->10us
u16 Pluse_Period=100; //脉冲周期  100->1ms (网上电机例程里，此值为1600us)         160->1600us


extern float adc_temp;
extern float angle_speed;
float POS_OUT;
float POS_fdb;
float Speed_OUT;
u8 accflag;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2	
	My_USART1_Init(115200);
	printf("start");
	delay_init(168);
	LED_Init();			
  Motor_Init();
	KEY_Init();    
	
	TIM3_Int_Init(1000-1,8400-1);//100ms  (按键扫描500ms)
 	TIM4_Int_Init(10-1,84-1);	//84M/84=1Mhz(10^6)计数频率，计数10次为10us  (生成脉冲)

	
	while(1)
	{
		//在这里进行pid计算，并改变Pluse_Period
	}

}
