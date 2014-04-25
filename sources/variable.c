#include "main.h"
float Car_speed = 0;
float Set_speed_temp=60;
float gravity;//重力加速度换算得到的角度
float gyro;//陀螺仪采的的角速度
float angle;//计算得出的角度
float angle_gyro=0;//陀螺仪积分得到
int gravity_zero = 2060;//加速度计零点
int gyro_zero = 1830;//陀螺仪零点
float set_angle = 20.3;
unsigned char Image_Data[ROW][COLUMN];//采集到的图像数组，起始点喂左上角
unsigned char Image_bw[ROW][COLUMN];
unsigned char Image_disp[ROW/8+1][COLUMN];
unsigned char Image_Edge[ROW][2];
int Image_Middle[ROW];
unsigned char Image_Distance[ROW][2];
unsigned char Image_T=100;//二值化图像的值
unsigned char VSYN_Flag,HREF_Flag;
unsigned int Point_C=0,Line_C=0,Line_ROW=0;
unsigned char Valid_Line=45;//用于记录有效行数量
unsigned char Right_miss_flag=0,Left_miss_flag=0;
unsigned char Def_middle;
struct PID Speed_L_PID;
struct PID Speed_R_PID;
struct PID Angle_PID;
struct PID Speed_Angle_PID;
struct DIR Dir;
struct PID PID_k;
struct PID PID_b;
char temp=0,graph_switch=0;//用于参数切换
void Init_Variable()
{
	//这里为要初始化的PID参数
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