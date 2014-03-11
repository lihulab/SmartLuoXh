#include "main.h"
char AT_cmd[10];
char i = 0;
char str_flag = 0;
/************************************************************/
/*                    ��ʼ��UART����                        */
/*                uartNoΪ��Ҫ���õĴ��ڱ��                */
/*                   sysclkΪϵͳʱ��Ƶ��                   */
/*                baudΪ��Ҫ���õĴ��ڵĲ�����              */
/*                intStatus: Ϊ��ʼ���жϵ�״̬�JY-MC�           */
/*                1��ʾ���ж� ������ʾ���ж�                */
/************************************************************/
void UART_INIT(uint8 uartNo, int32 sysclk, int32 baud, uint8 intStatus)
{
	register uint16 ubgs;
	MCF_GPIO_PTJPAR|=MCF_GPIO_PTJPAR_FEC_TXD0_GPIO;
	MCF_GPIO_DDRTJ&=~MCF_GPIO_DDRTJ_DDRTJ2;
	if(uartNo > 2)
	{  //��������ͨ���Ŵ���2������2������
	    uartNo = 2;
	}

	//ʹ��UART�ӿڹ���
	if(uartNo==0)
		MCF_GPIO_PUAPAR=MCF_GPIO_PUAPAR_UTXD0_UTXD0      
				   	   |MCF_GPIO_PUAPAR_URXD0_URXD0;
	else if(uartNo==1)
	    MCF_GPIO_PUBPAR=MCF_GPIO_PUBPAR_UTXD1_UTXD1
				       |MCF_GPIO_PUBPAR_URXD1_URXD1;
	else
			MCF_GPIO_PUCPAR=MCF_GPIO_PUCPAR_UTXD2_UTXD2
				       |MCF_GPIO_PUCPAR_URXD2_URXD2;
		
	//��λ�������ͷ������Լ�ģʽ�Ĵ���
	MCF_UART_UCR(uartNo) = MCF_UART_UCR_RESET_TX;  //��λ������
	MCF_UART_UCR(uartNo) = MCF_UART_UCR_RESET_RX;  //��λ������
	MCF_UART_UCR(uartNo) = MCF_UART_UCR_RESET_MR;  //��λģʽ�Ĵ���
	//����UARTģʽ:����żУ�顢8λ���ݡ�����ͨ��ģʽ��1ֹͣλ
	MCF_UART_UMR(uartNo) = (0
	                      | MCF_UART_UMR_PM_NONE
	                      | MCF_UART_UMR_BC_8 );
	MCF_UART_UMR(uartNo) = (0
	                      | MCF_UART_UMR_CM_NORMAL
	                      | MCF_UART_UMR_SB_STOP_BITS_1);
	                      
	//ѡ��Ԥ��Ƶ����ڲ�����ʱ����Ϊ�շ�����ʱ��Դ
	MCF_UART_UCSR(uartNo) = (0
				           |MCF_UART_UCSR_RCS_SYS_CLK
		  		           |MCF_UART_UCSR_TCS_SYS_CLK);

	//�������е�UART�ж�
	MCF_UART_UIMR(uartNo) = 0;
	//���㲨���ʲ�����:UBG = fsys/(������*32)
	ubgs = (uint16)(sysclk/(baud * 32));
	MCF_UART_UBG1(uartNo) = (uint8)((ubgs & 0xFF00) >> 8);
	MCF_UART_UBG2(uartNo) = (uint8) (ubgs & 0x00FF);

	//ʹ�ܽ������ͷ�����
	MCF_UART_UCR(uartNo) = (0
	                      | MCF_UART_UCR_RX_ENABLED
	                      | MCF_UART_UCR_TX_ENABLED);

	//�����Ƿ񿪷�UARTx�����ж�
	if(1 == intStatus)
	{  //�����н����ж�
	    MCF_UART_UIMR(uartNo) = 0x02;
		if(uartNo==0)
		{
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;       //ʹ�����ж�
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK13;    //ʹ��UART0�ж�
			MCF_INTC0_ICR13=MCF_INTC_ICR_IP(6)+MCF_INTC_ICR_IL(2);	 //�����ж����ȼ�
		}
		else if(uartNo==1)
		{
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;       //ʹ�����ж�
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK14;    //ʹ��UART1�ж�
			MCF_INTC0_ICR14=MCF_INTC_ICR_IP(5)+MCF_INTC_ICR_IL(2);  //�����ж����ȼ�	
		}
		else
		{
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;       //ʹ�����ж�
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK15;    //ʹ��UART2�ж�
			MCF_INTC0_ICR15=MCF_INTC_ICR_IP(4)+MCF_INTC_ICR_IL(2); //�����ж����ȼ�		
		}

	}
	else
	{  //�����н����ж�
	    MCF_UART_UIMR(uartNo) = 0x00;
		if(uartNo==0)
		{
			MCF_INTC0_IMRL|=MCF_INTC_IMRL_INT_MASK13;    //��ֹUART0�ж�
		}
		else if(uartNo==1)
		{
			MCF_INTC0_IMRL|=MCF_INTC_IMRL_INT_MASK14;    //��ֹUART1�ж�
		}
		else
		{
			MCF_INTC0_IMRL|=MCF_INTC_IMRL_INT_MASK15;    //��ֹUART2�ж�
		}
	}
}

/************************************************************/
/*                    UART����һ���ֽ�                      */
/*                uartNoΪ�������͵Ĵ��ڱ��                */
/*                   dataΪ��Ҫ���͵�����                   */
/************************************************************/
void UART_Send1byte(uint8 uartNo, uint8 data)
{
    if(uartNo > 2)
    {  //��������ͨ���Ŵ���2������2������
        uartNo = 2;
    }
    
    //�ȴ�FIFO���п��л���������
    while (!(MCF_UART_USR(uartNo) & MCF_UART_USR_TXRDY))
    {
    }
    //��������
    MCF_UART_UTB(uartNo) = data;
}

/************************************************************/
/*                    UART����һ���ֽ�                      */
/*                uartNoΪ�������յĴ��ڱ��                */
/************************************************************/
uint8 UART_Receive1byte(uint8 uartNo)
{
    if(uartNo > 2)
    {  //��������ͨ���Ŵ���2������2������
        uartNo = 2;
    }

    //�ȴ���������
    while (!(MCF_UART_USR(uartNo) & MCF_UART_USR_RXRDY)) 
    {
    }
    
    //���ؽ��յ�������
    return MCF_UART_URB(uartNo);	
}

/************************************************************/
/*                    UART�����ַ�������                    */
/*                uartNoΪ�������͵Ĵ��ڱ��                */
/*                pΪ��Ҫ���͵��ַ�����ָ��                 */
/************************************************************/
void UART_SendString(uint8 uartNo, char *p)
{
	while(*p!=0x00)       //�ж��ַ����Ƿ������
	{
		UART_Send1byte(uartNo,*p++); 
	}
}

/************************************************************/
/*                   UART0�жϽ��պ���                      */
/************************************************************/
__declspec(interrupt:0) void UART0_inter(void)//13
{
	uint8 temp;
	temp=UART_Receive1byte(0);
	//if((temp == 'A') || (str_flag == 1))
	//{
		//str_flag = 1;
		switch(temp)
		{
			case 0xff://����ͣ��
				{
					PIT0_stop();
					set_motor_highduty(0,0);
					while(1);	
				}
			
		}
		//if(temp = ';')
		//{
		//	str_flag = 0;
		//}
		/*
		if(temp != ';')
		{
			AT_cmd[i] = temp;
			i++;
		}
		else
		{
			AT_cmd[i] = 0;
			i = 0;
			UART_SendString(0,AT_cmd);
			str_flag = 0;
		}
		*/
	//}
}
/************************************************************/
/*                    UART��������ʾ��������                */
/*                uartNoΪ�������͵Ĵ��ڱ��                */
/*                dataΪҪ���͵�������ֵ                    */
/************************************************************/
void UART_Sendgraph(uint8 uartNO,uint8 channel,int data)
{
	UART_Send1byte(uartNO,0xA0);//֡ͷ
	UART_Send1byte(uartNO,channel);//���������ͨ��
	UART_Send1byte(uartNO,6);
	UART_Send1byte(uartNO,data>>8);
	UART_Send1byte(uartNO,data);
	UART_Send1byte(uartNO,0xFF);//֡β
}
void UART_Sendint(uint8 uartNO,int num)
{
	char str[5];
	sprintf(str,"%d",num);
	UART_SendString(uartNO,str);
}