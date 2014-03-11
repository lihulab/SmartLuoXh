#include "main.h"
float gravity;//重力加速度换算得到的角度
float gyro;//陀螺仪采的的角速度
float angle;//计算得出的角度
float angular_speed;//卡尔曼滤波得到的角速度
float angle_gyro=0;//陀螺仪积分得到
int gravity_zero = 2060;//加速度计零点
int gyro_zero = 1810;//陀螺仪零点
float set_angle = 0.0;
//-------------------------------------------------------
//卡尔曼滤波，波形基本可用
//-------------------------------------------------------
void Kalman_Filter(void)                        
{
	static  float Q_angle=0.001, Q_gyro=0.003, R_angle=0.05, dt=0.0055;//注意：dt的取值为kalman滤波器采样时间;
	static float Pk[2][2] = { {1, 0 }, {0, 1 }};
	static float Pdot[4] ={0,0,0,0};
	static const float C_0 = 1;
	static float q_bias = 0, angle_err = 0, PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;
    angle+=(gyro-q_bias) * dt;
    Pdot[0]=Q_angle - Pk[0][1] - Pk[1][0];
    Pdot[1]=- Pk[1][1];
    Pdot[2]=- Pk[1][1];
    Pdot[3]=Q_gyro;
    
    Pk[0][0] += Pdot[0] * dt;
    Pk[0][1] += Pdot[1] * dt;
    Pk[1][0] += Pdot[2] * dt;
    Pk[1][1] += Pdot[3] * dt;
    
    
    angle_err = gravity - angle;
    
    
    PCt_0 = C_0 * Pk[0][0];
    PCt_1 = C_0 * Pk[1][0];
    
    E = R_angle + C_0 * PCt_0;
    
    K_0 = PCt_0*1.0/ E;
    K_1 = PCt_1*1.0/ E;
    
    t_0 = PCt_0;
    t_1 = C_0 * Pk[0][1];

    Pk[0][0] -= K_0 * t_0;
    Pk[0][1] -= K_0 * t_1;
    Pk[1][0] -= K_1 * t_0;
    Pk[1][1] -= K_1 * t_1;
    
    
    angle        += K_0 * angle_err;
    q_bias        += K_1 * angle_err;
    angular_speed = gyro-q_bias;//滤波后的角速度
}
//------------------------------------------------------
//互补滤波，参数未整定
//------------------------------------------------------
void Complementary_filter()
{
	static float dt = 0.005,t = 0.5,A = 0.92;
	//A = 0.98;//t/(t+dt);
	angle = A*(angle + gyro * dt) + (1-A)*(gravity);
}
//------------------------------------------------------
//获取加速度传感器零点
//------------------------------------------------------
void get_gravity_zero()
{
	unsigned int i;
	gravity_zero = AD_capture(1);
	for(i = 0;i <= 1000;i++)
	{
		gravity_zero = (int)(gravity_zero*0.9 + AD_capture(0)*0.1);
	}
	UART_SendString(0,"get gravity zero point:");
	UART_Sendgraph(0,0,gravity_zero);
}
//------------------------------------------------------
//获取陀螺仪零点
//------------------------------------------------------
void get_gyro_zero()
{
	unsigned int i;
	gyro_zero = AD_capture(0);
	for(i = 0;i <= 1000;i++)
	{
		gyro_zero = (int)(gyro_zero*0.9 + AD_capture(1)*0.1);
	}
	UART_SendString(0,"get gyro zero point:");
	UART_Sendgraph(0,0,gyro_zero);
}
//------------------------------------------------------
//获取加速度计得出的角度值
//------------------------------------------------------
void get_gravity(void)
{
	uint8 i;
	int AD_gravity=0,gravity_error;
	for(i=0;i<10;i++)
	{
		AD_gravity+=AD_capture(1)*0.1;
	}
	//利用反三角函数做归一化
	if((gravity_zero - AD_gravity)>810)
		gravity_error = 810;
	else if((gravity_zero - AD_gravity)<-810)
		gravity_error = -810;
	else gravity_error = gravity_zero - AD_gravity;
	gravity = (asinf((gravity_error)/810.0))*57.2957;
	
	//gravity = (gravity_zero - AD_gravity)*0.1058;//线性归一化,参数要重新调整，现在效果不好
}
//------------------------------------------------------
//获取当前陀螺仪得出的角速度值
//------------------------------------------------------
void get_gyro(void)
{
	uint8 i;
	int AD_gyro=0;
	for(i=0;i<10;i++)
	{
		AD_gyro+=AD_capture(0)*0.1;
	}
	gyro = (AD_gyro - gyro_zero)*0.25;
}
//------------------------------------------------------
//利用角速度积分计算角度，整定角速度的系数用
//------------------------------------------------------
void get_gyro_angle()
{
	float dt = 0.005;
	angle_gyro += gyro * dt;
}
float angle_out()
{
	Angle_PID.Out = Angle_PID.Proportion*(set_angle-angle) + Angle_PID.Derivative*(-gyro);//到底用卡尔曼滤波之后的角速度还是直接获取的角速度还未确定
}