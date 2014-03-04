#include "main.h"
float Car_speed = 0;
struct PID Speed_PID;
struct PID Angle_PID;
int main(void)
{
	pll_init_128M();
	init_PIT0();
	init_ADC();
	UART_INIT(0,128000000,256000,1);
	Coder_init();
	EnableInterrupts;
	while(1)
	{
		UART_Sendint(0,(int)Car_speed);
	}
}