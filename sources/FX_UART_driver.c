#include "main.h"
char AT_cmd[10];
char i = 0;
char str_flag = 0;
/************************************************************/
/*                    ³õÊ¼»¯UART´®¿Ú                        */
/*                uartNoÎªÐèÒªÉèÖÃµÄ´®¿Ú±àºÅ                */
/*                   sysclkÎªÏµÍ³Ê±ÖÓÆµÂÊ                   */
/*                baudÎªÐèÒªÉèÖÃµÄ´®¿ÚµÄ²¨ÌØÂÊ              */
/*                intStatus: Îª³õÊ¼»¯ÖÐ¶ÏµÄ×´Ì¬£JY-MC¬           */
/*                1±íÊ¾¿ªÖÐ¶Ï ÆäËü±íÊ¾¹ØÖÐ¶Ï                */
/************************************************************/
void UART_INIT(uint8 uartNo, int32 sysclk, int32 baud, uint8 intStatus)
{
	register uint16 ubgs;
	MCF_GPIO_PTJPAR|=MCF_GPIO_PTJPAR_FEC_TXD0_GPIO;
	MCF_GPIO_DDRTJ&=~MCF_GPIO_DDRTJ_DDRTJ2;
	if(uartNo > 2)
	{  //Èô´«½øµÄÍ¨µÀºÅ´óÓÚ2£¬Ôò°´ÕÕ2À´ÉèÖÃ
	    uartNo = 2;
	}

	//Ê¹ÄÜUART½Ó¿Ú¹¦ÄÜ
	if(uartNo==0)
		MCF_GPIO_PUAPAR=MCF_GPIO_PUAPAR_UTXD0_UTXD0      
				   	   |MCF_GPIO_PUAPAR_URXD0_URXD0;
	else if(uartNo==1)
	    MCF_GPIO_PUBPAR=MCF_GPIO_PUBPAR_UTXD1_UTXD1
				       |MCF_GPIO_PUBPAR_URXD1_URXD1;
	else
			MCF_GPIO_PUCPAR=MCF_GPIO_PUCPAR_UTXD2_UTXD2
				       |MCF_GPIO_PUCPAR_URXD2_URXD2;
		
	//¸´Î»½ÓÊÕÆ÷ºÍ·¢ËÍÆ÷ÒÔ¼°Ä£Ê½¼Ä´æÆ÷
	MCF_UART_UCR(uartNo) = MCF_UART_UCR_RESET_TX;  //¸´Î»·¢ËÍÆ÷
	MCF_UART_UCR(uartNo) = MCF_UART_UCR_RESET_RX;  //¸´Î»½ÓÊÕÆ÷
	MCF_UART_UCR(uartNo) = MCF_UART_UCR_RESET_MR;  //¸´Î»Ä£Ê½¼Ä´æÆ÷
	//ÅäÖÃUARTÄ£Ê½:ÎÞÆæÅ¼Ð£Ñé¡¢8Î»Êý¾Ý¡¢Õý³£Í¨µÀÄ£Ê½¡¢1Í£Ö¹Î»
	MCF_UART_UMR(uartNo) = (0
	                      | MCF_UART_UMR_PM_NONE
	                      | MCF_UART_UMR_BC_8 );
	MCF_UART_UMR(uartNo) = (0
	                      | MCF_UART_UMR_CM_NORMAL
	                      | MCF_UART_UMR_SB_STOP_BITS_1);
	                      
	//Ñ¡ÔñÔ¤·ÖÆµºóµÄÄÚ²¿×ÜÏßÊ±ÖÓ×÷ÎªÊÕ·¢Æ÷µÄÊ±ÖÓÔ´
	MCF_UART_UCSR(uartNo) = (0
				           |MCF_UART_UCSR_RCS_SYS_CLK
		  		           |MCF_UART_UCSR_TCS_SYS_CLK);

	//ÆÁ±ÎËùÓÐµÄUARTÖÐ¶Ï
	MCF_UART_UIMR(uartNo) = 0;
	//¼ÆËã²¨ÌØÂÊ²¢ÉèÖÃ:UBG = fsys/(²¨ÌØÂÊ*32)
	ubgs = (uint16)(sysclk/(baud * 32));
	MCF_UART_UBG1(uartNo) = (uint8)((ubgs & 0xFF00) >> 8);
	MCF_UART_UBG2(uartNo) = (uint8) (ubgs & 0x00FF);

	//Ê¹ÄÜ½ÓÊÕÆ÷ºÍ·¢ËÍÆ÷
	MCF_UART_UCR(uartNo) = (0
	                      | MCF_UART_UCR_RX_ENABLED
	                      | MCF_UART_UCR_TX_ENABLED);

	//¾ö¶¨ÊÇ·ñ¿ª·ÅUARTx½ÓÊÕÖÐ¶Ï
	if(1 == intStatus)
	{  //¿ª´®ÐÐ½ÓÊÕÖÐ¶Ï
	    MCF_UART_UIMR(uartNo) = 0x02;
		if(uartNo==0)
		{
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;       //Ê¹ÄÜ×ÜÖÐ¶Ï
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK13;    //Ê¹ÄÜUART0ÖÐ¶Ï
			MCF_INTC0_ICR13=MCF_INTC_ICR_IP(6)+MCF_INTC_ICR_IL(2);	 //ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶
		}
		else if(uartNo==1)
		{
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;       //Ê¹ÄÜ×ÜÖÐ¶Ï
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK14;    //Ê¹ÄÜUART1ÖÐ¶Ï
			MCF_INTC0_ICR14=MCF_INTC_ICR_IP(5)+MCF_INTC_ICR_IL(2);  //ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶	
		}
		else
		{
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;       //Ê¹ÄÜ×ÜÖÐ¶Ï
			MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK15;    //Ê¹ÄÜUART2ÖÐ¶Ï
			MCF_INTC0_ICR15=MCF_INTC_ICR_IP(4)+MCF_INTC_ICR_IL(2); //ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶		
		}

	}
	else
	{  //½û´®ÐÐ½ÓÊÕÖÐ¶Ï
	    MCF_UART_UIMR(uartNo) = 0x00;
		if(uartNo==0)
		{
			MCF_INTC0_IMRL|=MCF_INTC_IMRL_INT_MASK13;    //½ûÖ¹UART0ÖÐ¶Ï
		}
		else if(uartNo==1)
		{
			MCF_INTC0_IMRL|=MCF_INTC_IMRL_INT_MASK14;    //½ûÖ¹UART1ÖÐ¶Ï
		}
		else
		{
			MCF_INTC0_IMRL|=MCF_INTC_IMRL_INT_MASK15;    //½ûÖ¹UART2ÖÐ¶Ï
		}
	}
}

/************************************************************/
/*                    UART·¢ËÍÒ»¸ö×Ö½Ú                      */
/*                uartNoÎªÓÃÀ´·¢ËÍµÄ´®¿Ú±àºÅ                */
/*                   dataÎªÐèÒª·¢ËÍµÄÊý¾Ý                   */
/************************************************************/
void UART_Send1byte(uint8 uartNo, uint8 data)
{
    if(uartNo > 2)
    {  //Èô´«½øµÄÍ¨µÀºÅ´óÓÚ2£¬Ôò°´ÕÕ2À´·¢ËÍ
        uartNo = 2;
    }
    
    //µÈ´ýFIFOÖÐÓÐ¿ÕÏÐ»º³åÇø¿ÉÓÃ
    while (!(MCF_UART_USR(uartNo) & MCF_UART_USR_TXRDY))
    {
    }
    //·¢ËÍÊý¾Ý
    MCF_UART_UTB(uartNo) = data;
}

/************************************************************/
/*                    UART½ÓÊÕÒ»¸ö×Ö½Ú                      */
/*                uartNoÎªÓÃÀ´½ÓÊÕµÄ´®¿Ú±àºÅ                */
/************************************************************/
uint8 UART_Receive1byte(uint8 uartNo)
{
    if(uartNo > 2)
    {  //Èô´«½øµÄÍ¨µÀºÅ´óÓÚ2£¬Ôò°´ÕÕ2À´½ÓÊÕ
        uartNo = 2;
    }

    //µÈ´ý½ÓÊÕÊý¾Ý
    while (!(MCF_UART_USR(uartNo) & MCF_UART_USR_RXRDY)) 
    {
    }
    
    //·µ»Ø½ÓÊÕµ½µÄÊý¾Ý
    return MCF_UART_URB(uartNo);	
}

/************************************************************/
/*                    UART·¢ËÍ×Ö·û´®º¯Êý                    */
/*                uartNoÎªÓÃÀ´·¢ËÍµÄ´®¿Ú±àºÅ                */
/*                pÎªÐèÒª·¢ËÍµÄ×Ö·û´®µÄÖ¸Õë                 */
/************************************************************/
void UART_SendString(uint8 uartNo, char *p)
{
	while(*p!=0x00)       //ÅÐ¶Ï×Ö·û´®ÊÇ·ñ·¢ËÍÍê±Ï
	{
		UART_Send1byte(uartNo,*p++); 
	}
}

/************************************************************/
/*                   UART0ÖÐ¶Ï½ÓÊÕº¯Êý                      */
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
			case 0xff://½ô¼±Í£³µ
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
/*                    UART·¢ËÍÐéÄâÊ¾²¨Æ÷º¯Êý                */
/*                uartNoÎªÓÃÀ´·¢ËÍµÄ´®¿Ú±àºÅ                */
/*                dataÎªÒª·¢ËÍµÄÕûÐÍÊýÖµ                    */
/************************************************************/
void UART_Sendgraph(uint8 uartNO,uint8 channel,int data)
{
	UART_Send1byte(uartNO,0xA0);//Ö¡Í·
	UART_Send1byte(uartNO,channel);//¹¦ÄÜÂë¾ö¶¨Í¨µÀ
	UART_Send1byte(uartNO,6);
	UART_Send1byte(uartNO,data>>8);
	UART_Send1byte(uartNO,data);
	UART_Send1byte(uartNO,0xFF);//Ö¡Î²
}
void UART_Sendint(uint8 uartNO,int num)
{
	char str[5];
	sprintf(str,"%d",num);
	UART_SendString(uartNO,str);
}