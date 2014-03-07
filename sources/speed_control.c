#include "main.h"

char MOTOR_DEAD = 0;//���������ռ�ձ�
float Right_motor_speed=0,Left_motor_speed=0;//�����ֲ�ĵ��ٶ�
float Set_right_speed=0,Set_left_speed=0;
float Set_speed=0;
uint32 *scount1, *scount2;		//DMA����ָ������Ч�����ַ
/************************************************************/
/* 				��ʼ��������							*/
/************************************************************/
void Coder_init()
{
/*GPT����ģ��*/
	MCF_GPIO_PTAPAR|=MCF_GPIO_PTAPAR_ICOC3_ICOC3;
	MCF_GPT_GPTPACTL=MCF_GPT_GPTPACTL_PAE		//ʹ�������ۼӹ���
					&~MCF_GPT_GPTPACTL_PAMOD	//�ۼ�����ʱ��ģʽ
					|MCF_GPT_GPTPACTL_PEDGE//�ۼ�������
					|MCF_GPT_GPTPACTL_CLK(0);    //����GPTʱʱ��
	MCF_GPT_GPTIE=0x00;//��ֹGPT�ж�
	
/*DMA����ģ��*/
	MCF_DMA_DSR(1) |= MCF_DMA_DSR_DONE;					//��մ�����ɱ�־λ
	MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);
	MCF_SCM_DMAREQC = MCF_SCM_DMAREQC_DMAC1(0x05);		//�趨DMA1ΪDTIM1����
	MCF_DMA_SAR(1)=(uint32)scount1;			//Դ��ַΪPAN
	MCF_DMA_DAR(1)=(uint32)scount2;				//Ŀ�ĵ�ַΪͼ������
	MCF_DMA_BCR(1)=1000;									//���䳤��Ϊÿ�еĳ��ȣ�������320������
	MCF_DMA_DCR(1)=//MCF_DMA_DCR_SSIZE_BYTE
				  //|MCF_DMA_DCR_DSIZE_BYTE		//ÿ�δ���Դ��ַ��Ŀ�ĵ�ַ��Ϊһ�ֽ�
				  //|MCF_DMA_DCR_DINC						//ÿ�δ���֮��Դ��ַ���ֲ��䣬Ŀ�ĵ�ַ��1
				  MCF_DMA_DCR_D_REQ						//ÿ�δ������֮��ر�DMA�ⲿ����
				  |MCF_DMA_DCR_EEXT
				  |MCF_DMA_DCR_CS;						//һ��DMA������һ��
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN1_DTIN1;		//�趨DTIN0�˿ڵĹ���
	MCF_DTIM_DTMR(1) = 0;								//����STMR�Ĵ���
	MCF_DTIM_DTMR(1) = MCF_DTIM_DTMR_CE_RISE			//��׽������
				      |MCF_DTIM_DTMR_CLK_DTIN;				//ʱ������ΪDTIN
	MCF_DTIM_DTER(1) |= MCF_DTIM_DTER_CAP;				//���ز�׽����
	MCF_DTIM_DTXMR(1)|= MCF_DTIM_DTXMR_DMAEN;			//DMA����ʹ��
	MCF_DTIM_DTMR(1) |= MCF_DTIM_DTMR_RST;				//����DTIM	

}
/************************************************************/
/* 				��ȡ�ٶ�ֵ									*/
/************************************************************/
void get_speed()
{
	Left_motor_speed = 1000 - MCF_DMA1_BCR;
	MCF_DMA1_BCR = 1000;
	Right_motor_speed=MCF_GPT_GPTPACNT/0.005;//Ҫͨ���������һ���������ľ���
	MCF_GPT_GPTPACNT=0;
	Car_speed=(Right_motor_speed+Left_motor_speed)/2;//���ֵ��ٶ�ƽ��ֵ��Ϊ����ǰ���ٶȣ����ٶ���Ϊ���ٶȱջ���������
}
/************************************************************/
/*                       ��ʼ��PWMģ��                      */
/************************************************************/
void PWM_INIT(void)
{
    //ʹ��PWM���Ź���	
	MCF_GPIO_PTCPAR=MCF_GPIO_PTCPAR_DTIN1_PWM2
				   |MCF_GPIO_PTCPAR_DTIN2_PWM4;
	
	MCF_GPIO_PTAPAR=MCF_GPIO_PTAPAR_ICOC0_PWM1
				   |MCF_GPIO_PTAPAR_ICOC1_PWM3;
				   
	//����PWM���ԣ�����Ч
	MCF_PWM_PWMPOL=MCF_PWM_PWMPOL_PPOL1
				  |MCF_PWM_PWMPOL_PPOL2
				  |MCF_PWM_PWMPOL_PPOL3
				  |MCF_PWM_PWMPOL_PPOL4;

	//����Aʱ��Ƶ�ʺ�Bʱ��Ƶ�ʣ���Ƶϵ��Ϊ4������Ƶ�ʾ�����Ƶ��Ϊ10MHz
	MCF_PWM_PWMPRCLK=MCF_PWM_PWMPRCLK_PCKA(2)
					|MCF_PWM_PWMPRCLK_PCKB(2);

	//����Aʱ��Ƶ�ʺ�Bʱ��Ƶ�ʣ� SA=Clock A/2*PWMSCLA; SB=Clock B/2*PWMSCLB
	MCF_PWM_PWMSCLA=5;    //SAʱ��Ƶ��Ϊ1MHz
	MCF_PWM_PWMSCLB=5;    //SBʱ��Ƶ��Ϊ1MHz


	//ѡ��PWMʱ�ӣ�ѡ��SA��SBʱ��
	MCF_PWM_PWMCLK=MCF_PWM_PWMCLK_PCLK1
				  |MCF_PWM_PWMCLK_PCLK2
				  |MCF_PWM_PWMCLK_PCLK3
				  |MCF_PWM_PWMCLK_PCLK4;

	//����PWM���뷽ʽ������Ϊ��Ե���롣
	MCF_PWM_PWMCAE=0;


	//��λPWM������
	MCF_PWM_PWMCNT1=0;
	MCF_PWM_PWMCNT2=0;
	MCF_PWM_PWMCNT3=0;
	MCF_PWM_PWMCNT4=0;
	
	//����PWM����
	MCF_PWM_PWMPER(1)=100;	//10KHz���Ȱѷֱ�����Ϊ100������趨Ϊ256
	MCF_PWM_PWMPER(2)=100;	//10KHz
	MCF_PWM_PWMPER(3)=100;	//10KHz
	MCF_PWM_PWMPER(4)=100;
	
	//����PWM��ռ�ձ�
	MCF_PWM_PWMDTY(1)=0;
	MCF_PWM_PWMDTY(2)=0;
	MCF_PWM_PWMDTY(3)=0;
	MCF_PWM_PWMDTY(4)=0;
	
	//ʹ��PWM���
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
//��������ٶȵ�pid���ֵ
//-----------------------------------------------
void speed_out(float target_speed)
{
	Speed_PID.Error+= (Car_speed- target_speed);
	Speed_PID.Error_sum+=Speed_PID.Error;
	Speed_PID.Out=Speed_PID.Proportion*Speed_PID.Error+ Speed_PID.Integral*Speed_PID.Error_sum;
	set_motor_highduty((char)(Speed_PID.Out+ Angle_PID.Out),(char)(Speed_PID.Out+ Angle_PID.Out));//ֻ�������ٶȵĿ��ƣ�ת��Ŀ��ƻ�û�м����ȥ����ʱ׼��д������
}