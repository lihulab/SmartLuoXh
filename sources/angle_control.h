extern float gravity;//�������ٶȻ���õ��ĽǶ�
extern float gyro;//�����ǲɵĵĽ��ٶ�
extern float angle;//����ó��ĽǶ�
extern float angle_gyro;
void Kalman_Filter(void);
void Complementary_filter();
void get_gravity_zero();
void get_gyro_zero();
void get_gravity(void);
void get_gyro(void);
void get_gyro_angle();
float angle_out();

extern float set_angle;