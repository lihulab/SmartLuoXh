void PWM_INIT(void);
void set_motor_highduty(float highdutyA,float highdutyB);
void Coder_init();
void get_speed();
void speed_out(float target_speed);
void Dir_control();
void speed_out_angle(float target_speed);
extern float Right_motor_speed,Left_motor_speed;//�����ֲ�ĵ��ٶ�
extern float Set_speed;
extern float Set_right_speed,Set_left_speed;

