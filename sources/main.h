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
#include "variable.h"
#include "MCF52259.h"
#define EnableInterrupts asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }
void Blink_LED1();

