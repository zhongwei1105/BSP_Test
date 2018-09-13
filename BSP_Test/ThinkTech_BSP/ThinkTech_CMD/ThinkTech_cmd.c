/****************************************************************************************
**  Filename :  ThinkTech_cmd.c
**  Abstract :  This file implements key function.
**  By       :  钟伟
**  Date     :  2018-09-6
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "ThinkTech_cmd.h"  
#include "Driver_USART.h"
#include "system_M100.h"
#include "ThinkTech_ESP8266.h" 
#include "ThinkTech_MCP2515.h"
#include "ThinkTech_OBD.h"
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
#define cmd_num 	14

char *cmd_tab[cmd_num]=
{
	"AT+SSID=\"\"",
	"AT+Password=\"\"",
	"AT+ServerIP=\"\"",
	"AT+PortNum=\"\"",
	
	"AT+ISO15765-4STD_500K",
	"AT+ISO15765-4EXT_500K",
	"AT+ISO15765-4STD_250K",
	"AT+ISO15765-4EXT_250K",
	"AT+DS",
	"AT+STOPDS",	 
	"AT+CDTC",
	"AT+DTC",
	"AT+VIN",
	"AT+EXIT",
};	
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
/*****************************************************************************************
* Function Name : CMD_Drv
* Description   : 串口交互函数
* Arguments     : NONE
* Return Value	: NONE
******************************************************************************************/
void CMD_Drv(void)
{
	uint16_t i,len;
	
	if(USART_RX_STA&0X8000)	
	{
//////////////////////////////////////以下为系统命令，不可更改//////////////////////////////////////////////		
		if((USART_Check_Cmd("AT+HELP")))
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();		
			
			printf("CMD List:\r\n");				
			for(i=0; i<cmd_num; i++)
			printf("%s\r\n",cmd_tab[i]);
		}
		else if((USART_Check_Cmd("AT+SSID=""")))
		{
			len = USART_RX_STA&0X3FFF;
			if(len>(9+31))
			{
				printf("EEROR\r\n");
			}
			else
			{
				for(i=0; i<31; i++)
				{
					Wif_ssid[i] = 0;
				}
				
				for(i=0; i<(len-9); i++)
				{
					Wif_ssid[i] = USART_RX_BUF[i+8];
				}

				printf("OK\r\n");
			}
					
			USART_RX_STA = 0;
			Clean_RX_BUF();									
		}
		else if((USART_Check_Cmd("AT+Password=""")))
		{
			len = USART_RX_STA&0X3FFF;
			if(len>(13+31))
			{
				printf("EEROR\r\n");
			}
			else
			{
				for(i=0; i<31; i++)
				{
					Wif_password[i] = 0;
				}
				
				for(i=0; i<(len-13); i++)
				{
					Wif_password[i] = USART_RX_BUF[i+12];
				}

				printf("OK\r\n");
			}
					
			USART_RX_STA = 0;
			Clean_RX_BUF();									
		}
		else if((USART_Check_Cmd("AT+ServerIP=""")))
		{
			len = USART_RX_STA&0X3FFF;
			if(len>(13+15))
			{
				printf("EEROR\r\n");
			}
			else
			{
				for(i=0; i<15; i++)
				{
					ServerIP[i] = 0;
				}
				
				for(i=0; i<(len-13); i++)
				{
					ServerIP[i] = USART_RX_BUF[i+12];
				}

				printf("OK\r\n");
			}
					
			USART_RX_STA = 0;
			Clean_RX_BUF();									
		}
		else if((USART_Check_Cmd("AT+PortNum=""")))
		{
			len = USART_RX_STA&0X3FFF;
			if(len>(12+15))
			{
				printf("EEROR\r\n");
			}
			else
			{
				for(i=0; i<15; i++)
				{
					PortNum[i] = 0;
				}
				
				for(i=0; i<(len-12); i++)
				{
					PortNum[i] = USART_RX_BUF[i+11];
				}

				printf("OK\r\n");
			}
					
			USART_RX_STA = 0;
			Clean_RX_BUF();									
		}	
//////////////////////////////////////以上为系统命令，不可更改//////////////////////////////////////////////
		
//////////////////////////////////////以下为自定义命令//////////////////////////////////////////////////////	
		else if(USART_Check_Cmd("AT+ISO15765-4STD_500K"))
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();				
			printf("OK\r\n");
			
			if(CotrolVale == SYSOUT)
			{
				ISO15765_4_Config(Std_ID, CAN_500Kbps);
				CotrolVale = SYSCFG;
			}
			else
			{
				printf("ERROR-002\r\n");
			}
		}	
		else if(USART_Check_Cmd("AT+ISO15765-4EXT_500K"))
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();				
			printf("OK\r\n");
			
			if(CotrolVale == SYSOUT)
			{
				ISO15765_4_Config(Ext_ID, CAN_500Kbps);
				CotrolVale = SYSCFG;
			}
			else
			{
				printf("ERROR-002\r\n");
			}
		}
		else if(USART_Check_Cmd("AT+ISO15765-4STD_250K"))
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();				
			printf("OK\r\n");
			
			if(CotrolVale == SYSOUT)
			{
				ISO15765_4_Config(Std_ID, CAN_250Kbps);
				CotrolVale = SYSCFG;
			}
			else
			{
				printf("ERROR-002\r\n");
			}
		}	
		else if(USART_Check_Cmd("AT+ISO15765-4EXT_250K"))
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();				
			printf("OK\r\n");
			
			if(CotrolVale == SYSOUT)
			{
				ISO15765_4_Config(Ext_ID, CAN_250Kbps);
				CotrolVale = SYSCFG;
			}
			else
			{
				printf("ERROR-002\r\n");
			}
		}
		else if(USART_Check_Cmd("AT+DS"))
		{
			printf("OK\r\n");	
			
			if(CotrolVale != SYSOUT)
			{
				len = USART_RX_STA&0X3FFF;
				for(i=0; i<len; i++)
				{
					ATCmd[i] = USART_RX_BUF[i];
				}	
				CotrolVale = SYSDS;		
			}
			else
			{
				printf("ERROR-002\r\n");
			}
			
			USART_RX_STA = 0;
			Clean_RX_BUF();							
		}
		else if(USART_Check_Cmd("AT+STOPDS"))
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();			
			printf("OK\r\n");	
			
			if(CotrolVale != SYSOUT)
				CotrolVale = SYSSTOPDS;	
			else
				printf("ERROR-002\r\n");
		}
		else if(USART_Check_Cmd("AT+CDTC"))
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();			
			printf("OK\r\n");	

			if(CotrolVale != SYSOUT)
				CotrolVale = SYSCDTC;	
			else
				printf("ERROR-002\r\n");	
		}
		else if(USART_Check_Cmd("AT+DTC"))	
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();			
			printf("OK\r\n");	
			
			if(CotrolVale != SYSOUT)
				CotrolVale = SYSDTC;	
			else
				printf("ERROR-002\r\n");	
		}
		else if(USART_Check_Cmd("AT+VIN"))	
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();			
			printf("OK\r\n");	
			
			if(CotrolVale != SYSOUT)
			{
				CotrolVale = SYSVIN;	
			}
			else
				printf("ERROR-002\r\n");	
		}
		else if(USART_Check_Cmd("AT+EXIT"))	
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();			
			printf("OK\r\n");	
			
			if(CotrolVale != SYSOUT)
			{
				CotrolVale = SYSOUT;	
				printf("EXIT\r\n");	
			}
			else
				printf("ERROR-002\r\n");	
		}
//////////////////////////////////////以上为自定义命令//////////////////////////////////////////////////////
		else
		{
			USART_RX_STA = 0;
			Clean_RX_BUF();	
			
			printf("ERROR-001\r\n");
		}
	}
}
/***********************************END OF FILE*******************************************/





