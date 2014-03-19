void Init_OV7620_DMA();
__declspec(interrupt:0) void DMA0_inter(void);
__declspec(interrupt:0) void EPORT3_inter(void);
__declspec(interrupt:0) void EPORT1_inter(void);
#define ROW 40
#define COLUMN 80
#define SDA_H MCF_GPIO_PORTAS|=MCF_GPIO_PORTAS_PORTAS1
#define SDA_L MCF_GPIO_PORTAS&=~MCF_GPIO_PORTAS_PORTAS1
#define SCL_H MCF_GPIO_PORTAS|=MCF_GPIO_PORTAS_PORTAS0
#define SCL_L MCF_GPIO_PORTAS&=~MCF_GPIO_PORTAS_PORTAS0
#define SDA_OUT MCF_GPIO_DDRAS|=MCF_GPIO_DDRAS_DDRAS1
#define SDA_IN MCF_GPIO_DDRAS &= (~MCF_GPIO_DDRAS_DDRAS1)
extern unsigned char Image_Data[ROW][COLUMN];
extern unsigned char Image_bw[ROW/8][COLUMN];
extern unsigned char Image_Edge[ROW][2];
extern unsigned char Image_T;//二值化图像的值
extern unsigned char VSYN_Flag,HREF_Flag;
extern unsigned int Point_C,Line_C,Line_ROW;