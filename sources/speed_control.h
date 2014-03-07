#define SPEED_DMA 10000 - MCF_DMA1_BCR	//DMA≤‚ÀŸ÷µ

extern float Right_motor_speed, Left_motor_speed;

void PWM_INIT(void);
void set_motor_highduty(char highdutyA,char highdutyB);
void Coder_init();
void get_speed();
void speed_out(float target_speed);
