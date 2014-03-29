/*
 * main implementation: use this sample to create your own application
 *
 */
#include "main.h"
unsigned char Image_Data[ROW][COLUMN];//�ɼ�����ͼ�����飬��ʼ��ι���Ͻ�
unsigned char Image_bw[ROW][COLUMN];
unsigned char Image_disp[ROW/8+1][COLUMN];
unsigned char Image_Edge[ROW][2];
unsigned char Image_Middle[ROW];
unsigned char Image_T=100;//��ֵ��ͼ���ֵ
unsigned char VSYN_Flag,HREF_Flag;
unsigned int Point_C=0,Line_C=0,Line_ROW=0;
unsigned char Valid_Line=45;//���ڼ�¼��Ч������
unsigned char Right_miss_flag=0,Left_miss_flag=0;
char Def_middle=COLUMN/2;
/*************************************
/��С���˷�����ڲ�����    
/int x1 x2      ��Ҫ�������С�е������       //���뱣֤x2��x1
*************************************/
void Calc_slope(int x1, int x2)
{
	float line_xi=0,line_yi=0,line_A=0,line_B=0,line_C=0,line_K=0;//�ֱ��ʾx�ۼӺͣ�y�ۼӺͣ��ۼӺ͵ĳ˻����˻����ۼӺͣ�x�ۼӺ͵�ƽ����xƽ�����ۼӺ�
	int i;
	int n=x2-x1+1;
	int temp[60];
	for(i=x1;i<=x2;i++)
		temp[i]=((Image_Edge[i][0]+Image_Edge[i][1])/2-39.5)*75/(75-i)+39.5;//ͼ�����
	for(i=x1;i<=x2;i++) //�ȶ������е�����λ����λ����ͼ�����������������Ϊ��x�ᣬˮƽ����Ϊ��y��
	{
		line_xi+=i;           //x���ۼӺ�
		line_yi+=temp[i];     //y���ۼӺ�
		line_B+=i*temp[i];    //�˻����ۼӺ�
		line_K+=i*i;          //xƽ�����ۼӺ�
	}
	line_A=line_xi*line_yi;           //�ۼӺ͵ĳ˻�
	line_C=line_xi*line_xi;           //x�ۼӺ͵�ƽ��
	Dir.k=(int)((line_A-n*line_B)/(line_C-n*line_K)*100);
	Dir.b=(int)((line_yi-Dir.k/100*line_xi)/n-40);


	//Mid_err=Get_average(x1,x2);
}

//ȡ�м�ֵ
unsigned char middle(uint8 a,uint8 b,uint8 c)
{
  unsigned char max,min;
  max=a;
  min=b;
  if(a<=b)
  {
    min=a;
    max=b;
  }//��������������
  if(c<min)
    return min;
  else if(c<=max)
    return c;
  else
    return max;
} 
void Black_lvbo()
{
  unsigned char i;
  for(i=1;i<ROW-4;i++)
  {
    if(( Image_Edge[i][0]<= Image_Edge[i+1][0])&& ((Image_Edge[i+2][0]< Image_Edge[i+1][0])||(Image_Edge[i+2][0]> Image_Edge[i+3][0]))&& (Image_Edge[i+1][0]<= Image_Edge[i+3][0]))
    {
      Image_Edge[i+2][0]=(Image_Edge[i+3][0]+Image_Edge[i+1][0])/2;
    }
    if(( Image_Edge[i][0]>= Image_Edge[i+1][0])&& ((Image_Edge[i+2][0]> Image_Edge[i+1][0])||(Image_Edge[i+2][0]< Image_Edge[i+3][0]))&& (Image_Edge[i+1][0]>= Image_Edge[i+3][0]))
    {
      Image_Edge[i+2][0]=(Image_Edge[i+3][0]+Image_Edge[i+1][0])/2;
    }
    
    
    if(( Image_Edge[i][1]>= Image_Edge[i+1][1])&& ((Image_Edge[i+2][1]> Image_Edge[i+1][1])||(Image_Edge[i+2][1]< Image_Edge[i+3][1]))&& (Image_Edge[i+1][1]>= Image_Edge[i+3][1]))
    {
      Image_Edge[i+2][1]=(Image_Edge[i+3][1]+Image_Edge[i+1][1])/2;
    }
    if(( Image_Edge[i][1]<= Image_Edge[i+1][1])&& ((Image_Edge[i+2][1]< Image_Edge[i+1][1])||(Image_Edge[i+2][1]> Image_Edge[i+3][1]))&& (Image_Edge[i+1][1]<= Image_Edge[i+3][1]))
    {
      Image_Edge[i+2][1]=(Image_Edge[i+3][1]+Image_Edge[i+1][1])/2;
    }  
  }
}
void Get_valid_line()
{
	uint8 i,j;
	int Mid[ROW];
	  
	for(i=0;i<45;i++)
	{      
	if(((Image_Edge[i][0] - Image_Edge[i+1][0]) > 5)||((Image_Edge[i+1][1] - Image_Edge[i][1]) > 5))
		if(((Image_Edge[i][0] - Image_Edge[i+2][0]) > 5)||((Image_Edge[i+2][1] - Image_Edge[i][1]) > 5))
			if(((Image_Edge[i][0] - Image_Edge[i+3][0]) > 5)||((Image_Edge[i+3][1] - Image_Edge[i][1]) > 5))
	  break;
	}
	j=i;
	  
	for(i=0;i<j;i++)
	{      
		Mid[i] = (Image_Edge[i][0]+Image_Edge[i][1])/2;
	} 
	for(i=1;i<j-1;i++)
	{      
		Mid[i] = middle(Mid[i-1],Mid[i],Mid[i+1]);
	}
	for(i=10;i<j-1;i++)
	{
		if(Mid[i]<10 || Mid[i]>70||fabs(Image_Edge[i][1]-Image_Edge[i][0])<10) break;
	}
	if(i>45) i=45;
	Valid_Line=i;
}
void Get_middle()
{
	char i;
	//Valid_Line = 30;
	for(i=ROW-1;i>=Valid_Line;i--)
	{
		Image_Middle[i]=(Image_Edge[i][0]+Image_Edge[i][1])/2;
	}
	for(i=Valid_Line-1;i>=0;i--) Image_Middle[i]=0;
}
void test(void)
{
	unsigned char i,j;
	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COLUMN;j++)
		{
			Image_disp[i/8][j]&=~(1<<(i%8));
		}
	}
	for(i=ROW-Valid_Line;i<ROW;i++)
	{
		Image_disp[i/8][Image_Edge[ROW-i-1][0]]|=(1<<(i%8));
		Image_disp[i/8][Image_Edge[ROW-i-1][1]]|=(1<<(i%8));
		//Image_disp[i/8][Image_Middle[i]]|=(1<<(i%8));
		//Image_disp[i/8][Def_middle]|=(1<<(i%8));
	}
}
void Dynamic_threshold(void)
{
	unsigned char T_target_error=5,T_error=0,T_last;
	long int S_target=0,S_background=0;//Ŀ��ͱ����ĻҶȺ�
	int N_target=0,N_background=0;//Ŀ��ͱ����ĵ������
	unsigned char E_target=0,E_background=0;
	unsigned int i,j;
	do
	{
		for(i=0;i<ROW;i++)
		{
			for(j=0;j<COLUMN;j+=1)
			{
				if(Image_Data[i][j]<Image_T)
				{
					S_target+=Image_Data[i][j];
					N_target++;
				}
				else 
				{
					S_background+=Image_Data[i][j];
					N_background++;
				}
			}
		}
		if((N_target==0)||(N_background==0)) break;
		E_target=(unsigned char)(S_target/N_target);
		E_background=(unsigned char)(S_background/N_background);
		T_last=Image_T;
		Image_T=fabs((E_target+E_background)/2);
		T_error=fabs(Image_T-T_last);
	}while(T_error>T_target_error);
}
void Image_binaryzation()
{
	unsigned int i,j;
	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COLUMN;j++)
		{
			if(Image_Data[i][j]<Image_T+10) Image_bw[i/8][j]&=~(1<<(i%8)) ;
			else Image_bw[i/8][j]|=(1<<(i%8));
		}
	}
}
void Image_display()
{
	unsigned int i,j;
	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COLUMN;j++)
		{
			if(Image_bw[ROW-1-i][COLUMN-1-j]==1) Image_disp[i/8][j]&=~(1<<(i%8)) ;
			else Image_disp[i/8][j]|=(1<<(i%8));
		}
	}
}
void UART_SendBWImage()
{
	unsigned int i,j;
	UART_Send1byte(0,0x00);
	UART_Send1byte(0,0xff);
	UART_Send1byte(0,0x01);
	UART_Send1byte(0,0x00);
	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COLUMN;j++)
		{
			UART_Send1byte(0,Image_bw[i][j]);
		}
	}
}
void UART_SendImage()
{
	unsigned int i,j;
	UART_Send1byte(0,0x00);
	UART_Send1byte(0,0xff);
	UART_Send1byte(0,0x01);
	UART_Send1byte(0,0x00);
	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COLUMN;j++)
		{
			UART_Send1byte(0,Image_Data[i][j]);
		}
	}
}
/************************************************************/
/*                     ��ʼ��EPORTģ��                      */
/************************************************************/
void EPORT_init(void)
{
	//ʹ��IRQ1��3��5��7��������
	MCF_GPIO_PNQPAR = MCF_GPIO_PNQPAR_IRQ1_IRQ1
				     |MCF_GPIO_PNQPAR_IRQ3_IRQ3;

	//���������ش����ж�		   
	MCF_EPORT_EPPAR = MCF_EPORT_EPPAR_EPPA1_FALLING
				 	 |MCF_EPORT_EPPAR_EPPA3_RISING;
	//�������ݷ���Ϊ����
	MCF_EPORT_EPDDR =0x00;

	//ʹ��1,3,5,7�ж�
	MCF_EPORT_EPIER = MCF_EPORT_EPIER_EPIE1;
					 //|MCF_EPORT_EPIER_EPIE3	

	//���1,3,5,7�жϱ�־λ
	MCF_EPORT_EPFR = MCF_EPORT_EPFR_EPF1
				    |MCF_EPORT_EPFR_EPF3;
				   
	//ʹ���ж�				   
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL
				   &~MCF_INTC_IMRL_INT_MASK1 
				   &~MCF_INTC_IMRL_INT_MASK3;
}
/************************************************************/
/*                    EPORT1�жϷ�����                    */
/************************************************************/
__declspec(interrupt:0) void EPORT1_inter(void)
{
	MCF_EPORT_EPFR = MCF_EPORT_EPFR_EPF1;           //�����־λ
	VSYN_Flag = 1;
	Line_C=0;
	Point_C=0;
	Line_ROW=0;
	MCF_EPORT_EPIER|=MCF_EPORT_EPIER_EPIE3;//�����ж�
}

/************************************************************/
/*                    ���ж��жϷ�����                    */
/************************************************************/
__declspec(interrupt:0) void EPORT3_inter(void)
{
	unsigned char i,j;
	MCF_EPORT_EPFR = MCF_EPORT_EPFR_EPF3;           //�����־λ
	HREF_Flag = 1;
	if((Line_C%4)==1)
	{
		MCF_DMA_BCR(0)=COLUMN;
		MCF_DMA_DAR(0)=(uint32)Image_Data[Line_ROW];
		MCF_DMA_DCR(0)|=MCF_DMA_DCR_EEXT;
	}
	else if((Line_C%4)==2)
	{
		/*��ֵ��*/
		for(j=0;j<COLUMN;j++)
		{
			if(Image_Data[Line_ROW-1][j]>Image_T) Image_bw[Line_ROW-1][j]=1;
			else Image_bw[Line_ROW-1][j]=0;
		}
		/*ǰʮ���˲�*/
		if(Line_ROW<30)
		{
			for(i=1;i<COLUMN-1;i++)
			{
				if((Image_bw[Line_ROW-1][i+1]==1)&&(Image_bw[Line_ROW-1][i-1]==1)) Image_bw[Line_ROW-1][i]=1;
			}
		}
	}
	else if((Line_C%4)==3)
	{
		/*��ȡ����*/
		if((Line_ROW-1)==0) 
		{
			if(((Image_Edge[0][0]+Image_Edge[0][1])/2)==0) Def_middle=COLUMN/2;
			else Def_middle=(Image_Edge[0][0]+Image_Edge[0][1])/2;
		}
		else
		{
			Def_middle = (Image_Edge[Line_ROW-2][0]+Image_Edge[Line_ROW-2][1])/2;
		}
		for(i=Def_middle;i>1;i--)
		{
			if(Image_bw[Line_ROW-1][i]==0)
			{
				break;
			}
			Image_Edge[Line_ROW-1][0]=COLUMN-i;
		}
		for(i=Def_middle;i<COLUMN-2;i++)
		{
			if(Image_bw[Line_ROW-1][i]==0)
			{
				break;
			}
			Image_Edge[Line_ROW-1][1]=COLUMN-i;
		}
	}
	Line_C++;
	if(Line_C==240)
	{
		MCF_EPORT_EPIER&=~MCF_EPORT_EPIER_EPIE3;
	}
}
__declspec(interrupt:0) void DMA0_inter(void)
{
	MCF_DMA0_DSR |= MCF_DMA_DSR_DONE;
	Line_ROW++;
	if(Line_ROW==ROW)
	{
		MCF_EPORT_EPIER&=~MCF_EPORT_EPIER_EPIE3;
		Line_ROW=0;
		Dynamic_threshold();
		Image_display();
		Black_lvbo();
		//Image_binaryzation();
		//Edge_detect();
		Get_valid_line();
		Calc_slope(0,(Valid_Line>45?45:Valid_Line));
		//Get_middle();
		//Calc_error();

		if(graph_switch==0) test();
		//LCD_CLS();
	}
}
void SCCB_Init(void)
{
	MCF_GPIO_PASPAR |= MCF_GPIO_PASPAR_SDA0_GPIO
					  |MCF_GPIO_PASPAR_SCL0_GPIO;
	MCF_GPIO_DDRAS |= MCF_GPIO_DDRAS_DDRAS0;
}
void SCCB_Wait(void)
{
	unsigned char i;
	for(i=0;i<20;i++)
	{
		asm("nop");
	}
}
void SCCB_Start(void)
{
	SDA_OUT;
	SDA_H;
	SCL_H;
	SCCB_Wait();
	SDA_L;
	SCCB_Wait();
	SCL_L;
}
void SCCB_Stop(void)
{
	SDA_OUT;
	SDA_L;
	SCCB_Wait();
	SCL_H;
	SCCB_Wait();
	SDA_H;
	SCCB_Wait();
}
void SCCB_SendAck(unsigned char ack)
{
	SDA_OUT;
	if(ack) SDA_H;
	else if(!ack) SDA_L;
	SCL_H;
	SCCB_Wait();
	SCL_L;
}
unsigned char SCCB_Sendbyte(unsigned char data)
{
	unsigned char i;
	unsigned char ack;
	SDA_OUT;
	for(i=0;i<8;i++)
	{
		if(data&0x80) SDA_H;
		else SDA_L;
		data<<=1;
		SCCB_Wait();
		SCL_H;
		SCCB_Wait();
		SCL_L;
		SCCB_Wait();
	}
	SDA_H;
	SDA_IN;
	SCCB_Wait();
	SCL_H;
	SCCB_Wait();
	ack=MCF_GPIO_SETAS&MCF_GPIO_SETAS_SETAS1;
	SCL_L;
	SCCB_Wait();
	return ack;
}
unsigned char SCCB_Receivebyte(void)
{
	unsigned char i;
	unsigned char data=0;
	SDA_IN;
	for(i=0;i<8;i++)
	{
		SCL_H;
		SCCB_Wait();
		data<<1;
		if(MCF_GPIO_SETAS&MCF_GPIO_SETAS_SETAS1) data|=0x01;
		SCL_L;
		SCCB_Wait();
	}
	return data;
}
void SCCB_Bytewrite(unsigned char device,unsigned char address,unsigned char data)
{
	unsigned char i;
	unsigned char ack;
	for(i=0;i<20;i++)
	{
		SCCB_Start();
		ack=SCCB_Sendbyte(device);
		if(ack)
		{
			SCCB_Stop();
			continue;
		}
		ack=SCCB_Sendbyte(address);
		if(ack)
		{
			SCCB_Stop();
			continue;
		}
		ack=SCCB_Sendbyte(data);
		if(ack)
		{
			SCCB_Stop();
			continue;
		}
		SCCB_Stop();
		if(ack==0) break;
	}
}
void Init_OV7620_DMA()
{
	MCF_GPIO_PTEPAR = 0;
	MCF_GPIO_DDRTE = 0;
	SCCB_Init();
	SCCB_Bytewrite(0x42,0x14,0x24);
	SCCB_Bytewrite(0x42,0x11,0x01);
	MCF_DMA_DSR(0) |= MCF_DMA_DSR_DONE;//��մ�����ɱ�־λ
	MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);
	MCF_SCM_DMAREQC |= MCF_SCM_DMAREQC_DMAC0(0x04);//�趨DMA0ΪDTIM0����
	MCF_DMA_SAR(0)=(uint32)0x40100030;//Դ��ַΪPTE
	MCF_DMA_DAR(0)=(uint32)Image_Data[0];//Ŀ�ĵ�ַΪͼ������
	MCF_DMA_BCR(0)=80;//���䳤��Ϊÿ�еĳ��ȣ�������80������
	MCF_DMA_DCR(0)=MCF_DMA_DCR_INT//����DMA�ж�
				|MCF_DMA_DCR_SSIZE(1)
				|MCF_DMA_DCR_DSIZE(1)//ÿ�δ���Դ��ַ��Ŀ�ĵ�ַ��Ϊһ�ֽ�
				|MCF_DMA_DCR_DINC//ÿ�δ���֮��Դ��ַ���ֲ��䣬Ŀ�ĵ�ַ��1
				|MCF_DMA_DCR_D_REQ//ÿ�δ������֮��ر�DMA�ⲿ����
				//|MCF_DMA_DCR_EEXT
				|MCF_DMA_DCR_CS;//һ��DMA������һ��
	MCF_GPIO_PTCPAR|=MCF_GPIO_PTCPAR_DTIN0_DTIN0;//�趨DTIN0�˿ڵĹ���
	MCF_DTIM0_DTMR=0;//����STMR�Ĵ���
	MCF_DTIM0_DTMR=MCF_DTIM_DTMR_CE_RISE//��׽������
				|MCF_DTIM_DTMR_CLK_DTIN;//ʱ������ΪDTIN
	MCF_DTIM0_DTER|=MCF_DTIM_DTER_CAP;//���ز�׽����
	MCF_DTIM0_DTXMR|=MCF_DTIM_DTXMR_DMAEN;//DMA����ʹ��
	MCF_DTIM_DTMR(0) |= MCF_DTIM_DTMR_RST;//����DTIM
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK9;
	MCF_INTC0_ICR09=MCF_INTC_ICR_IP(5)+MCF_INTC_ICR_IL(6);
	EPORT_init();
}
