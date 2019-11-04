#include "adc.h"
#include "delay.h"

void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_DeInit();//复位ADC1
	
	ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Mode=ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
//	ADC_InitStructure.ADC_ExternalTrigConv=    //不使用外部触发
	ADC_InitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;     //不使用外部触发
	ADC_InitStructure.ADC_NbrOfConversion=1;
	ADC_InitStructure.ADC_Resolution=ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_Cmd(ADC1,ENABLE);
}

u16 Get_Adc(u8 ch)//ch指通道号
{
	ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==RESET);//EOC是规则转换结束标志位，它为RESET说明转换没结束，这里是在等待转换结束
	
	return ADC_GetConversionValue(ADC1);	//转换结束后返回ADC的值
}
	

u16 Get_Adc_Average(u8 ch,u8 times)//ch是通道号，times是次数，此函数调用获取ADC值times次并返回ADC平均值
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}









