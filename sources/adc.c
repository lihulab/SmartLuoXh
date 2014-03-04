#include "main.h"
/*---------------------------------------------------------*/
/************************************************************
单片机采集AN0和AN1的AD值，即陀螺仪和加速度计的值
************************************************************/
/*---------------------------------------------------------*/
void init_ADC(void);
uint16 AD_capture(uint8 chanel);

/************************************************************/
/*                     初始化ADC模块                        */
/************************************************************/
void init_ADC(void)
{
	MCF_GPIO_PANPAR=MCF_GPIO_PANPAR_AN0_AN0   //使能ADC0和1两个通道，用于采集。
				   |MCF_GPIO_PANPAR_AN1_AN1
				   |MCF_GPIO_PANPAR_AN2_AN2;
				   
	MCF_ADC_CTRL1=MCF_ADC_CTRL1_CHNCFG(0)   //ADC为正常采集模式
				 |MCF_ADC_CTRL1_SMODE(2);//循环采集
				 
	MCF_ADC_CTRL2=MCF_ADC_CTRL2_DIV(9); //分频系数为：2*(9+1)=20
										//ADC模块时钟为4MHz
	MCF_ADC_CAL=0;       //高低电压的参考电压分别为VRH和VRL     
		  
	MCF_ADC_ADSDIS=0;    //使能所有的采样通道
	
	//设置8个通道的补偿值，这里全部设置为0
	MCF_ADC_ADOFS(0)=0X0000;
	MCF_ADC_ADOFS(1)=0X0000;
	MCF_ADC_ADOFS(2)=0X0000;
	//使能ADC供电，并设置供电延时时间为15个ADC周期
	MCF_ADC_POWER=MCF_ADC_POWER_PUDELAY(15)
				 |MCF_ADC_POWER_PD2;
	
	MCF_ADC_CTRL1 &= 0xBFFF;    //将CTRL1的STOP0位清0，其它位不变，设置为正常模式
	
	while (MCF_ADC_POWER & MCF_ADC_POWER_PSTS0)  //等待ADC转换器A被使能
	{}
	while (MCF_ADC_POWER & MCF_ADC_POWER_PSTS1)  //等待ADC转换器B被使能
	{}
	MCF_ADC_CTRL1 |= 0x2000;  //将CTRL1的START0位置1，其它位不变，启动转换		 				 				 		
}

/************************************************************/
/*           ADC采集函数，chanel为采集的通道号              */
/************************************************************/
uint16 AD_capture(uint8 chanel) 
{
    uint16 result;
    
    while ((MCF_ADC_ADSTAT & (1 << chanel))==0)  //等待数据准备好
    { }
    result = (MCF_ADC_ADRSLT(chanel)) >> 3;  //取得结果
    return result;
}