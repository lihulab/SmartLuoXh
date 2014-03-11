void PWM_INIT(void);
void set_motor_highduty(float highdutyA,float highdutyB);
void Coder_init();
void get_speed();
void speed_out(float target_speed);
extern int Right_motor_speed,Left_motor_speed;//左右轮测的的速度
extern float Set_speed;