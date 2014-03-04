#include "MCF52259.h"

#define EnableInterrupts asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }

void UART_INIT(uint8 uartNo, int32 sysclk, int32 baud, uint8 intStatus);
void UART_Send1byte(uint8 uartNo, uint8 data);
uint8 UART_Receive1byte(uint8 uartNo);
extern char AT_cmd[10];
void UART_SendString(uint8 uartNo, char *p);
__declspec(interrupt:0) void UART0_inter(void);
__declspec(interrupt:0) void UART1_inter(void);
__declspec(interrupt:0) void UART2_inter(void);
void UART_Sendgraph(uint8 uartNO,uint8 channel,int data);
void UART_Sendint(uint8 uartNO,int data);