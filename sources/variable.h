struct PID//����PID���㼰���
{
	float Proportion;
	float Integral;
	float Derivative;
	float Out;
	float Error;
	float Error_L;
	float Error_P;
};
struct DIR//���ڷ���б���Լ��ؾ�ļ�¼
{
	float k;
	float b;
	float Qk;
	float QB;
	float Out;
};
extern float Car_speed,Set_speed_temp;//���ڼ�¼��ǰ�����ٶ�
extern struct DIR Dir;
extern struct PID Speed_L_PID;//�����ֵ�pid���������ܿ��Ժϲ�
extern struct PID Speed_R_PID;
extern struct PID Angle_PID;//ֱ�����Ƶ�pid����
extern struct PID Speed_Angle_PID;//ֱ�ӿ��ƽǶȵ��ٶȱջ��������ð�
extern struct PID PID_k;//���Ҫ����kb��΢�ֲ���Ҫ�����������¼
extern struct PID PID_b;
extern char graph_switch,temp;//�л���ʾԭʼ��ֵ��ͼ���ʶ������ͼ��ı�־λ
extern float gravity;//�������ٶȻ���õ��ĽǶ�
extern float gyro;//�����ǲɵĵĽ��ٶ�
extern float angle;//����ó��ĽǶ�
extern float angle_gyro;
extern float set_angle;
extern int gravity_zero,gyro_zero;
extern unsigned char Image_Data[ROW][COLUMN];
extern unsigned char Image_bw[ROW][COLUMN];
extern unsigned char Image_Edge[ROW][2];
extern unsigned char Image_T;//��ֵ��ͼ���ֵ
extern int Image_Middle[ROW];
extern unsigned char VSYN_Flag,HREF_Flag;
extern unsigned int Point_C,Line_C,Line_ROW;
extern unsigned char Valid_Line;
extern unsigned char Image_disp[ROW/8+1][COLUMN];
extern unsigned char Image_Distance[ROW][2];
extern unsigned char Def_middle;
void Init_Variable();