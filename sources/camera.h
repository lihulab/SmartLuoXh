void Init_OV7620_DMA();
void UART_SendImage();
void Image_display();
void test();
void Dynamic_threshold();
__declspec(interrupt:0) void DMA0_inter(void);
__declspec(interrupt:0) void EPORT3_inter(void);
__declspec(interrupt:0) void EPORT1_inter(void);
#define ROW 60
#define COLUMN 80
#define SDA_H MCF_GPIO_PORTAS|=MCF_GPIO_PORTAS_PORTAS1
#define SDA_L MCF_GPIO_PORTAS&=~MCF_GPIO_PORTAS_PORTAS1
#define SCL_H MCF_GPIO_PORTAS|=MCF_GPIO_PORTAS_PORTAS0
#define SCL_L MCF_GPIO_PORTAS&=~MCF_GPIO_PORTAS_PORTAS0
#define SDA_OUT MCF_GPIO_DDRAS|=MCF_GPIO_DDRAS_DDRAS1
#define SDA_IN MCF_GPIO_DDRAS &= (~MCF_GPIO_DDRAS_DDRAS1)