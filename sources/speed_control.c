#include "main.h"
char MOTOR_DEAD_F = 0;//���������ռ�ձ�
char MOTOR_DEAD_B = 0;
float Right_motor_speed=0,Left_motor_speed=0;//�����ֲ�ĵ��ٶ�
float Set_right_speed=0,Set_left_speed=0;
float Set_speed=0;
uint32 *scount1, *scount2;

/************************************************************/
/* 				��ʼ��������							*/
/************************************************************/
void Coder_init()
{
	MCF_GPIO_PTGPAR|=MCF_GPIO_PTGPAR_MB_A17_GPIO|MCF_GPIO_PTGPAR_MB_A18_GPIO;
	MCF_GPIO_DDRTG=0;
	MCF_GPIO_PTAPAR|=MCF_GPIO_PTAPAR_ICOC3_ICOC3|MCF_GPIO_PTAPAR_ICOC2_GPIO;
	MCF_GPIO_DDRTA&=~MCF_GPIO_DDRTA_DDRTA2;
	MCF_GPT_GPTPACTL|=MCF_GPT_GPTPACTL_PAE		//ʹ�������ۼӹ���
					|MCF_GPT_GPTPACTL_PEDGE//�ۼ�������
					|MCF_GPT_GPTPACTL_CLK(0);    //����GPTʱʱ��
	MCF_GPT_GPTPACTL&=~MCF_GPT_GPTPACTL_PAMOD;	//�ۼ�����ʱ��ģʽ
	MCF_GPT_GPTIE=0x00;//��ֹGPT�ж�
	
	MCF_DMA_DSR(1) |= MCF_DMA_DSR_DONE;					//��մ�����ɱ�־λ
	MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);
	MCF_SCM_DMAREQC |= MCF_SCM_DMAREQC_DMAC1(0x07);		//�趨DMA3ΪDTIM3����
	MCF_DMA_SAR(1)=(uint32)scount1;			//Դ��ַΪPAN
	MCF_DMA_DAR(1)=(uint32)scount2;				//Ŀ�ĵ�ַΪͼ������
	MCF_DMA_BCR(1)=1000;
	MCF_DMA_DCR(1)=MCF_DMA_DCR_SSIZE(1)
				  |MCF_DMA_DCR_DSIZE(1)		//ÿ�δ���Դ��ַ��Ŀ�ĵ�ַ��Ϊһ�ֽ�
				  |MCF_DMA_DCR_D_REQ						//ÿ�δ������֮��ر�DMA�ⲿ����
				  |MCF_DMA_DCR_EEXT
				  |MCF_DMA_DCR_CS;						//һ��DMA������һ��
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN3_DTIN3;		//�趨DTIN3�˿ڵĹ���
	MCF_DTIM3_DTMR = 0;								//����STMR�Ĵ���
	MCF_DTIM3_DTMR = MCF_DTIM_DTMR_CE_RISE			//��׽������
				   |MCF_DTIM_DTMR_CLK_DTIN;				//ʱ������ΪDTIN
	MCF_DTIM3_DTER |= MCF_DTIM_DTER_CAP;				//���ز�׽����
	MCF_DTIM3_DTXMR|= MCF_DTIM_DTXMR_DMAEN;			//DMA����ʹ��
	MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;				//����DTIM	
}
/************************************************************/
/* 				��ȡ�ٶ�								*/
/************************************************************/
void get_speed()//��ǰΪ�������Ϊ��
{
	if((MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG1)==0) 
	{
		Left_motor_speed = Left_motor_speed-(1000-MCF_DMA_BCR(1)&0xffffff)/5.0;
	}
	else
	{
		Left_motor_speed = Left_motor_speed+(1000 - MCF_DMA_BCR(1)&0xffffff)/5.0;
		//Left_motor_speed = -Left_motor_speed;
	}
	if((MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG2)==0) Right_motor_speed=Right_motor_speed-MCF_GPT_GPTPACNT/5.0;//Ҫͨ���������һ���������ľ���
	else Right_motor_speed = Right_motor_speed +(MCF_GPT_GPTPACNT/5.0);
	MCF_GPT_GPTPACNT=0;
	MCF_DMA_BCR(1)=1000;
	//���ֵ��ٶ�ƽ��ֵ��Ϊ����ǰ���ٶȣ����ٶ���Ϊ���ٶȱջ���������

}
/************************************************************/
/*                       ��ʼ��PWMģ��                      */
/************************************************************/
void PWM_INIT(void)
{
	MCF_GPIO_PTHPAR|=MCF_GPIO_PTHPAR_MB_D5_GPIO|MCF_GPIO_PTHPAR_MB_D6_GPIO;
	MCF_GPIO_DDRTH|=MCF_GPIO_DDRTH_DDRTH3|MCF_GPIO_DDRTH_DDRTH4;
	MCF_GPIO_PORTTH|=MCF_GPIO_PORTTH_PORTTH3|MCF_GPIO_PORTTH_PORTTH4;
	//ʹ��PWM���Ź��
	/*
	MCF_GPIO_PTCPAR=MCF_GPIO_PTCPAR_DTIN1_PWM2
				   |MCF_GPIO_PTCPAR_DTIN2_PWM4;
	*/
	MCF_GPIO_PTCPAR=MCF_GPIO_PTCPAR_DTIN1_GPIO
				   |MCF_GPIO_PTCPAR_DTIN2_GPIO;
	
	MCF_GPIO_PTAPAR=MCF_GPIO_PTAPAR_ICOC0_PWM1
				   |MCF_GPIO_PTAPAR_ICOC1_PWM3;
	/*
	MCF_GPIO_PTAPAR|=MCF_GPIO_PTAPAR_ICOC0_GPIO
					|MCF_GPIO_PTAPAR_ICOC1_GPIO;
	MCF_GPIO_DDRTA|=MCF_GPIO_DDRTA_DDRTA0
					|MCF_GPIO_DDRTA_DDRTA1;
	MCF_GPIO_PORTTA&=~MCF_GPIO_PORTTA_PORTTA0;
	MCF_GPIO_PORTTA&=~MCF_GPIO_PORTTA_PORTTA1;
	*/
	MCF_GPIO_DDRTC|=MCF_GPIO_DDRTC_DDRTC1
					|MCF_GPIO_DDRTC_DDRTC2;
	MCF_GPIO_PORTTC&=~MCF_GPIO_PORTTC_PORTTC1;
	MCF_GPIO_PORTTC&=~MCF_GPIO_PORTTC_PORTTC2;
	MCF_PWM_PWMCTL=MCF_PWM_PWMCTL_CON01
				  |MCF_PWM_PWMCTL_CON23;
	//����PWM���ԣ�����Ч
	MCF_PWM_PWMPOL|=MCF_PWM_PWMPOL_PPOL1
				  //|MCF_PWM_PWMPOL_PPOL2
				  |MCF_PWM_PWMPOL_PPOL3;
				  //|MCF_PWM_PWMPOL_PPOL4;

	//����Aʱ��Ƶ�ʺ�Bʱ��Ƶ�ʣ���Ƶϵ��Ϊ4������Ƶ�ʾ�����Ƶ��Ϊ10MHz
	MCF_PWM_PWMPRCLK=MCF_PWM_PWMPRCLK_PCKA(0)
					|MCF_PWM_PWMPRCLK_PCKB(0);

	//����Aʱ��Ƶ�ʺ�Bʱ��Ƶ�ʣ� SA=Clock A/2*PWMSCLA; SB=Clock B/2*PWMSCLB
	MCF_PWM_PWMSCLA=1;    //SAʱ��Ƶ��Ϊ1MHz
	MCF_PWM_PWMSCLB=1;    //SBʱ��Ƶ��Ϊ1MHz


	//ѡ��PWMʱ�ӣ�ѡ��SA��SBʱ��
	/*
	MCF_PWM_PWMCLK|=MCF_PWM_PWMCLK_PCLK1
				  //|MCF_PWM_PWMCLK_PCLK2
				  |MCF_PWM_PWMCLK_PCLK3;
				  //|MCF_PWM_PWMCLK_PCLK4;
	*/
	//����PWM���뷽ʽ������Ϊ��Ե���롣
	MCF_PWM_PWMCAE=0;


	//��λPWM������
	MCF_PWM_PWMCNT0=0;
	MCF_PWM_PWMCNT1=0;
	MCF_PWM_PWMCNT2=0;
	MCF_PWM_PWMCNT3=0;
	
	//����PWM����
	MCF_PWM_PWMPER(0)=5000/256;
	MCF_PWM_PWMPER(1)=5000%256;	//10KHz���Ȱѷֱ�����Ϊ100������趨Ϊ256
	MCF_PWM_PWMPER(2)=5000/256;	//10KHz
	MCF_PWM_PWMPER(3)=5000%256;	//10KHz
	
	//����PWM��ռ�ձ�
	MCF_PWM_PWMDTY(0)=0;
	MCF_PWM_PWMDTY(1)=0;
	MCF_PWM_PWMDTY(2)=0;
	MCF_PWM_PWMDTY(3)=0;
	
	//ʹ��PWM���
	MCF_PWM_PWME|=MCF_PWM_PWME_PWME1
				//|MCF_PWM_PWME_PWME2
				|MCF_PWM_PWME_PWME3;
				//|MCF_PWM_PWME_PWME4;
}
void set_motor_highduty(float Set_highdutyA,float Set_highdutyB)//��Ϊ��ǰ�ߣ���Ϊ�����
{
	if((Set_highdutyA>4500)&&(Set_highdutyA<10000)) Set_highdutyA=4500;
	else if(Set_highdutyA>=10000) Set_highdutyA=0;
	if((Set_highdutyA<(-4500))&&(Set_highdutyA>(-10000)))  Set_highdutyA=-4500;
	else if(Set_highdutyA<=(-10000)) Set_highdutyA=0;
	if((Set_highdutyB>4500)&&(Set_highdutyB<10000)) Set_highdutyB=4500;
	else if(Set_highdutyB>=10000) Set_highdutyB=0;
	if((Set_highdutyB<(-4500))&&(Set_highdutyB>(-10000))) Set_highdutyB=-4500;
	else if(Set_highdutyB<=(-10000)) Set_highdutyB=0;
	if((Set_highdutyA<0)&&(Set_highdutyA>=(-5000)))
	{
		MCF_GPIO_PORTTC|=MCF_GPIO_PORTTC_PORTTC1;
		MCF_PWM_PWMDTY(0)=((int)(5000+Set_highdutyA-MOTOR_DEAD_B)/256);
		MCF_PWM_PWMDTY(1)=((int)(5000+Set_highdutyA-MOTOR_DEAD_B)%256);
	}
	else if((Set_highdutyA>0)&&(Set_highdutyA<=5000))
	{
		MCF_GPIO_PORTTC&=~MCF_GPIO_PORTTC_PORTTC1;
		MCF_PWM_PWMDTY(0)=((int)(Set_highdutyA+MOTOR_DEAD_F)/256);
		MCF_PWM_PWMDTY(1)=((int)(Set_highdutyA+MOTOR_DEAD_F)%256);
	}
	else
	{
		MCF_GPIO_PORTTC&=~MCF_GPIO_PORTTC_PORTTC1;
		MCF_PWM_PWMDTY(0)=0;
		MCF_PWM_PWMDTY(1)=0;
	}
	if((Set_highdutyB<0)&&(Set_highdutyB>=(-5000)))
	{
		MCF_GPIO_PORTTC|=MCF_GPIO_PORTTC_PORTTC2;
		MCF_PWM_PWMDTY(2)=((int)(5000+Set_highdutyB-MOTOR_DEAD_B)/256);	
		MCF_PWM_PWMDTY(3)=((int)(5000+Set_highdutyB-MOTOR_DEAD_B)%256);	
	}
	else if((Set_highdutyB>0)&&(Set_highdutyB<=5000))
	{
		MCF_GPIO_PORTTC&=~MCF_GPIO_PORTTC_PORTTC2;
		MCF_PWM_PWMDTY(2)=((int)(Set_highdutyB+MOTOR_DEAD_F)/256);	
		MCF_PWM_PWMDTY(3)=((int)(Set_highdutyB+MOTOR_DEAD_F)%256);		
	}
	else
	{
		MCF_GPIO_PORTTC&=~MCF_GPIO_PORTTC_PORTTC2;
		MCF_PWM_PWMDTY(2)=0;
		MCF_PWM_PWMDTY(3)=0;
	}
}
//-----------------------------------------------
//��������ٶȵ�pid���ֵ
//-----------------------------------------------
void speed_out(float target_speed)
{
	float SpeedA_out=0,SpeedB_out=0,tempA_error=0,tempB_error=0;
	tempA_error=Car_speed- target_speed;
	tempB_error=Car_speed- target_speed;
	//if(((Speed_L_PID.Error-Speed_L_PID.Error_L)>5)||((Speed_L_PID.Error-Speed_L_PID.Error_L)<-5))
		Speed_L_PID.Error=target_speed-Car_speed;
	Speed_L_PID.Out-=Speed_L_PID.Proportion*(Speed_L_PID.Error-Speed_L_PID.Error_L)+Speed_L_PID.Integral*Speed_L_PID.Error+Speed_L_PID.Derivative*(Speed_L_PID.Error-2*Speed_L_PID.Error_L+Speed_L_PID.Error_P);
	//if(Speed_L_PID.Out<-4500) Speed_L_PID.Out=-3500;
	//else if(Speed_L_PID.Out>4500) Speed_L_PID.Out=3500;
	Speed_L_PID.Error_P=Speed_L_PID.Error_L;
	Speed_L_PID.Error_L=Speed_L_PID.Error;
	//if(((Speed_R_PID.Error-Speed_R_PID.Error_L)>5)||((Speed_R_PID.Error-Speed_R_PID.Error_L)<-5))
		Speed_R_PID.Error=target_speed-Car_speed;
	Speed_R_PID.Out-=Speed_R_PID.Proportion*(Speed_R_PID.Error-Speed_R_PID.Error_L)+ Speed_R_PID.Integral*Speed_R_PID.Error+Speed_R_PID.Derivative*(Speed_R_PID.Error-2*Speed_R_PID.Error_L+Speed_R_PID.Error_P);
	//if(Speed_R_PID.Out<-4500) Speed_R_PID.Out=-3500;
	//else if(Speed_R_PID.Out>4500) Speed_R_PID.Out=3500;
	Speed_R_PID.Error_P=Speed_R_PID.Error_L;
	Speed_R_PID.Error_L=Speed_R_PID.Error;
}
void speed_out_angle(float target_speed)
{
	Speed_Angle_PID.Error=target_speed-Car_speed;
	Speed_Angle_PID.Out-=Speed_Angle_PID.Proportion*(Speed_Angle_PID.Error-Speed_Angle_PID.Error_L)+Speed_Angle_PID.Integral*Speed_Angle_PID.Error+Speed_Angle_PID.Derivative*(Speed_Angle_PID.Error-2*Speed_Angle_PID.Error_L+Speed_Angle_PID.Error_P);
	Speed_Angle_PID.Error_P=Speed_Angle_PID.Error_L;
	Speed_Angle_PID.Error_L=Speed_Angle_PID.Error;
}
void Dir_control()
{
	PID_k.Out=PID_k.Proportion*PID_k.Error+PID_k.Derivative*(PID_k.Error-PID_k.Error_L);
	PID_b.Out=PID_b.Proportion*PID_b.Error;
	Set_left_speed=-PID_k.Out-PID_b.Out;
	Set_right_speed=PID_k.Out+PID_b.Out;
}