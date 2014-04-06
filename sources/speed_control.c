#include "main.h"
char MOTOR_DEAD_F = 10;//电机的死区占空比
char MOTOR_DEAD_B = 7;
float Right_motor_speed=0,Left_motor_speed=0;//左右轮测的的速度
float Set_right_speed=0,Set_left_speed=0;
float Set_speed=0;
uint32 *scount1, *scount2;

/************************************************************/
/* 				初始化编码器							*/
/************************************************************/
void Coder_init()
{
	MCF_GPIO_PTGPAR|=MCF_GPIO_PTGPAR_MB_A17_GPIO|MCF_GPIO_PTGPAR_MB_A18_GPIO;
	MCF_GPIO_DDRTG=0;
	MCF_GPIO_PTAPAR|=MCF_GPIO_PTAPAR_ICOC3_ICOC3|MCF_GPIO_PTAPAR_ICOC2_GPIO;
	MCF_GPIO_DDRTA&=~MCF_GPIO_DDRTA_DDRTA2;
	MCF_GPT_GPTPACTL|=MCF_GPT_GPTPACTL_PAE		//使能脉冲累加功能
					|MCF_GPT_GPTPACTL_PEDGE//累加上升沿
					|MCF_GPT_GPTPACTL_CLK(0);    //采用GPT时时钟
	MCF_GPT_GPTPACTL&=~MCF_GPT_GPTPACTL_PAMOD;	//累加脉冲时间模式
	MCF_GPT_GPTIE=0x00;//禁止GPT中断
	
	MCF_DMA_DSR(1) |= MCF_DMA_DSR_DONE;					//清空传输完成标志位
	MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);
	MCF_SCM_DMAREQC |= MCF_SCM_DMAREQC_DMAC1(0x07);		//设定DMA3为DTIM3触发
	MCF_DMA_SAR(1)=(uint32)scount1;			//源地址为PAN
	MCF_DMA_DAR(1)=(uint32)scount2;				//目的地址为图像数组
	MCF_DMA_BCR(1)=1000;
	MCF_DMA_DCR(1)=MCF_DMA_DCR_SSIZE(1)
				  |MCF_DMA_DCR_DSIZE(1)		//每次传输源地址和目的地址都为一字节
				  |MCF_DMA_DCR_D_REQ						//每次传输结束之后关闭DMA外部请求
				  |MCF_DMA_DCR_EEXT
				  |MCF_DMA_DCR_CS;						//一次DMA请求传输一次
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN3_DTIN3;		//设定DTIN3端口的功能
	MCF_DTIM3_DTMR = 0;								//重置STMR寄存器
	MCF_DTIM3_DTMR = MCF_DTIM_DTMR_CE_RISE			//捕捉上升沿
				   |MCF_DTIM_DTMR_CLK_DTIN;				//时钟输入为DTIN
	MCF_DTIM3_DTER |= MCF_DTIM_DTER_CAP;				//边沿捕捉触发
	MCF_DTIM3_DTXMR|= MCF_DTIM_DTXMR_DMAEN;			//DMA请求使能
	MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;				//开启DTIM	
}
/************************************************************/
/* 				获取速度								*/
/************************************************************/
void get_speed()//向前为正，向后为负
{
	if((MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG1)==0) 
	{
		Left_motor_speed = Left_motor_speed+(1000-MCF_DMA_BCR(1)&0xffffff)/5.0;
	}
	else
	{
		Left_motor_speed = Left_motor_speed-(1000 - MCF_DMA_BCR(1)&0xffffff)/5.0;
		//Left_motor_speed = -Left_motor_speed;
	}
	if((MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG2)==0) Right_motor_speed=Right_motor_speed+MCF_GPT_GPTPACNT/5.0;//要通过测量算出一个脉冲代表的距离
	else Right_motor_speed = Right_motor_speed -(MCF_GPT_GPTPACNT/5.0);
	MCF_GPT_GPTPACNT=0;
	MCF_DMA_BCR(1)=1000;
	Car_speed=(Right_motor_speed+Left_motor_speed)/2.0;//两轮的速度平均值作为车向前的速度，该速度作为车速度闭环的输入量

}
/************************************************************/
/*                       初始化PWM模块                      */
/************************************************************/
void PWM_INIT(void)
{
	MCF_GPIO_PTHPAR|=MCF_GPIO_PTHPAR_MB_D5_GPIO|MCF_GPIO_PTHPAR_MB_D6_GPIO;
	MCF_GPIO_DDRTH|=MCF_GPIO_DDRTH_DDRTH3|MCF_GPIO_DDRTH_DDRTH4;
	MCF_GPIO_PORTTH|=MCF_GPIO_PORTTH_PORTTH3|MCF_GPIO_PORTTH_PORTTH4;
    //使能PWM引脚功能	
	MCF_GPIO_PTCPAR=MCF_GPIO_PTCPAR_DTIN1_PWM2
				   |MCF_GPIO_PTCPAR_DTIN2_PWM4;
	
	/*
	MCF_GPIO_PTAPAR=MCF_GPIO_PTAPAR_ICOC0_PWM1
				   |MCF_GPIO_PTAPAR_ICOC1_PWM3;
	*/
	MCF_GPIO_PTAPAR|=MCF_GPIO_PTAPAR_ICOC0_GPIO
					|MCF_GPIO_PTAPAR_ICOC1_GPIO;
	MCF_GPIO_DDRTA|=MCF_GPIO_DDRTA_DDRTA0
					|MCF_GPIO_DDRTA_DDRTA1;
	MCF_GPIO_PORTTA&=~MCF_GPIO_PORTTA_PORTTA0;
	MCF_GPIO_PORTTA&=~MCF_GPIO_PORTTA_PORTTA1;
	//设置PWM极性，高有效
	MCF_PWM_PWMPOL=//MCF_PWM_PWMPOL_PPOL1
				  MCF_PWM_PWMPOL_PPOL2
				  //|MCF_PWM_PWMPOL_PPOL3
				  |MCF_PWM_PWMPOL_PPOL4;

	//设置A时钟频率和B时钟频率，分频系数为4，总线频率经过分频后为10MHz
	MCF_PWM_PWMPRCLK=MCF_PWM_PWMPRCLK_PCKA(2)
					|MCF_PWM_PWMPRCLK_PCKB(2);

	//设置A时钟频率和B时钟频率， SA=Clock A/2*PWMSCLA; SB=Clock B/2*PWMSCLB
	MCF_PWM_PWMSCLA=4;    //SA时钟频率为1MHz
	MCF_PWM_PWMSCLB=4;    //SB时钟频率为1MHz


	//选择PWM时钟，选择SA或SB时钟
	MCF_PWM_PWMCLK=//MCF_PWM_PWMCLK_PCLK1
				  MCF_PWM_PWMCLK_PCLK2
				  //|MCF_PWM_PWMCLK_PCLK3
				  |MCF_PWM_PWMCLK_PCLK4;

	//设置PWM对齐方式，设置为边缘对齐。
	MCF_PWM_PWMCAE=0;


	//复位PWM计数器
	//MCF_PWM_PWMCNT1=0;
	MCF_PWM_PWMCNT2=0;
	//MCF_PWM_PWMCNT3=0;
	MCF_PWM_PWMCNT4=0;
	
	//设置PWM周期
	//MCF_PWM_PWMPER(1)=200;	//10KHz，先把分辨率设为100，最高设定为256
	MCF_PWM_PWMPER(2)=200;	//10KHz
	//MCF_PWM_PWMPER(3)=200;	//10KHz
	MCF_PWM_PWMPER(4)=200;
	
	//设置PWM的占空比
	//MCF_PWM_PWMDTY(1)=0;
	MCF_PWM_PWMDTY(2)=0;
	//MCF_PWM_PWMDTY(3)=0;
	MCF_PWM_PWMDTY(4)=0;
	
	//使能PWM输出
	MCF_PWM_PWME=//MCF_PWM_PWME_PWME1
				MCF_PWM_PWME_PWME2
				//|MCF_PWM_PWME_PWME3
				|MCF_PWM_PWME_PWME4;
}
void set_motor_highduty(float Set_highdutyA,float Set_highdutyB)//正为向前走，负为向后走
{
	if((Set_highdutyA>190)&&(Set_highdutyA<300)) Set_highdutyA=190;
	else if(Set_highdutyA>=300) Set_highdutyA=0;
	if((Set_highdutyA<(-190))&&(Set_highdutyA>(-300)))  Set_highdutyA=-190;
	else if(Set_highdutyA<=(-300)) Set_highdutyA=0;
	if((Set_highdutyB>190)&&(Set_highdutyB<300)) Set_highdutyB=190;
	else if(Set_highdutyB>=300) Set_highdutyB=0;
	if((Set_highdutyB<(-190))&&(Set_highdutyB>(-300))) Set_highdutyB=-190;
	else if(Set_highdutyB<=(-300)) Set_highdutyB=0;
	if((Set_highdutyA<0)&&(Set_highdutyA>=(-200)))
	{
		MCF_GPIO_PORTTA|=MCF_GPIO_PORTTA_PORTTA0;
		MCF_PWM_PWMDTY(2)=(int)(200+Set_highdutyA-MOTOR_DEAD_B);
	}
	else if((Set_highdutyA>0)&&(Set_highdutyA<=200))
	{
		MCF_GPIO_PORTTA&=~MCF_GPIO_PORTTA_PORTTA0;
		MCF_PWM_PWMDTY(2)=(int)(Set_highdutyA+MOTOR_DEAD_F);	
	}
	else
	{
		MCF_GPIO_PORTTA&=~MCF_GPIO_PORTTA_PORTTA0;
		MCF_PWM_PWMDTY(2)=0;
	}
	if((Set_highdutyB<0)&&(Set_highdutyB>=(-200)))
	{
		MCF_GPIO_PORTTA|=MCF_GPIO_PORTTA_PORTTA1;
		MCF_PWM_PWMDTY(4)=(int)(200+Set_highdutyB-MOTOR_DEAD_B);	
	}
	else if((Set_highdutyB>0)&&(Set_highdutyB<=200))
	{
		MCF_GPIO_PORTTA&=~MCF_GPIO_PORTTA_PORTTA1;
		MCF_PWM_PWMDTY(4)=(int)(Set_highdutyB+MOTOR_DEAD_F);	
	}
	else
	{
		MCF_GPIO_PORTTA&=~MCF_GPIO_PORTTA_PORTTA1;
		MCF_PWM_PWMDTY(4)=0;
	}
}
//-----------------------------------------------
//计算根据速度的pid输出值
//-----------------------------------------------
void speed_out(float target_speed)
{
	float SpeedA_out=0,SpeedB_out=0,tempA_error=0,tempB_error=0;
	tempA_error=Car_speed- target_speed;
	tempB_error=Car_speed- target_speed;
	Speed_L_PID.Error=target_speed-Car_speed;
	Speed_L_PID.Out-=Speed_L_PID.Proportion*(Speed_L_PID.Error-Speed_L_PID.Error_L)+Speed_L_PID.Integral*Speed_L_PID.Error+Speed_L_PID.Derivative*(Speed_L_PID.Error-2*Speed_L_PID.Error_L+Speed_L_PID.Error_P);
	Speed_L_PID.Error_P=Speed_L_PID.Error_L;
	Speed_L_PID.Error_L=Speed_L_PID.Error;

	Speed_R_PID.Error=target_speed-Car_speed;
	Speed_R_PID.Out-=Speed_R_PID.Proportion*(Speed_R_PID.Error-Speed_R_PID.Error_L)+ Speed_R_PID.Integral*Speed_R_PID.Error+Speed_R_PID.Derivative*(Speed_R_PID.Error-2*Speed_R_PID.Error_L+Speed_R_PID.Error_P);
	Speed_R_PID.Error_P=Speed_R_PID.Error_L;
	Speed_R_PID.Error_L=Speed_R_PID.Error;
}
void Dir_control()
{
	Dir.Out=Dir.Qk*Dir.k+Dir.QB*Dir.b;
	Set_left_speed=Dir.Out;
	Set_right_speed=-Dir.Out;
}