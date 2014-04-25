#include "main.h"
//-------------------------------------------------------
//�������˲������λ�������
//-------------------------------------------------------
void Kalman_Filter(void)                        
{
	static  float Q_angle=0.001, Q_gyro=0.003, R_angle=0.5, dt=0.005;//ע�⣺dt��ȡֵΪkalman�˲�������ʱ��;
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
	//angular_speed = gyro-q_bias;//�˲���Ľ��ٶ�
}
//------------------------------------------------------
//�����˲�������δ������û��
//------------------------------------------------------
void Complementary_Filter()
{
	static float dt = 0.005,t = 0.5,A = 0.92;
	//A = 0.98;//t/(t+dt);
	angle = A*(angle + gyro * dt) + (1-A)*(gravity);
}
//------------------------------------------------------
//��ȡ���ٶȴ��������
//------------------------------------------------------
void Get_Gravity_Zero()
{
	unsigned int i;
	gravity_zero = AD_Capture(1);
	for(i = 0;i <= 1000;i++)
	{
		gravity_zero = (int)(gravity_zero*0.9 + AD_Capture(0)*0.1);
	}
	UART_SendString(0,"get gravity zero point:");
	UART_Sendgraph(0,0,gravity_zero);
}
//------------------------------------------------------
//��ȡ���������
//------------------------------------------------------
void Get_Gyro_Zero()
{
	unsigned int i;
	gyro_zero = AD_Capture(0);
	for(i = 0;i <= 1000;i++)
	{
		gyro_zero = (int)(gyro_zero*0.9 + AD_Capture(1)*0.1);
	}
	UART_SendString(0,"get gyro zero point:");
	UART_Sendgraph(0,0,gyro_zero);
}
//------------------------------------------------------
//��ȡ���ٶȼƵó��ĽǶ�ֵ����Ϊ��ǰ��
//------------------------------------------------------
void Get_Gravity(void)
{
	uint8 i;
	int AD_gravity=0,gravity_error;
	for(i=0;i<10;i++)
	{
		AD_gravity+=AD_Capture(1)*0.1;
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
//��ȡ��ǰ�����ǵó��Ľ��ٶ�ֵ(��Ϊ��ǰת)
//------------------------------------------------------
void Get_Gyro(void)
{
	uint8 i;
	int AD_gyro=0;
	for(i=0;i<10;i++)
	{
		AD_gyro+=AD_Capture(0)*0.1;
	}
	gyro = (AD_gyro - gyro_zero)*0.45;
}
//------------------------------------------------------
//���ý��ٶȻ��ּ���Ƕȣ��������ٶȵ�ϵ����
//------------------------------------------------------
void Get_Gyro_Angle()
{
	float dt = 0.005;
	angle_gyro += gyro * dt;
}
//------------------------------------------------------
//������Ƕȵ�PID���ֵ
//------------------------------------------------------
float Angle_Out()
{
	Angle_PID.Out = Angle_PID.Proportion*(angle-set_angle+Speed_Angle_PID.Out) + Angle_PID.Derivative*gyro;//�����ÿ������˲�֮��Ľ��ٶȻ���ֱ�ӻ�ȡ�Ľ��ٶȻ�δȷ��
}