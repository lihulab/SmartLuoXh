struct PID//用于PID计算及输出
{
	float Proportion;
	float Integral;
	float Derivative;
	float Out;
	float Error;
	float Error_L;
	float Error_P;
};
struct DIR//用于方向斜率以及截距的记录
{
	float k;
	float b;
	float Qk;
	float QB;
	float Out;
};
extern float Car_speed,Set_speed_temp;//用于记录当前车的速度
extern struct DIR Dir;
extern struct PID Speed_L_PID;//左右轮的pid参数，可能可以合并
extern struct PID Speed_R_PID;
extern struct PID Angle_PID;//直立控制的pid参数
extern struct PID Speed_Angle_PID;//直接控制角度的速度闭环好像不能用吧
extern struct PID PID_k;//如果要加入kb的微分参数要用这个变量记录
extern struct PID PID_b;
extern char graph_switch,temp;//切换显示原始二值化图像和识别赛道图像的标志位
extern float gravity;//重力加速度换算得到的角度
extern float gyro;//陀螺仪采的的角速度
extern float angle;//计算得出的角度
extern float angle_gyro;
extern float set_angle;
extern int gravity_zero,gyro_zero;
extern unsigned char Image_Data[ROW][COLUMN];
extern unsigned char Image_bw[ROW][COLUMN];
extern unsigned char Image_Edge[ROW][2];
extern unsigned char Image_T;//二值化图像的值
extern int Image_Middle[ROW];
extern unsigned char VSYN_Flag,HREF_Flag;
extern unsigned int Point_C,Line_C,Line_ROW;
extern unsigned char Valid_Line;
extern unsigned char Image_disp[ROW/8+1][COLUMN];
extern unsigned char Image_Distance[ROW][2];
extern unsigned char Def_middle;
void Init_Variable();