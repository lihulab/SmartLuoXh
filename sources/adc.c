#include "main.h"
/************************************************************
��Ƭ���ɼ�AN0��AN1��ADֵ���������Ǻͼ��ٶȼƵ�ֵ
************************************************************/
/************************************************************/
/*                     ��ʼ��ADCģ��                        */
/************************************************************/
void Init_ADC(void)
{
	MCF_GPIO_PANPAR=MCF_GPIO_PANPAR_AN0_AN0   //ʹ��ADC0��1����ͨ�������ڲɼ���
				   |MCF_GPIO_PANPAR_AN1_AN1
				   |MCF_GPIO_PANPAR_AN2_AN2;
				   
	MCF_ADC_CTRL1=MCF_ADC_CTRL1_CHNCFG(0)   //ADCΪ�����ɼ�ģʽ
				 |MCF_ADC_CTRL1_SMODE(2);//ѭ���ɼ�
				 
	MCF_ADC_CTRL2=MCF_ADC_CTRL2_DIV(9); //��Ƶϵ��Ϊ��2*(9+1)=20
										//ADCģ��ʱ��Ϊ4MHz
	MCF_ADC_CAL=0;       //�ߵ͵�ѹ�Ĳο���ѹ�ֱ�ΪVRH��VRL     
		  
	MCF_ADC_ADSDIS=0;    //ʹ�����еĲ���ͨ��
	
	//����8��ͨ���Ĳ���ֵ������ȫ������Ϊ0
	MCF_ADC_ADOFS(0)=0X0000;
	MCF_ADC_ADOFS(1)=0X0000;
	MCF_ADC_ADOFS(2)=0X0000;
	//ʹ��ADC���磬�����ù�����ʱʱ��Ϊ15��ADC����
	MCF_ADC_POWER=MCF_ADC_POWER_PUDELAY(15)
				 |MCF_ADC_POWER_PD2;
	
	MCF_ADC_CTRL1 &= 0xBFFF;    //��CTRL1��STOP0λ��0������λ���䣬����Ϊ����ģʽ
	
	while (MCF_ADC_POWER & MCF_ADC_POWER_PSTS0)  //�ȴ�ADCת����A��ʹ��
	{}
	while (MCF_ADC_POWER & MCF_ADC_POWER_PSTS1)  //�ȴ�ADCת����B��ʹ��
	{}
	MCF_ADC_CTRL1 |= 0x2000;  //��CTRL1��START0λ��1������λ���䣬����ת��		 				 				 		
}

/************************************************************/
/*           ADC�ɼ�������chanelΪ�ɼ���ͨ����              */
/************************************************************/
uint16 AD_Capture(uint8 chanel) 
{
    uint16 result;
    
    while ((MCF_ADC_ADSTAT & (1 << chanel))==0)  //�ȴ�����׼����
    { }
    result = (MCF_ADC_ADRSLT(chanel)) >> 3;  //ȡ�ý��
    return result;
}