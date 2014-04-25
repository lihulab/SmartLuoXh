#include "main.h"

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
	pll_init_128M();
	Init_Variable();
	UART_INIT(0,128000000,19200,0);
	PWM_INIT();
	init_PIT0();
	Init_ADC();
	OLCD_init();
	Coder_init();
	init_key();
	Init_OV7620_DMA();
	Init_LED();
	EnableInterrupts;
	while(1)
	{		
		Dynamic_threshold();
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
			if(temp==7) temp=0;
		}
		if(temp==0)
		{
			LCD_P6x8Str(0, 0, "start");
			if(key5_press())
			{
				Speed_R_PID.Out=0;
				Speed_L_PID.Out=0;
			}
			if(key4_press())
			{
				Set_speed=0;
			}
			if(key3_press())
			{
				Set_speed=Set_speed_temp;
			}
		}
		else if(temp==1)
		{
			LCD_P6x8float(0, 0, Speed_L_PID.Proportion);
			LCD_P6x8float(0, 1, Speed_L_PID.Integral);
			if(key2_press())
			{
				Speed_L_PID.Proportion-=5;
				Speed_R_PID.Proportion-=5;
			}
			if(key3_press())
			{
				Speed_L_PID.Integral-=0.5;
				Speed_R_PID.Integral-=0.5;
			}
			if(key4_press())
			{
				Speed_L_PID.Proportion+=5;
				Speed_R_PID.Proportion+=5;
			}
			if(key5_press())
			{
				Speed_L_PID.Integral+=0.5;
				Speed_R_PID.Integral+=0.5;
			}
			LCD_P6x8Str(0, 6,"Speed");
		}
		else if(temp==2)
		{
			LCD_P6x8float(0, 0, PID_k.Proportion);
			LCD_P6x8float(0, 1, PID_k.Derivative);
			if(key2_press())
			{
				PID_k.Proportion+=0.025;
			}
			if(key3_press())
			{
				PID_k.Derivative+=0.005;
			}
			if(key4_press())
			{
				PID_k.Proportion-=0.025;
			}
			if(key5_press())
			{
				PID_k.Derivative-=0.005;
			
			}
			LCD_P6x8Str(0, 6,"DIR_K");
		}\
		else if(temp==3)
		{
			LCD_P6x8float(0, 0, PID_b.Proportion);
			LCD_P6x8float(0, 1, PID_b.Derivative);
			if(key2_press())
			{
				PID_b.Proportion+=2.5;
			}
			if(key3_press())
			{
				PID_b.Derivative+=0.005;
			}
			if(key4_press())
			{
				PID_b.Proportion-=2.5;
			}
			if(key5_press())
			{
				PID_b.Derivative-=0.005;
			
			}
			LCD_P6x8Str(0, 6,"DIR_B");
		}
		else if(temp==4)
		{
			LCD_P6x8Str(0,0,"Set speed");
			LCD_P6x8float(0, 1, Set_speed_temp);

			if(key2_press())
			{
				Set_speed_temp+=5;
			}
			if(key4_press())
			{
				Set_speed_temp-=5;
			}
		}
			else if(temp==5)
		{
			LCD_P6x8float(0, 0, Speed_Angle_PID.Proportion);
			LCD_P6x8float(0, 1, Speed_Angle_PID.Derivative);
			if(key2_press())
			{
				Speed_Angle_PID.Proportion+=0.5;
			}
			if(key3_press())
			{
				Speed_Angle_PID.Derivative+=0.1;
			}
			if(key4_press())
			{
				Speed_Angle_PID.Proportion-=0.5;
			}
			if(key5_press())
			{
				Speed_Angle_PID.Derivative-=0.1;
			
			}
			LCD_P6x8Str(0, 6,"speed");	
		}
		else if(temp==6)
		{
			//LCD_P6x8float(0, 1, Dir_PID.Error);
			LCD_P6x8Str(0,0,"Set angle");
			if(key2_press())
			{
				set_angle=angle;
			}
		}
		Image_display();
		if(graph_switch==0) test();
		Draw_BMP(40, 0, 120, 7, Image_disp[0]);
		LCD_P6x8int(0, 7, Dir.k);
		LCD_P6x8int(18, 7, Dir.b);
		//UART_SendImage();
		//Blink_LED1();
		UART_Sendgraph(0,0,angle*10);
		UART_Sendgraph(0,1,Car_speed*10);
		UART_Sendgraph(0,2,Angle_PID.Out);
		UART_Sendgraph(0,3,Speed_L_PID.Out);
	}
}