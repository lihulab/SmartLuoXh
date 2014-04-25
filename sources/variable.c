#include "main.h"
float Car_speed = 0;
float Set_speed_temp=60;
float gravity;//�������ٶȻ���õ��ĽǶ�
float gyro;//�����ǲɵĵĽ��ٶ�
float angle;//����ó��ĽǶ�
float angle_gyro=0;//�����ǻ��ֵõ�
int gravity_zero = 2060;//���ٶȼ����
int gyro_zero = 1830;//���������
float set_angle = 20.3;
unsigned char Image_Data[ROW][COLUMN];//�ɼ�����ͼ�����飬��ʼ��ι���Ͻ�
unsigned char Image_bw[ROW][COLUMN];
unsigned char Image_disp[ROW/8+1][COLUMN];
unsigned char Image_Edge[ROW][2];
int Image_Middle[ROW];
unsigned char Image_Distance[ROW][2];
unsigned char Image_T=100;//��ֵ��ͼ���ֵ
unsigned char VSYN_Flag,HREF_Flag;
unsigned int Point_C=0,Line_C=0,Line_ROW=0;
unsigned char Valid_Line=45;//���ڼ�¼��Ч������
unsigned char Right_miss_flag=0,Left_miss_flag=0;
unsigned char Def_middle;
struct PID Speed_L_PID;
struct PID Speed_R_PID;
struct PID Angle_PID;
struct PID Speed_Angle_PID;
struct DIR Dir;
struct PID PID_k;
struct PID PID_b;
char temp=0,graph_switch=0;//���ڲ����л�
void Init_Variable()
{
	//����ΪҪ��ʼ����PID����
	Angle_PID.Proportion = 450;
	Angle_PID.Derivative = 12;
	Speed_L_PID.Proportion = 114;
	Speed_L_PID.Derivative = -24;
	Speed_L_PID.Integral = 0.0;
	Speed_L_PID.Error_P = 0;
	Speed_L_PID.Error_L = 0;
	Speed_R_PID.Proportion = 114;
	Speed_R_PID.Derivative = -24;
	Speed_R_PID.Integral = 0.0;
	Speed_R_PID.Error_P = 0;
	Speed_R_PID.Error_L = 0;
	Speed_Angle_PID.Proportion=0;
	Speed_Angle_PID.Derivative=0;
	Speed_Angle_PID.Integral=0;
	Dir.Qk=5;
	Dir.QB=19;
	PID_k.Proportion=5;
	PID_k.Derivative=0;
	PID_b.Proportion=19;
	PID_b.Derivative=0;
}