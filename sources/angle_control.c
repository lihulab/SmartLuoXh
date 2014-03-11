#include "main.h"
float gravity;//�������ٶȻ���õ��ĽǶ�
float gyro;//�����ǲɵĵĽ��ٶ�
float angle;//����ó��ĽǶ�
float angular_speed;//�������˲��õ��Ľ��ٶ�
float angle_gyro=0;//�����ǻ��ֵõ�
int gravity_zero = 2060;//���ٶȼ����
int gyro_zero = 1810;//���������
float set_angle = 0.0;
//-------------------------------------------------------
//�������˲������λ�������
//-------------------------------------------------------
void Kalman_Filter(void)                        
{
	static  float Q_angle=0.001, Q_gyro=0.003, R_angle=0.05, dt=0.0055;//ע�⣺dt��ȡֵΪkalman�˲�������ʱ��;
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
    angular_speed = gyro-q_bias;//�˲���Ľ��ٶ�
}
//------------------------------------------------------
//�����˲�������δ����
//------------------------------------------------------
void Complementary_filter()
{
	static float dt = 0.005,t = 0.5,A = 0.92;
	//A = 0.98;//t/(t+dt);
	angle = A*(angle + gyro * dt) + (1-A)*(gravity);
}
//------------------------------------------------------
//��ȡ���ٶȴ��������
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
//��ȡ���������
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
//��ȡ���ٶȼƵó��ĽǶ�ֵ
//------------------------------------------------------
void get_gravity(void)
{
	uint8 i;
	int AD_gravity=0,gravity_error;
	for(i=0;i<10;i++)
	{
		AD_gravity+=AD_capture(1)*0.1;
	}
	//���÷����Ǻ�������һ��
	if((gravity_zero - AD_gravity)>810)
		gravity_error = 810;
	else if((gravity_zero - AD_gravity)<-810)
		gravity_error = -810;
	else gravity_error = gravity_zero - AD_gravity;
	gravity = (asinf((gravity_error)/810.0))*57.2957;
	
	//gravity = (gravity_zero - AD_gravity)*0.1058;//���Թ�һ��,����Ҫ���µ���������Ч������
}
//------------------------------------------------------
//��ȡ��ǰ�����ǵó��Ľ��ٶ�ֵ
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
//���ý��ٶȻ��ּ���Ƕȣ��������ٶȵ�ϵ����
//------------------------------------------------------
void get_gyro_angle()
{
	float dt = 0.005;
	angle_gyro += gyro * dt;
}
float angle_out()
{
	Angle_PID.Out = Angle_PID.Proportion*(set_angle-angle) + Angle_PID.Derivative*(-gyro);//�����ÿ������˲�֮��Ľ��ٶȻ���ֱ�ӻ�ȡ�Ľ��ٶȻ�δȷ��
}