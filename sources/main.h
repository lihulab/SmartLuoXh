#include <stdio.h>
#include <string.h>
#include <math.h>
#include "support_common.h"
#include "adc.h"
#include "OLED12864.h"
#include "speed_control.h"
#include "angle_control.h"
#include "FX_UART_driver.h"
#include "PIT.h"
#include "camera.h"
#include "key.h"
#define EnableInterrupts asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }
void Blink_LED1();

struct PID
{
	float Proportion;
	float Integral;
	float Derivative;
	float Out;
	float Error;
	float Error_L;
	float Error_P;
};
struct DIR
{
	float k;
	float b;
	float Qk;
	float QB;
	float Out;
};

extern float Car_speed;
extern struct DIR Dir;
extern struct PID Speed_L_PID;
extern struct PID Speed_R_PID;
extern struct PID Angle_PID;
extern char graph_switch;