#include "main.h"
float Car_speed = 0;
struct PID Speed_L_PID;
struct PID Speed_R_PID;
struct PID Angle_PID;
char temp=0;//用于参数切换
int main(void)
{
	Angle_PID.Proportion = 15;
	Angle_PID.Derivative = 0.15;
	Speed_L_PID.Proportion = 0.1;
	Speed_L_PID.Integral = 0.000;
	Speed_L_PID.Error_P = 0;
	Speed_L_PID.Error_L = 0;
	Speed_R_PID.Proportion = 0.1;
	Speed_R_PID.Integral = 0.000;
	Speed_R_PID.Error_P = 0;
	Speed_R_PID.Error_L = 0;
	pll_init_128M();
	UART_INIT(0,128000000,19200,0);
	PWM_INIT();
	init_PIT0();
	init_ADC();
	OLCD_init();
	Coder_init();
	init_key();
	//get_gyro_zero();
	//get_gravity_zero();
	EnableInterrupts;
	while(1)
	{		
		LCD_P6x8float(0, 2, Speed_L_PID.Out);
		LCD_P6x8float(0, 3, Speed_R_PID.Out);
		LCD_P6x8float(0, 4, angle);
		LCD_P6x8float(0, 5, Angle_PID.Out);
		if(key1_press())
		{
			temp++;
			if(temp==3) temp=0;
		}
		if(temp==0)
		{
			LCD_P6x8float(0, 0, Speed_L_PID.Proportion);
			LCD_P6x8float(0, 1, Speed_L_PID.Integral);
			if(key2_press())
			{
				Speed_L_PID.Proportion-=0.05;
				Speed_R_PID.Proportion-=0.05;
			}
			if(key3_press())
			{
				Speed_L_PID.Integral-=0.005;
				Speed_R_PID.Integral-=0.005;
			}
			if(key4_press())
			{
				Speed_L_PID.Proportion+=0.05;
				Speed_R_PID.Proportion+=0.05;
			}
			if(key5_press())
			{
				Speed_L_PID.Integral+=0.005;
				Speed_R_PID.Integral+=0.005;
			}
			LCD_P6x8Str(0, 6,"Speed");
		}
		if(temp==1)
		{
			LCD_P6x8float(0, 0, Angle_PID.Proportion);
			LCD_P6x8float(0, 1, Angle_PID.Derivative);
			if(key2_press())
			{
				Angle_PID.Proportion+=0.5;
			}
			if(key3_press())
			{
				Angle_PID.Derivative+=0.01;
			}
			if(key4_press())
			{
				Angle_PID.Derivative-=0.01;
			}
			if(key5_press())
			{
				Angle_PID.Proportion-=0.5;
			}
			LCD_P6x8Str(0, 6,"Angle");
		}
		if(temp==2)
		{
			LCD_P6x8Str(0,0,"Set speed");
			LCD_P6x8float(0, 1, Set_speed);
			if(key2_press())
			{
				Set_speed+=1;
			}
			if(key4_press())
			{
				Set_speed-=1;
			}
		}
		UART_Sendgraph(0, 0,gravity*10);
		UART_Sendgraph(0, 1, angle*10);
	}
}