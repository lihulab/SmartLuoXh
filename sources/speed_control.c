#include "main.h"
char MOTOR_DEAD_F = 0;//µç»úµÄËÀÇøÕ¼¿Õ±È
char MOTOR_DEAD_B = 0;
float Right_motor_speed=0,Left_motor_speed=0;//×óÓÒÂÖ²âµÄµÄËÙ¶È
float Set_right_speed=0,Set_left_speed=0;
float Set_speed=0;
uint32 *scount1, *scount2;

/************************************************************/
/* 				³õÊ¼»¯±àÂëÆ÷							*/
/************************************************************/
void Coder_init()
{
	MCF_GPIO_PTGPAR|=MCF_GPIO_PTGPAR_MB_A17_GPIO|MCF_GPIO_PTGPAR_MB_A18_GPIO;
	MCF_GPIO_DDRTG=0;
	MCF_GPIO_PTAPAR|=MCF_GPIO_PTAPAR_ICOC3_ICOC3|MCF_GPIO_PTAPAR_ICOC2_GPIO;
	MCF_GPIO_DDRTA&=~MCF_GPIO_DDRTA_DDRTA2;
	MCF_GPT_GPTPACTL|=MCF_GPT_GPTPACTL_PAE		//Ê¹ÄÜÂö³åÀÛ¼Ó¹¦ÄÜ
					|MCF_GPT_GPTPACTL_PEDGE//ÀÛ¼ÓÉÏÉýÑØ
					|MCF_GPT_GPTPACTL_CLK(0);    //²ÉÓÃGPTÊ±Ê±ÖÓ
	MCF_GPT_GPTPACTL&=~MCF_GPT_GPTPACTL_PAMOD;	//ÀÛ¼ÓÂö³åÊ±¼äÄ£Ê½
	MCF_GPT_GPTIE=0x00;//½ûÖ¹GPTÖÐ¶Ï
	
	MCF_DMA_DSR(1) |= MCF_DMA_DSR_DONE;					//Çå¿Õ´«ÊäÍê³É±êÖ¾Î»
	MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);
	MCF_SCM_DMAREQC |= MCF_SCM_DMAREQC_DMAC1(0x07);		//Éè¶¨DMA3ÎªDTIM3´¥·¢
	MCF_DMA_SAR(1)=(uint32)scount1;			//Ô´µØÖ·ÎªPAN
	MCF_DMA_DAR(1)=(uint32)scount2;				//Ä¿µÄµØÖ·ÎªÍ¼ÏñÊý×é
	MCF_DMA_BCR(1)=1000;
	MCF_DMA_DCR(1)=MCF_DMA_DCR_SSIZE(1)
				  |MCF_DMA_DCR_DSIZE(1)		//Ã¿´Î´«ÊäÔ´µØÖ·ºÍÄ¿µÄµØÖ·¶¼ÎªÒ»×Ö½Ú
				  |MCF_DMA_DCR_D_REQ						//Ã¿´Î´«Êä½áÊøÖ®ºó¹Ø±ÕDMAÍâ²¿ÇëÇó
				  |MCF_DMA_DCR_EEXT
				  |MCF_DMA_DCR_CS;						//Ò»´ÎDMAÇëÇó´«ÊäÒ»´Î
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN3_DTIN3;		//Éè¶¨DTIN3¶Ë¿ÚµÄ¹¦ÄÜ
	MCF_DTIM3_DTMR = 0;								//ÖØÖÃSTMR¼Ä´æÆ÷
	MCF_DTIM3_DTMR = MCF_DTIM_DTMR_CE_RISE			//²¶×½ÉÏÉýÑØ
				   |MCF_DTIM_DTMR_CLK_DTIN;				//Ê±ÖÓÊäÈëÎªDTIN
	MCF_DTIM3_DTER |= MCF_DTIM_DTER_CAP;				//±ßÑØ²¶×½´¥·¢
	MCF_DTIM3_DTXMR|= MCF_DTIM_DTXMR_DMAEN;			//DMAÇëÇóÊ¹ÄÜ
	MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;				//¿ªÆôDTIM	
}
/************************************************************/
/* 				»ñÈ¡ËÙ¶È								*/
/************************************************************/
void get_speed()//ÏòÇ°ÎªÕý£¬ÏòºóÎª¸º
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
	if((MCF_GPIO_SETTG&MCF_GPIO_SETTG_SETTG2)==0) Right_motor_speed=Right_motor_speed-MCF_GPT_GPTPACNT/5.0;//ÒªÍ¨¹ý²âÁ¿Ëã³öÒ»¸öÂö³å´ú±íµÄ¾àÀë
	else Right_motor_speed = Right_motor_speed +(MCF_GPT_GPTPACNT/5.0);
	MCF_GPT_GPTPACNT=0;
	MCF_DMA_BCR(1)=1000;
	//Á½ÂÖµÄËÙ¶ÈÆ½¾ùÖµ×÷Îª³µÏòÇ°µÄËÙ¶È£¬¸ÃËÙ¶È×÷Îª³µËÙ¶È±Õ»·µÄÊäÈëÁ¿

}
/************************************************************/
/*                       ³õÊ¼»¯PWMÄ£¿é                      */
/************************************************************/
void PWM_INIT(void)
{
	MCF_GPIO_PTHPAR|=MCF_GPIO_PTHPAR_MB_D5_GPIO|MCF_GPIO_PTHPAR_MB_D6_GPIO;
	MCF_GPIO_DDRTH|=MCF_GPIO_DDRTH_DDRTH3|MCF_GPIO_DDRTH_DDRTH4;
	MCF_GPIO_PORTTH|=MCF_GPIO_PORTTH_PORTTH3|MCF_GPIO_PORTTH_PORTTH4;
	//Ê¹ÄÜPWMÒý½Å¹¦Ä
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
	//ÉèÖÃPWM¼«ÐÔ£¬¸ßÓÐÐ§
	MCF_PWM_PWMPOL|=MCF_PWM_PWMPOL_PPOL1
				  //|MCF_PWM_PWMPOL_PPOL2
				  |MCF_PWM_PWMPOL_PPOL3;
				  //|MCF_PWM_PWMPOL_PPOL4;

	//ÉèÖÃAÊ±ÖÓÆµÂÊºÍBÊ±ÖÓÆµÂÊ£¬·ÖÆµÏµÊýÎª4£¬×ÜÏßÆµÂÊ¾­¹ý·ÖÆµºóÎª10MHz
	MCF_PWM_PWMPRCLK=MCF_PWM_PWMPRCLK_PCKA(0)
					|MCF_PWM_PWMPRCLK_PCKB(0);

	//ÉèÖÃAÊ±ÖÓÆµÂÊºÍBÊ±ÖÓÆµÂÊ£¬ SA=Clock A/2*PWMSCLA; SB=Clock B/2*PWMSCLB
	MCF_PWM_PWMSCLA=1;    //SAÊ±ÖÓÆµÂÊÎª1MHz
	MCF_PWM_PWMSCLB=1;    //SBÊ±ÖÓÆµÂÊÎª1MHz


	//Ñ¡ÔñPWMÊ±ÖÓ£¬Ñ¡ÔñSA»òSBÊ±ÖÓ
	/*
	MCF_PWM_PWMCLK|=MCF_PWM_PWMCLK_PCLK1
				  //|MCF_PWM_PWMCLK_PCLK2
				  |MCF_PWM_PWMCLK_PCLK3;
				  //|MCF_PWM_PWMCLK_PCLK4;
	*/
	//ÉèÖÃPWM¶ÔÆë·½Ê½£¬ÉèÖÃÎª±ßÔµ¶ÔÆë¡£
	MCF_PWM_PWMCAE=0;


	//¸´Î»PWM¼ÆÊýÆ÷
	MCF_PWM_PWMCNT0=0;
	MCF_PWM_PWMCNT1=0;
	MCF_PWM_PWMCNT2=0;
	MCF_PWM_PWMCNT3=0;
	
	//ÉèÖÃPWMÖÜÆÚ
	MCF_PWM_PWMPER(0)=5000/256;
	MCF_PWM_PWMPER(1)=5000%256;	//10KHz£¬ÏÈ°Ñ·Ö±æÂÊÉèÎª100£¬×î¸ßÉè¶¨Îª256
	MCF_PWM_PWMPER(2)=5000/256;	//10KHz
	MCF_PWM_PWMPER(3)=5000%256;	//10KHz
	
	//ÉèÖÃPWMµÄÕ¼¿Õ±È
	MCF_PWM_PWMDTY(0)=0;
	MCF_PWM_PWMDTY(1)=0;
	MCF_PWM_PWMDTY(2)=0;
	MCF_PWM_PWMDTY(3)=0;
	
	//Ê¹ÄÜPWMÊä³ö
	MCF_PWM_PWME|=MCF_PWM_PWME_PWME1
				//|MCF_PWM_PWME_PWME2
				|MCF_PWM_PWME_PWME3;
				//|MCF_PWM_PWME_PWME4;
}
void set_motor_highduty(float Set_highdutyA,float Set_highdutyB)//ÕýÎªÏòÇ°×ß£¬¸ºÎªÏòºó×ß
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
//¼ÆËã¸ù¾ÝËÙ¶ÈµÄpidÊä³öÖµ
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