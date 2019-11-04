#include "sys.h"
#include "delay.h"
#include "uart.h"
#include "led.h"
#include "motor.h"
#include "key.h"
#include "adc.h"
#include "pid.h"

u8 Start_Flag=0; //�������/������־
u16 Pluse_High=10; //����ߵ�ƽ���� 1000->10ms (���ϵ���������ֵΪ1us)         1->10us
u16 Pluse_Period=100; //��������  100->1ms (���ϵ���������ֵΪ1600us)         160->1600us


extern float adc_temp;
extern float angle_speed;
float POS_OUT;
float POS_fdb;
float Speed_OUT;
u8 accflag;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2	
	My_USART1_Init(115200);
	printf("start");
	delay_init(168);
	LED_Init();			
  Motor_Init();
	KEY_Init();    
	
	TIM3_Int_Init(1000-1,8400-1);//100ms  (����ɨ��500ms)
 	TIM4_Int_Init(10-1,84-1);	//84M/84=1Mhz(10^6)����Ƶ�ʣ�����10��Ϊ10us  (��������)

	
	while(1)
	{
		//���������pid���㣬���ı�Pluse_Period
	}

}
