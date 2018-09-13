/****************************************************************************************
**  Filename :  ThinkTech_ESP8266.c
**  Abstract :  This file implements led function.
**  By       :  钟伟
**  Date     :  2018-08-13 
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "Driver_USART.h"
#include "ThinkTech_ESP8266.h" 
#include "system_M100.h"
/*****************************************************************************************
								Variables definitions
*****************************************************************************************/
uint8_t PortNum[5] = {"8080"}; 					//服务器端口
uint8_t ServerIP[15] = {"172.25.40.1"};			//服务器IP
uint8_t Wif_ssid[31] = {"LuoShuang"};			//Wifi账号
uint8_t Wif_password[31] = {"sunrise123"}; 		//Wifi密码

extern ARM_DRIVER_USART 			Driver_USART0;
static ARM_DRIVER_USART 			* ESP8266_USARTdrv = &Driver_USART0;
TS_ESP8266_RESOURCES				s_ESP8266;
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
/*****************************************************************************************
* Function Name: ESP8266_UART_callback
* Description  : 串口中断回调函数
* Arguments    : e：中断事件
* Return Value : NONE
******************************************************************************************/
void ESP8266_UART_callback(uint32_t e)
{
	if (e & ARM_USART_EVENT_SEND_COMPLETE) 
	{
		// set tx complete flag
		s_ESP8266.Tx_CompleteF = true;
	}
	
	if (e & ARM_USART_EVENT_RECEIVE_COMPLETE) 
	{
		// set rx complete flag
		s_ESP8266.Rx_CompleteF = true;
		
		if((Get_ESP8266_Binding_Status() == false) && (Get_ESP8266_Conenect_Status() == true))
		{
			s_ESP8266.Demand_Cnt = 0;
		}
		
		if((s_ESP8266.RX_STA&0X8000) == 0)			
		{ 	
			if((Get_ESP8266_Binding_Status() == false) && (Get_ESP8266_Conenect_Status() == true))
			{
				Frame_Cnt = 0;
				if(s_ESP8266.RX_STA == 0)
				{
					s_ESP8266.OpenFrameF = true;
				}
			}
			
			s_ESP8266.FrameCnt = 0;
			s_ESP8266.RX_BUF[s_ESP8266.RX_STA++] = s_ESP8266.RX_TEMP_BUF[0];			 
		}
		
		ESP8266_USARTdrv->Receive(s_ESP8266.RX_TEMP_BUF, 1);
	}
	
	if (e & ARM_USART_EVENT_RX_TIMEOUT) 
	{
		// set timeout flag

	}
		
	if (e & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW)) 
	{
		// set overflow flag

	}
}
/*****************************************************************************************
* Function Name: ESP8266_Usart_Init
* Description  : 串口初始化
* Arguments    : BAUD_RATE：波特率
* Return Value : NONE
******************************************************************************************/
void ESP8266_Usart_Init(uint32_t BAUD_RATE)
{
	// Initialize the USART driver
	ESP8266_USARTdrv->Initialize(ESP8266_UART_callback);
	
	// Power up the USART peripheral
	ESP8266_USARTdrv->PowerControl(ARM_POWER_FULL);

	ESP8266_USARTdrv->Control(	ARM_USART_DATA_BITS_8       |
								ARM_USART_PARITY_NONE       |
								ARM_USART_STOP_BITS_1       |
								ARM_USART_FLOW_CONTROL_NONE,
								BAUD_RATE);
	
	ESP8266_USARTdrv->Receive(s_ESP8266.RX_TEMP_BUF, 1);
}
/*****************************************************************************************
* Function Name: ESP8266_Usart_printf
* Description  : 串口发送多个字符串，在回调中查询发送完成标志
* Arguments    : fmt,... :字符串内容
* Return Value : NONE
******************************************************************************************/
void ESP8266_Usart_printf(char* fmt,...)
{
	uint16_t i; 
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)s_ESP8266.TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)s_ESP8266.TX_BUF);		
	ESP8266_USARTdrv->Send(s_ESP8266.TX_BUF, i);
}
/*****************************************************************************************
* Function Name: ESP8266_Usart_Send
* Description  : 串口发送，在回调中查询发送完成标志
* Arguments    : *pBuf：需要发送的数据指针
                 len：需要发送的数据长度
* Return Value : NONE
******************************************************************************************/
void ESP8266_Usart_Send(uint8_t *pBuf, uint16_t len)
{
	ESP8266_USARTdrv->Send(pBuf, len);
}
/*****************************************************************************************
* Function Name: ESP8266_Usart_Receive
* Description  : 串口接收，在回调中查询接收完成标志
* Arguments    : *pBuf：需要接收的数据指针
                 len：需要接收的数据长度
* Return Value : NONE
******************************************************************************************/
void ESP8266_Usart_Receive(uint8_t *pBuf, uint16_t len)
{
	ESP8266_USARTdrv->Receive(pBuf, len);
}
/*****************************************************************************************
* Function Name: ESP8266_Check_Cmd
* Description  : 接收字符串与期待的字符串比较
* Arguments    : *str：需要对比的字符串
* Return Value : 0：没有得到期待的应答结果
                 其他：期待应答结果的位置（str起始位置）
******************************************************************************************/
uint8_t* ESP8266_Check_Cmd(char *str)
{
	char *strx=0;
	
	if(s_ESP8266.RX_STA&0X8000)				
	{ 
		s_ESP8266.RX_BUF[s_ESP8266.RX_STA&0X7FFF]=0;	
		strx=strstr((const char*)s_ESP8266.RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}
/*****************************************************************************************
* Function Name: ESP8266_Send_Cmd
* Description  : 发送命令字符串
* Arguments    : *cmd：命令字符串
				 *ack：期待应答的结果，如果为空，则表示不需要应答
                 waittime：等待时间（单位：10ms）
* Return Value : true：发送使能
				 false：当前正处于发送或接收中
******************************************************************************************/
uint8_t ESP8266_Send_Cmd(char *cmd, char *ack, uint16_t waittime)
{
	if(s_ESP8266.e_Usart_Steup == Out)	
	{
		s_ESP8266.e_Usart_Steup = StartSend;
		
		s_ESP8266.Cmd = cmd;
		s_ESP8266.Ack = ack;
		s_ESP8266.WaitTime = waittime;
		
		return true;
	}
	else
	{
		return false;
	}
}
/*****************************************************************************************
* Function Name: ESP8266_Drv
* Description  : ESP8266控制进程，需放在10ms时间片中
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void ESP8266_Drv(void)
{
	if((Get_ESP8266_Binding_Status() == false) && (Get_ESP8266_Conenect_Status() == true))
	{
		s_ESP8266.Demand_Cnt++;
		if(s_ESP8266.Demand_Cnt == Demand_Set_Cnt)
		{
			s_ESP8266.Demand_Cnt = 0;
			//s_ESP8266.e_Demand_Steup = Demand_Steup0;
		}
	}
	ESP8266_Demand();
	
	if(s_ESP8266.Wifi_BindingF == true)
	{
		ESP8266_STA_Config();
		
		s_ESP8266.Wifi_BindingCnt++;
		if(s_ESP8266.Wifi_BindingCnt == Binding_Set_Cnt)
		{
			s_ESP8266.Wifi_BindingCnt = 0;
			//s_ESP8266.Wifi_BindingF = false;
		}
	}
	
	switch(s_ESP8266.e_Usart_Steup)
	{
		case StartSend:
			{
				s_ESP8266.Tx_CompleteF = false;	
				s_ESP8266.Rx_CompleteF = false;	
				s_ESP8266.RX_STA = 0;
				s_ESP8266.FrameCnt = 0;
				
				ESP8266_Usart_printf("%s\r\n", s_ESP8266.Cmd);
				
				s_ESP8266.e_Usart_Status = Sending;
				s_ESP8266.e_Usart_Steup = WaitSend;
			}
		break;
			
		case WaitSend:
			{
				if(s_ESP8266.Tx_CompleteF != true)	
				{
					s_ESP8266.WaitTime -= 1;
					if(s_ESP8266.WaitTime == 0)
					{
						s_ESP8266.e_Usart_Status = SendError;
						s_ESP8266.e_Usart_Steup = Out;
					}
				}
				else
				{
					s_ESP8266.e_Usart_Status = SendFinish;
					s_ESP8266.e_Usart_Steup = WaitReceive;
				}
			}
		break;			
			
		case WaitReceive:
			{
				if(s_ESP8266.Ack)	
				{
					s_ESP8266.e_Usart_Status = Receiveing;
					
					if((s_ESP8266.RX_STA&0X8000) == 0)		
					{
						s_ESP8266.WaitTime -= 1;
						if(s_ESP8266.WaitTime == 0)
						{
							s_ESP8266.e_Usart_Steup = CheckReceive;
						}
						
//						if(s_ESP8266.RX_STA != 0)		//收到数据后，才开启段帧计时
//						{
//							s_ESP8266.FrameCnt++;
//							if(s_ESP8266.FrameCnt == 10)
//							{
//								s_ESP8266.FrameCnt = 0;
//								s_ESP8266.RX_STA |= 1<<15;							
//							}
//						}
					}
					else
					{				
						s_ESP8266.e_Usart_Steup = CheckReceive;
					}
				}
				else
				{
					s_ESP8266.e_Usart_Steup = Out;
				}
			}
		break;	
			
		case CheckReceive:			
			{
				s_ESP8266.RX_STA |= 1<<15;
				
				if(ESP8266_Check_Cmd(s_ESP8266.Ack))
				{
					s_ESP8266.e_Usart_Status = ReceiveCheckOk;
					
					#if Debug_printf
					printf("ack:%s\r\n",(uint8_t*)s_ESP8266.Ack);
					#endif
				}
				else
				{
					s_ESP8266.e_Usart_Status = ReceiveCheckError;
				}
				
				s_ESP8266.e_Usart_Steup = Out;
			}
		break;
			
		case Out:			
			break;		
			
		default: 
			break;
	}
}
/*****************************************************************************************
* Function Name: ESP8266_STA_Config
* Description  : ESP8266 STA TCP连接配置、连接流程
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void ESP8266_STA_Config(void)
{
	static uint16_t STA_TiemCnt;
	static char p[64];
	uint8_t s;
	
	switch(s_ESP8266.e_STA_Steup)
	{
		case STA_Steup0:
			{
				//退出透传
				s_ESP8266.TX_BUF[0] = '+';
				s_ESP8266.Tx_CompleteF = false;	
				ESP8266_Usart_Send(s_ESP8266.TX_BUF, 1);
				STA_TiemCnt = 0;
				s_ESP8266.e_STA_Steup = STA_Steup1;
			}
		break;
			
		case STA_Steup1:
			{
				if(s_ESP8266.Tx_CompleteF == true)	
				s_ESP8266.e_STA_Steup = STA_Steup2;
			}
		break;				
		
		case STA_Steup2:
			{
				STA_TiemCnt++;
				if(STA_TiemCnt == 1)
				{
					//退出透传
					s_ESP8266.TX_BUF[0] = '+';
					s_ESP8266.Tx_CompleteF = false;	
					ESP8266_Usart_Send(s_ESP8266.TX_BUF, 1);
					STA_TiemCnt = 0;
					s_ESP8266.e_STA_Steup = STA_Steup3;
				}
			}
		break;	

		case STA_Steup3:
			{
				if(s_ESP8266.Tx_CompleteF == true)	
				s_ESP8266.e_STA_Steup = STA_Steup4;
			}
		break;	

		case STA_Steup4:
			{
				STA_TiemCnt++;
				if(STA_TiemCnt == 1)
				{
					//退出透传
					s_ESP8266.TX_BUF[0] = '+';
					s_ESP8266.Tx_CompleteF = false;	
					ESP8266_Usart_Send(s_ESP8266.TX_BUF, 1);
					STA_TiemCnt = 0;
					s_ESP8266.e_STA_Steup = STA_Steup5;
				}
			}
		break;		

		case STA_Steup5:
			{
				STA_TiemCnt++;
				if(STA_TiemCnt == 50)	
				{
					STA_TiemCnt = 0;
					s = ESP8266_Send_Cmd("AT", "OK", 20);						//确定退出透传							
					if(s == true)
					s_ESP8266.e_STA_Steup = STA_Steup6;
				}
			}	
		break;			

		case STA_Steup6:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s = ESP8266_Send_Cmd("AT+CIPMODE=0", "OK", 50);				//关闭透传模式
					if(s == true)
					s_ESP8266.e_STA_Steup = STA_Steup7;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)
					s_ESP8266.e_STA_Steup = STA_Steup0;
			}
		break;	

		case STA_Steup7:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s = ESP8266_Send_Cmd("AT+CWMODE=1","OK",50);				//设置STA模式
					if(s == true)
					s_ESP8266.e_STA_Steup = STA_Steup8;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)
					s_ESP8266.e_STA_Steup = STA_Steup0;
			}
		break;			
			
		case STA_Steup8:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s = ESP8266_Send_Cmd("AT+RST","OK",20);						//复位
					if(s == true)
					s_ESP8266.e_STA_Steup = STA_Steup9;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)
					s_ESP8266.e_STA_Steup = STA_Steup0;
			}
		break;	
			
		case STA_Steup9:
			{
				STA_TiemCnt++;
				if(STA_TiemCnt == 200)											//等待2S复位
				{
					STA_TiemCnt = 0;
								
					sprintf(p,"AT+CWJAP=\"%s\",\"%s\"",Wif_ssid,Wif_password);
					#if Debug_printf
					printf("%s\r\n",p);
					#endif
					
					s = ESP8266_Send_Cmd(p,"WIFI GOT IP",300);					//连接目标路由器
					if(s == true)
					s_ESP8266.e_STA_Steup = STA_Steup10;
				}
			}
		break;	
			
		case STA_Steup10:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s_ESP8266.e_STA_Steup = STA_Steup11;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)	
				{
					sprintf(p,"AT+CWJAP=\"%s\",\"%s\"",Wif_ssid,Wif_password);
					s = ESP8266_Send_Cmd(p,"WIFI GOT IP",300);					//连接目标路由器
					if(s == true)
					{
						#if Debug_printf
						printf("retry cmd0\r\n");
						#endif
					}
				}
			}
		break;						
			
		case STA_Steup11:
			{
				sprintf(p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ServerIP,PortNum);
				#if Debug_printf
				printf("%s\r\n",p);
				#endif
				
				s = ESP8266_Send_Cmd(p,"CONNECT",200);							//连接目标服务器
				if(s == true)
				s_ESP8266.e_STA_Steup = STA_Steup12;
			}
		break;		
			
		case STA_Steup12:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s_ESP8266.e_STA_Steup = STA_Steup13;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)	
				{
					sprintf(p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ServerIP,PortNum);
					s = ESP8266_Send_Cmd(p,"CONNECT",200);						//连接目标服务器
					if(s == true)
					{
						#if Debug_printf
						printf("retry cmd1\r\n");
						#endif
					}
				}
			}
		break;	
			
		case STA_Steup13:
			{
				s = ESP8266_Send_Cmd("AT+CIPMODE=1","OK",100);					//设置为透传模式
				if(s == true)
				s_ESP8266.e_STA_Steup = STA_Steup14;
			}
		break;				
			
		case STA_Steup14:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s_ESP8266.e_STA_Steup = STA_Steup15;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)	
				{
					s = ESP8266_Send_Cmd("AT+CIPMODE=1","OK",100);				//设置为透传模式
					if(s == true)
					{
						#if Debug_printf
						printf("retry cmd2\r\n");
						#endif
					}
				}
			}
		break;	

		case STA_Steup15:
			{
				s = ESP8266_Send_Cmd("AT+CIPSEND","OK",50);						//开始透传
				if(s == true)
				s_ESP8266.e_STA_Steup = STA_Steup16;
			}
		break;				

		case STA_Steup16:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s_ESP8266.e_STA_Steup = STA_Steup17;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)	
				{
					s = ESP8266_Send_Cmd("AT+CIPSEND","OK",50);					//开始透传
					if(s == true)
					{
						#if Debug_printf
						printf("retry cmd3\r\n");
						#endif
					}
				}
			}
		break;			
			
		case STA_Steup17:
			{
				s_ESP8266.Wifi_BindingF = false;
				s_ESP8266.Wifi_ConenectF = true;
				
				s_ESP8266.e_STA_Steup = STA_Out;
			}
		break;	

		case STA_Out:
			break; 

		default: 
			break; 
	}
}
/*****************************************************************************************
* Function Name: ESP8266_Demand
* Description  : 查询连接状态，未连接则重新绑定
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void ESP8266_Demand(void)
{
	static uint16_t Demand_TiemCnt;
	uint8_t s;
	
	switch(s_ESP8266.e_Demand_Steup)
	{
		case Demand_Steup0:
			{
				//退出透传
				s_ESP8266.TX_BUF[0] = '+';
				s_ESP8266.Tx_CompleteF = false;	
				ESP8266_Usart_Send(s_ESP8266.TX_BUF, 1);
				Demand_TiemCnt = 0;
				s_ESP8266.e_Demand_Steup = Demand_Steup1;
			}
		break;
			
		case Demand_Steup1:
			{
				if(s_ESP8266.Tx_CompleteF == true)	
				s_ESP8266.e_Demand_Steup = Demand_Steup2;
			}
		break;				
		
		case Demand_Steup2:
			{
				Demand_TiemCnt++;
				if(Demand_TiemCnt == 1)
				{
					//退出透传
					s_ESP8266.TX_BUF[0] = '+';
					s_ESP8266.Tx_CompleteF = false;	
					ESP8266_Usart_Send(s_ESP8266.TX_BUF, 1);
					Demand_TiemCnt = 0;
					s_ESP8266.e_Demand_Steup = Demand_Steup3;
				}
			}
		break;	

		case Demand_Steup3:
			{
				if(s_ESP8266.Tx_CompleteF == true)	
				s_ESP8266.e_Demand_Steup = Demand_Steup4;
			}
		break;	

		case Demand_Steup4:
			{
				Demand_TiemCnt++;
				if(Demand_TiemCnt == 1)
				{
					//退出透传
					s_ESP8266.TX_BUF[0] = '+';
					s_ESP8266.Tx_CompleteF = false;	
					ESP8266_Usart_Send(s_ESP8266.TX_BUF, 1);
					Demand_TiemCnt = 0;
					s_ESP8266.e_Demand_Steup = Demand_Steup5;
				}
			}
		break;		

		case Demand_Steup5:
			{
				Demand_TiemCnt++;
				if(Demand_TiemCnt == 50)	
				{
					Demand_TiemCnt = 0;
					s = ESP8266_Send_Cmd("AT", "OK", 20);						//确定退出透传							
					if(s == true)
					s_ESP8266.e_Demand_Steup = Demand_Steup6;
				}
			}	
		break;			

		case Demand_Steup6:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s = ESP8266_Send_Cmd("AT+CIPMODE=0", "OK", 50);				//关闭透传模式
					if(s == true)
					s_ESP8266.e_Demand_Steup = Demand_Steup7;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)
					s_ESP8266.e_Demand_Steup = Demand_Steup0;
			}
		break;	

		case Demand_Steup7:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s = ESP8266_Send_Cmd("AT+CIPSTATUS",":3",50);				//查询连接状态
					if(s == true)
					s_ESP8266.e_Demand_Steup = Demand_Steup8;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)
					s_ESP8266.e_Demand_Steup = Demand_Steup0;
			}
		break;	

		case Demand_Steup8:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s = ESP8266_Send_Cmd("AT+CIPMODE=1","OK",100);				//设置为透传模式
					if(s == true)
					s_ESP8266.e_Demand_Steup = Demand_Steup9;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)
				{
					ESP8266_Binding_Enable();
					s_ESP8266.e_Demand_Steup = Demand_Out;
				}
			}
		break;		

		case Demand_Steup9:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s = ESP8266_Send_Cmd("AT+CIPSEND","OK",50);					//开始透传
					if(s == true)
					s_ESP8266.e_Demand_Steup = Demand_Steup10;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)
					s_ESP8266.e_Demand_Steup = Demand_Steup0;
			}
		break;			

		case Demand_Steup10:
			{
				if(s_ESP8266.e_Usart_Status == ReceiveCheckOk)	
				{
					s_ESP8266.e_Demand_Steup = Demand_Out;
				}
				else if(s_ESP8266.e_Usart_Status == ReceiveCheckError)
					s_ESP8266.e_Demand_Steup = Demand_Steup0;
			}
		break;				

		case Demand_Out:
			break; 

		default: 
			break; 
	}
}
/*****************************************************************************************
* Function Name: ESP8266_Binding_Enable
* Description  : ESP8266 Wifi绑定使能
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void ESP8266_Binding_Enable(void)
{
	s_ESP8266.Wifi_BindingF = true;
	s_ESP8266.Wifi_ConenectF = false;
	s_ESP8266.Wifi_BindingCnt = 0;
	s_ESP8266.e_STA_Steup = STA_Steup0;
}
/*****************************************************************************************
* Function Name: Get_ESP8266_Binding_Status
* Description  : 获取绑定状态
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
uint8_t Get_ESP8266_Binding_Status(void)
{
	return s_ESP8266.Wifi_BindingF;
}
/*****************************************************************************************
* Function Name: Get_ESP8266_Conenect_Status
* Description  : 获取连接状态
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
uint8_t Get_ESP8266_Conenect_Status(void)
{
	return s_ESP8266.Wifi_ConenectF;
}
/*****************************************************************************************
* Function Name: Clean_ESP8266_RX_BUF
* Description  : 清零接收缓存
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Clean_ESP8266_RX_BUF(void)
{
	uint16_t i;
	
	for(i=0; i<ESP8266_MAX_RECV_LEN; i++)
	{
		s_ESP8266.RX_BUF[i] = 0;
	}
}
/*****************************************************************************************
* Function Name: ESP8266_Send_Data
* Description  : 控制ESP8266发送数据
* Arguments    : *pBuf：需要发送的数据指针
                 len：需要发送的数据长度
* Return Value : NONE
******************************************************************************************/
void ESP8266_Send_Data(uint8_t *pBuf, uint16_t len)
{
	if((Get_ESP8266_Binding_Status() == false) && (Get_ESP8266_Conenect_Status() == true) && (s_ESP8266.e_Demand_Steup == Demand_Out))		//只有在连接状态才能发生数据
	{
		ESP8266_Usart_Send(pBuf, len);
	}
}
/*****************************************************************************************
* Function Name: ESP8266_Receive_Data
* Description  : 从ESP8266接收数据
* Arguments    : *pBuf：接收数据指针             
* Return Value : len：接收到的数据长度
******************************************************************************************/
uint16_t ESP8266_Receive_Data(uint8_t *pBuf)
{
	uint16_t i,len=0;
	
	if((Get_ESP8266_Binding_Status() == false) && (Get_ESP8266_Conenect_Status() == true) && (s_ESP8266.e_Demand_Steup == Demand_Out))		//只有在连接状态接收数据才有效
	{
		if(s_ESP8266.RX_STA&0X8000)	
		{
			len = s_ESP8266.RX_STA&0X7FFF;
			for(i=0; i<len; i++)
			{
				*pBuf = s_ESP8266.RX_BUF[i];
				pBuf++;
			}
			
			s_ESP8266.RX_STA = 0;
		}
	}
	
	return len;
}
/***********************************END OF FILE*******************************************/


