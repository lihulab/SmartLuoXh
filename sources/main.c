#include "main.h"
float Car_speed = 0;
struct PID Speed_L_PID;
struct PID Speed_R_PID;
struct PID Angle_PID;
int main(void)
{
	Angle_PID.Proportion = 15;
	Angle_PID.Derivative = 0.15;
	Speed_L_PID.Proportion = 1;
	Speed_L_PID.Integral = 0.001;
	Speed_L_PID.Error_P = 0;
	Speed_L_PID.Error_L = 0;
	Speed_R_PID.Proportion = 1;
	Speed_R_PID.Integral = 0.001;
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
		LCD_P6x8int(0, 0, Angle_PID.Proportion);
		LCD_P6x8float(0, 1, Angle_PID.Derivative);
		LCD_P6x8float(0,2,Speed_L_PID.Out);
		LCD_P6x8float(0, 3, Speed_R_PID.Out);
		LCD_P6x8float(0,4,angle);
		if(key2_press())
		{
			Angle_PID.Proportion-=1;
		}
		if(key3_press())
		{
			Angle_PID.Derivative-=0.01;
		}
		if(key4_press())
		{
			Angle_PID.Proportion+=1;
		}
		if(key5_press())
		{
			Angle_PID.Derivative+=0.01;
		}
	}
}