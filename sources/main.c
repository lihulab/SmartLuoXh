#include "main.h"
float Car_speed = 0;
struct PID Speed_L_PID;
struct PID Speed_R_PID;
struct PID Angle_PID;
struct PID Dir_PID;
char temp=0,graph_switch=0;//用于参数切换
int main(void)
{
	Angle_PID.Proportion = 27;
	Angle_PID.Derivative = 0.2;
	Speed_L_PID.Proportion = 3;
	Speed_L_PID.Derivative = 0;
	Speed_L_PID.Integral = 0.000;
	Speed_L_PID.Error_P = 0;
	Speed_L_PID.Error_L = 0;
	Speed_R_PID.Proportion = 3;
	Speed_R_PID.Derivative = 0;
	Speed_R_PID.Integral = 0.000;
	Speed_R_PID.Error_P = 0;
	Speed_R_PID.Error_L = 0;
	Dir_PID.Error = 0;
	Dir_PID.Proportion = 2;
	pll_init_128M();
	UART_INIT(0,128000000,57600,0);
	PWM_INIT();
	init_PIT0();
	init_ADC();
	OLCD_init();
	Coder_init();
	init_key();
	Init_OV7620_DMA();
	EnableInterrupts;
	while(1)
	{		
		//LCD_P6x8float(0, 2, Left_motor_speed);
		//LCD_P6x8float(0, 3, Right_motor_speed);
		//LCD_P6x8float(0, 4, angle);
		//LCD_P6x8float(0, 5, Angle_PID.Out);
		//LCD_P6x8int(40, 6, Valid_Line);
		if(key1_press())
		{
			if(key5_press())
			{
				if(graph_switch==0) graph_switch=1;
				else graph_switch=0;
			}
			temp++;
			if(temp==4) temp=0;
		}
		if(temp==0)
		{
			LCD_P6x8float(0, 0, Speed_L_PID.Proportion);
			LCD_P6x8float(0, 1, Speed_L_PID.Integral);
			if(key2_press())
			{
				Speed_L_PID.Proportion-=0.5;
				Speed_R_PID.Proportion-=0.5;
			}
			if(key3_press())
			{
				Speed_L_PID.Integral-=0.01;
				Speed_R_PID.Integral-=0.01;
			}
			if(key4_press())
			{
				Speed_L_PID.Proportion+=0.5;
				Speed_R_PID.Proportion+=0.5;
			}
			if(key5_press())
			{
				Speed_L_PID.Integral+=0.01;
				Speed_R_PID.Integral+=0.01;
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
		if(temp==3)
		{
			LCD_P6x8float(0, 1, Dir_PID.Error);
		}
		Draw_BMP(40, 0, 120, 4, Image_bw[0]); 
		//UART_SendImage();
	}
}