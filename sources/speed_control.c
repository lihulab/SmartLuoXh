#include "main.h"

char MOTOR_DEAD = 0;//电机的死区占空比
float Right_motor_speed=0,Left_motor_speed=0;//左右轮测的的速度
float Set_right_speed=0,Set_left_speed=0;
float Set_speed=0;
uint32 *scount1, *scount2;		//DMA测速指定的无效传输地址
/************************************************************/
/* 				初始化编码器							*/
/************************************************************/
void Coder_init()
{
/*GPT测速模块*/
	MCF_GPIO_PTAPAR|=MCF_GPIO_PTAPAR_ICOC3_ICOC3;
	MCF_GPT_GPTPACTL=MCF_GPT_GPTPACTL_PAE		//使能脉冲累加功能
					&~MCF_GPT_GPTPACTL_PAMOD	//累加脉冲时间模式
					|MCF_GPT_GPTPACTL_PEDGE//累加上升沿
					|MCF_GPT_GPTPACTL_CLK(0);    //采用GPT时时钟
	MCF_GPT_GPTIE=0x00;//禁止GPT中断
	
/*DMA测速模块*/
	MCF_DMA_DSR(1) |= MCF_DMA_DSR_DONE;					//清空传输完成标志位
	MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);
	MCF_SCM_DMAREQC = MCF_SCM_DMAREQC_DMAC1(0x05);		//设定DMA1为DTIM1触发
	MCF_DMA_SAR(1)=(uint32)scount1;			//源地址为PAN
	MCF_DMA_DAR(1)=(uint32)scount2;				//目的地址为图像数组
	MCF_DMA_BCR(1)=1000;									//传输长度为每行的长度，这里是320个像素
	MCF_DMA_DCR(1)=//MCF_DMA_DCR_SSIZE_BYTE
				  //|MCF_DMA_DCR_DSIZE_BYTE		//每次传输源地址和目的地址都为一字节
				  //|MCF_DMA_DCR_DINC						//每次传输之后源地址保持不变，目的地址加1
				  MCF_DMA_DCR_D_REQ						//每次传输结束之后关闭DMA外部请求
				  |MCF_DMA_DCR_EEXT
				  |MCF_DMA_DCR_CS;						//一次DMA请求传输一次
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN1_DTIN1;		//设定DTIN0端口的功能
	MCF_DTIM_DTMR(1) = 0;								//重置STMR寄存器
	MCF_DTIM_DTMR(1) = MCF_DTIM_DTMR_CE_RISE			//捕捉上升沿
				      |MCF_DTIM_DTMR_CLK_DTIN;				//时钟输入为DTIN
	MCF_DTIM_DTER(1) |= MCF_DTIM_DTER_CAP;				//边沿捕捉触发
	MCF_DTIM_DTXMR(1)|= MCF_DTIM_DTXMR_DMAEN;			//DMA请求使能
	MCF_DTIM_DTMR(1) |= MCF_DTIM_DTMR_RST;				//开启DTIM	

}
/************************************************************/
/* 				读取速度值									*/
/************************************************************/
void get_speed()
{
	Left_motor_speed = 1000 - MCF_DMA1_BCR;
	MCF_DMA1_BCR = 1000;
	Right_motor_speed=MCF_GPT_GPTPACNT/0.005;//要通过测量算出一个脉冲代表的距离
	MCF_GPT_GPTPACNT=0;
	Car_speed=(Right_motor_speed+Left_motor_speed)/2;//两轮的速度平均值作为车向前的速度，该速度作为车速度闭环的输入量
}
/************************************************************/
/*                       初始化PWM模块                      */
/************************************************************/
void PWM_INIT(void)
{
    //使能PWM引脚功能	
	MCF_GPIO_PTCPAR=MCF_GPIO_PTCPAR_DTIN1_PWM2
				   |MCF_GPIO_PTCPAR_DTIN2_PWM4;
	
	MCF_GPIO_PTAPAR=MCF_GPIO_PTAPAR_ICOC0_PWM1
				   |MCF_GPIO_PTAPAR_ICOC1_PWM3;
				   
	//设置PWM极性，高有效
	MCF_PWM_PWMPOL=MCF_PWM_PWMPOL_PPOL1
				  |MCF_PWM_PWMPOL_PPOL2
				  |MCF_PWM_PWMPOL_PPOL3
				  |MCF_PWM_PWMPOL_PPOL4;

	//设置A时钟频率和B时钟频率，分频系数为4，总线频率经过分频后为10MHz
	MCF_PWM_PWMPRCLK=MCF_PWM_PWMPRCLK_PCKA(2)
					|MCF_PWM_PWMPRCLK_PCKB(2);

	//设置A时钟频率和B时钟频率， SA=Clock A/2*PWMSCLA; SB=Clock B/2*PWMSCLB
	MCF_PWM_PWMSCLA=5;    //SA时钟频率为1MHz
	MCF_PWM_PWMSCLB=5;    //SB时钟频率为1MHz


	//选择PWM时钟，选择SA或SB时钟
	MCF_PWM_PWMCLK=MCF_PWM_PWMCLK_PCLK1
				  |MCF_PWM_PWMCLK_PCLK2
				  |MCF_PWM_PWMCLK_PCLK3
				  |MCF_PWM_PWMCLK_PCLK4;

	//设置PWM对齐方式，设置为边缘对齐。
	MCF_PWM_PWMCAE=0;


	//复位PWM计数器
	MCF_PWM_PWMCNT1=0;
	MCF_PWM_PWMCNT2=0;
	MCF_PWM_PWMCNT3=0;
	MCF_PWM_PWMCNT4=0;
	
	//设置PWM周期
	MCF_PWM_PWMPER(1)=100;	//10KHz，先把分辨率设为100，最高设定为256
	MCF_PWM_PWMPER(2)=100;	//10KHz
	MCF_PWM_PWMPER(3)=100;	//10KHz
	MCF_PWM_PWMPER(4)=100;
	
	//设置PWM的占空比
	MCF_PWM_PWMDTY(1)=0;
	MCF_PWM_PWMDTY(2)=0;
	MCF_PWM_PWMDTY(3)=0;
	MCF_PWM_PWMDTY(4)=0;
	
	//使能PWM输出
	MCF_PWM_PWME=MCF_PWM_PWME_PWME1
				|MCF_PWM_PWME_PWME2
				|MCF_PWM_PWME_PWME3
				|MCF_PWM_PWME_PWME4;
}
void set_motor_highduty(char highdutyA,char highdutyB)
{
	if((highdutyA<0)&&(highdutyA>-100))
	{
		MCF_PWM_PWMPOL&=~MCF_PWM_PWMPOL_PPOL1;
		MCF_PWM_PWMPOL|=MCF_PWM_PWMPOL_PPOL2;
		MCF_PWM_PWMDTY(1)=-(highdutyA+MOTOR_DEAD);
		MCF_PWM_PWMDTY(2)=-(highdutyA+MOTOR_DEAD);
	}
	if((highdutyA>=0)&&(highdutyA<100))
	{
		MCF_PWM_PWMPOL|=MCF_PWM_PWMPOL_PPOL1;
		MCF_PWM_PWMPOL&=~MCF_PWM_PWMPOL_PPOL2;
		MCF_PWM_PWMDTY(1)=highdutyA+MOTOR_DEAD;
		MCF_PWM_PWMDTY(2)=highdutyA+MOTOR_DEAD;	
	}
	if((highdutyB<0)&&(highdutyB>-100))
	{
		MCF_PWM_PWMPOL&=~MCF_PWM_PWMPOL_PPOL3;
		MCF_PWM_PWMPOL|=MCF_PWM_PWMPOL_PPOL4;
		MCF_PWM_PWMDTY(3)=-(highdutyB+MOTOR_DEAD);
		MCF_PWM_PWMDTY(4)=-(highdutyB+MOTOR_DEAD);	
	}
	if((highdutyB>=0)&&(highdutyB<100))
	{
		MCF_PWM_PWMPOL|=MCF_PWM_PWMPOL_PPOL3;
		MCF_PWM_PWMPOL&=~MCF_PWM_PWMPOL_PPOL4;
		MCF_PWM_PWMDTY(3)=highdutyB+MOTOR_DEAD;
		MCF_PWM_PWMDTY(4)=highdutyB+MOTOR_DEAD;	
	}
}
//-----------------------------------------------
//计算根据速度的pid输出值
//-----------------------------------------------
void speed_out(float target_speed)
{
	Speed_PID.Error+= (Car_speed- target_speed);
	Speed_PID.Error_sum+=Speed_PID.Error;
	Speed_PID.Out=Speed_PID.Proportion*Speed_PID.Error+ Speed_PID.Integral*Speed_PID.Error_sum;
	set_motor_highduty((char)(Speed_PID.Out+ Angle_PID.Out),(char)(Speed_PID.Out+ Angle_PID.Out));//只加入了速度的控制，转向的控制还没有加入进去，暂时准备写个函数
}