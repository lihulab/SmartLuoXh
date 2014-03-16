#include"main.h"
void init_key()
{
	MCF_GPIO_PTHPAR|=MCF_GPIO_PTHPAR_MB_D0_GPIO
					|MCF_GPIO_PTHPAR_MB_D1_GPIO
					|MCF_GPIO_PTHPAR_MB_D2_GPIO
					|MCF_GPIO_PTHPAR_MB_D6_GPIO
					|MCF_GPIO_PTHPAR_MB_D7_GPIO;
	MCF_GPIO_DDRTH=0;
}
char key1_press()
{
	if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH6)!=0)
	{
		delay_ms(1);
		if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH6)!=0)
		{
			while((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH6)!=0);
			return 1;
		}
		else return 0;
	}
	else return 0;
}
char key2_press()
{
	if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH7)!=0)
	{
		delay_ms(1);
		if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH7)!=0)
		{
			while((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH7)!=0);
			return 1;
		}
		else return 0;
	}
	else return 0;
}
char key3_press()
{
	if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH0)!=0)
	{
		delay_ms(1);
		if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH0)!=0)
		{
			while((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH0)!=0);
			return 1;
		}
		else return 0;
	}
	else return 0;
}
char key4_press()
{
	if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH1)!=0)
	{
		delay_ms(1);
		if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH1)!=0)
		{
			while((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH1)!=0);
			return 1;
		}
		else return 0;
	}
	else return 0;
}
char key5_press()
{
	if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH2)!=0)
	{
		delay_ms(1);
		if((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH2)!=0)
		{
			while((MCF_GPIO_SETTH&MCF_GPIO_SETTH_SETTH2)!=0);
			return 1;
		}
		else return 0;
	}
	else return 0;
}