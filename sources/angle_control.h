extern float gravity;//重力加速度换算得到的角度
extern float gyro;//陀螺仪采的的角速度
extern float angle;//计算得出的角度
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