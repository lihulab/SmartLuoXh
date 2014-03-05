#include "main.h"
float Car_speed = 0;
struct PID Speed_PID;
struct PID Angle_PID;
int main(void)
{
	pll_init_128M();
	UART_INIT(0,128000000,115200,0);
	Init_OV7620_DMA();
	EnableInterrupts;
	while(1)
	{
	}
}