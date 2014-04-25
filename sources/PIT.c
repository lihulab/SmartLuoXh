#include "main.h"
/************************************************************/
/*                       初始化PIT0                         */
/************************************************************/
void init_PIT0(void)
{		

	MCF_PIT_PCSR(0)=MCF_PIT_PCSR_RLD     //重新载入初始值 
				   |MCF_PIT_PCSR_PIF     //清除中断标志位
				   |MCF_PIT_PCSR_PIE//使能中断
				   |MCF_PIT_PCSR_OVW     //允许覆盖
				   |MCF_PIT_PCSR_PRE(9); //分频系数为2^9=512，
				                         //时钟源的频率为40MHz，
				                         //分频之后频率为78125Hz。
		                         		 //at 80MHz
										 //0- 40000000Hz
										 //1- 20000000Hz
										 //2- 10000000Hz
										 //3- 5000000Hz
										 //4- 2500000Hz -0.4us
										 //5- 1250000Hz
										 //6- 625000Hz
										 //7- 312500Hz
										 //8- 156250Hz
										 //9- 78125Hz   -12.8us
										 //10-39062.5Hz
										 //11-19531.25Hz
										 //12-9765.625Hz
										 //13-4882.8125Hz
										 //14-2441.40625Hz
										 //15-1220.703125Hz
	MCF_PIT_PMR(0)=(uint16)(250000/200);    //计数时间为5ms  ，作为编码器和直立采样周期合适，摄像头控制未知
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;     //使能中断功能
	MCF_INTC0_IMRH&=~MCF_INTC_IMRH_INT_MASK55;  //使能PIT0中断
	MCF_INTC0_ICR55=MCF_INTC_ICR_IP(5)+MCF_INTC_ICR_IL(2); //设置PIT0中断优先级
	//MCF_INTC0_ICR55=MCF_INTC_ICR_IP(6)+MCF_INTC_ICR_IL(6); 
	                          //IL可设置7个级别，IP设置每个级别中的优先级。
	MCF_PIT_PCSR(0)|=MCF_PIT_PCSR_EN;    //使能PIT0   
}
void PIT0_start(uint x)
{
	MCF_PIT_PMR(0)=x;

	MCF_PIT_PCSR(0)|=MCF_PIT_PCSR_EN;
}

void PIT1_start(uint x)
{
	MCF_PIT_PMR(1)=x;

	MCF_PIT_PCSR(1)|=MCF_PIT_PCSR_EN;
}

void PIT0_stop(void)
{

	MCF_PIT_PCSR(0)&=~MCF_PIT_PCSR_EN;
}

void PIT1_stop(void)
{

	MCF_PIT_PCSR(1)&=~MCF_PIT_PCSR_EN;
}

__declspec(interrupt:0) void PIT0_inter(void)//interrupt source 55
{	
	static int i=0;
	i++;
	MCF_PIT_PCSR(0)|=MCF_PIT_PCSR_PIF;//清除PIT标志位
	Blink_LED1();
	Get_Gyro();
	Get_Gravity();
	Get_Gyro_Angle();
	Kalman_Filter();
	Angle_Out();
	Dir_control();
	get_speed();
	if(i==5)
	{
		Car_speed=(Right_motor_speed+Left_motor_speed)/2.0;
		speed_out(Set_speed);
		//speed_out_angle(Set_speed);
		Right_motor_speed=0;
		Left_motor_speed=0;
		i=0;
	}
	set_motor_highduty(Speed_L_PID.Out+Angle_PID.Out+Set_left_speed,Speed_R_PID.Out+Angle_PID.Out+Set_right_speed);
	//set_motor_highduty(Set_left_speed,Set_right_speed);
	//set_motor_highduty(2500,2500);
	//set_motor_highduty(Speed_L_PID.Out-Angle_PID.Out,Speed_R_PID.Out-Angle_PID.Out);
	//set_motor_highduty(-Angle_PID.Out+Set_left_speed,-Angle_PID.Out+Set_right_speed);
	//set_motor_highduty(Speed_L_PID.Out,Speed_R_PID.Out);
}
__declspec(interrupt:0) void PIT1_handler(void)//interrupt source 56
{
	MCF_PIT_PCSR(1)|=MCF_PIT_PCSR_PIF;//清除PIT标志位
}