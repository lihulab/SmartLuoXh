#include "main.h"
char MOTOR_DEAD = 0;//电机的死区占空比
float Right_motor_speed=0,Left_motor_speed=0;//左右轮测的的速度
float Set_right_speed=0,Set_left_speed=0;
float Set_speed=0;
/************************************************************/
/* 				初始化编码器							*/
/************************************************************/
void Coder_init()
{
	MCF_GPIO_PTAPAR|=MCF_GPIO_PTAPAR_ICOC3_ICOC3;
	MCF_GPT_GPTPACTL=MCF_GPT_GPTPACTL_PAE		//使能脉冲累加功能
					&~MCF_GPT_GPTPACTL_PAMOD	//累加脉冲时间模式
					|MCF_GPT_GPTPACTL_PEDGE//累加上升沿
					|MCF_GPT_GPTPACTL_CLK(0);    //采用GPT时时钟
	MCF_GPT_GPTIE=0x00;//禁止GPT中断
}
/************************************************************/
/* 				初始化编码器							*/
/************************************************************/
void get_speed()
{
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