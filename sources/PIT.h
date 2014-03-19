void init_PIT0(void);
void PIT0_start(uint x);
void PIT1_start(uint x);
void PIT0_stop(void);
void PIT1_stop(void);
__declspec(interrupt:0) void PIT0_inter(void);//interrupt source 55