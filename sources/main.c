#include "main.h"
float Car_speed = 0;
struct PID Speed_L_PID;
struct PID Speed_R_PID;
struct PID Angle_PID;
struct DIR Dir;
char temp=0,graph_switch=0;//用于参数切换
void Init_LED()
{
	MCF_GPIO_PUBPAR = MCF_GPIO_PUBPAR_UTXD1_GPIO      //设置PUB0~PUB3为通用IO。
			     |MCF_GPIO_PUBPAR_URXD1_GPIO
			     |MCF_GPIO_PUBPAR_URTS1_GPIO
			     |MCF_GPIO_PUBPAR_UCTS1_GPIO;
	MCF_GPIO_DDRUB = MCF_GPIO_DDRUB_DDRUB0           //设置PUB0~PUB3为输出接口。
				    |MCF_GPIO_DDRUB_DDRUB1
				    |MCF_GPIO_DDRUB_DDRUB2
				    |MCF_GPIO_DDRUB_DDRUB3;
}
void Blink_LED1()
{
	if(MCF_GPIO_PORTUB&0x01) MCF_GPIO_PORTUB &= ~0x01;
	else MCF_GPIO_PORTUB|=0x01;
}
int main(void)
{
	Angle_PID.Proportion = 16;
	Angle_PID.Derivative = 0.35;
	Speed_L_PID.Proportion = 3;
	Speed_L_PID.Derivative = -1;
	Speed_L_PID.Integral = 0.00;
	Speed_L_PID.Error_P = 0;
	Speed_L_PID.Error_L = 0;
	Speed_R_PID.Proportion = 3;
	Speed_R_PID.Derivative = -1;
	Speed_R_PID.Integral = 0.00;
	Speed_R_PID.Error_P = 0;
	Speed_R_PID.Error_L = 0;
	Dir.Qk=0.22;
	Dir.QB=0.8;
	pll_init_128M();
	UART_INIT(0,128000000,19200,0);
	PWM_INIT();
	init_PIT0();
	init_ADC();
	OLCD_init();
	Coder_init();
	init_key();
	Init_OV7620_DMA();
	Init_LED();
	EnableInterrupts;
	while(1)
	{		
		LCD_P6x8float(0, 2, Left_motor_speed);
		LCD_P6x8float(0, 3, Right_motor_speed);
		LCD_P6x8float(0, 4, angle);
		LCD_P6x8float(0, 5, Angle_PID.Out);
		//LCD_P6x8int(40, 6, Valid_Line);
		if(key1_press())
		{
			if(key5_press())
			{
				if(graph_switch==0) graph_switch=1;
				else graph_switch=0;
			}
			temp++;
			if(temp==5) temp=0;
		}
		if(temp==0)
		{
			LCD_P6x8float(0, 0, Speed_L_PID.Proportion);
			LCD_P6x8float(0, 1, Speed_L_PID.Derivative);
			if(key2_press())
			{
				Speed_L_PID.Proportion-=0.1;
				Speed_R_PID.Proportion-=0.1;
			}
			if(key3_press())
			{
				Speed_L_PID.Derivative-=0.05;
				Speed_R_PID.Derivative-=0.05;
			}
			if(key4_press())
			{
				Speed_L_PID.Proportion+=0.1;
				Speed_R_PID.Proportion+=0.1;
			}
			if(key5_press())
			{
				Speed_L_PID.Derivative+=0.05;
				Speed_R_PID.Derivative+=0.05;
			}
			LCD_P6x8Str(0, 6,"Speed");
		}
		if(temp==1)
		{
			LCD_P6x8float(0, 0, Dir.Qk);
			LCD_P6x8float(0, 1, Dir.QB);
			if(key2_press())
			{
				Dir.Qk+=0.01;
			}
			if(key3_press())
			{
				Dir.QB+=0.05;
			}
			if(key4_press())
			{
				Dir.Qk-=0.01;
			}
			if(key5_press())
			{
				Dir.QB-=0.05;
			
			}
			LCD_P6x8Str(0, 6,"Direction");
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
				Angle_PID.Proportion-=0.5;
			}
			if(key5_press())
			{
				Angle_PID.Derivative-=0.01;
			
			}
			LCD_P6x8Str(0, 6,"Angle");	
		}
		if(temp==4)
		{
			//LCD_P6x8float(0, 1, Dir_PID.Error);
			LCD_P6x8Str(0,0,"Set angle");
			if(key2_press())
			{
				set_angle=angle;
			}
		}
		Draw_BMP(40, 0, 120, 8, Image_disp[0]);
		//LCD_P6x8int(0, 7, Image_Edge[0][0]);
		//LCD_P6x8int(10, 7, Image_Edge[0][1]);
		//UART_SendImage();
		//Blink_LED1();
		UART_Sendgraph(0,0,Dir.Out);
		//UART_Sendgraph(0,1,Dir.b);
		UART_Sendgraph(0,2,Set_left_speed);
		UART_Sendgraph(0,3,Set_right_speed);
	}
}