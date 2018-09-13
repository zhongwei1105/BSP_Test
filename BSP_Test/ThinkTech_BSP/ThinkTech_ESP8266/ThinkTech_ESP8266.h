/****************************************************************************************
**  Filename :  ThinkTech_ESP8266.h
**  Abstract :  This file implements key function.
**  By       :  钟伟
**  Date     :  2018-08-13 
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _TKT_ESP8266_H
#define _TKT_ESP8266_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
#define ESP8266_MAX_RECV_LEN		1024				
#define ESP8266_MAX_SEND_LEN		256	

#define Debug_printf				0				//是否打印调试信息，默认关闭

#define	Demand_Set_Cnt				3000			//查询间隔，默认30S
#define	Binding_Set_Cnt				12000			//绑定超时，默认120S

extern uint8_t PortNum[5];			//服务器端口
extern uint8_t ServerIP[15];		//服务器IP
extern uint8_t Wif_ssid[31];		//Wifi账号
extern uint8_t Wif_password[31];	//Wifi密码
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
typedef enum
{ 
	Idle = 0,
	Sending,
	SendFinish,
	SendError,
	Receiveing,
	ReceiveCheckOk,
	ReceiveCheckError,
}TE_ESP8266_Usart_Status;

typedef enum
{ 
	Out = 0,
	StartSend, 
	WaitSend,
	WaitReceive,
	CheckReceive,
}TE_ESP8266_Usart_Steup;

typedef enum
{ 
	STA_Out = 0,
	STA_Steup0, 
	STA_Steup1,
	STA_Steup2,
	STA_Steup3,
	STA_Steup4,
	STA_Steup5,
	STA_Steup6,
	STA_Steup7,
	STA_Steup8,
	STA_Steup9,
	STA_Steup10,
	STA_Steup11,
	STA_Steup12,
	STA_Steup13,
	STA_Steup14,
	STA_Steup15,
	STA_Steup16,
	STA_Steup17,
}TE_ESP8266_STA_Steup;

typedef enum
{ 
	Demand_Out = 0,
	Demand_Steup0, 
	Demand_Steup1,
	Demand_Steup2,
	Demand_Steup3,
	Demand_Steup4,
	Demand_Steup5,
	Demand_Steup6,
	Demand_Steup7,
	Demand_Steup8,
	Demand_Steup9,
	Demand_Steup10,
}TE_ESP8266_Demand_Steup;

//ESP8266控制结构体
typedef struct
{  
	uint8_t  						Tx_CompleteF:1;							//串口数据发送完成标志，在回调中使用
	uint8_t 						Rx_CompleteF:1;							//串口数据接收完成标志，在回调中使用
	uint8_t  						Wifi_BindingF:1;						//Wifi绑定标志，0：位绑定，1：绑定
	uint8_t 						Wifi_ConenectF:1;						//Wifi连接状态，0：未连接，1：连接
	uint8_t  						OpenFrameF:1;
	uint8_t 						TX_BUF[ESP8266_MAX_SEND_LEN]; 			//数据发送缓存 
	uint8_t							RX_TEMP_BUF[10];						//数据接收缓存 
	uint8_t  						RX_BUF[ESP8266_MAX_RECV_LEN];  			//数据接收存储 
	char	 						*Cmd;									//AT命令缓存
	char	 						*Ack;									//AT命令发送后，期望应答字符串									
	uint16_t	 					WaitTime;								//发送数据后等待应答时间
	uint16_t						FrameCnt;								//断帧计时									
	uint32_t						Wifi_BindingCnt;						//Wifi绑定时间计时，30S超时
	uint32_t						Demand_Cnt;								//查询连接状态计时
	uint32_t  						RX_STA;									//接收数据计数，最高位用来表示一帧数据接收完成
	TE_ESP8266_Usart_Status 		e_Usart_Status;							//串口发送与接收状态
	TE_ESP8266_Usart_Steup			e_Usart_Steup;							//串口发送与接收控制步骤
	TE_ESP8266_STA_Steup			e_STA_Steup;							//STA模式TCP连接步骤
	TE_ESP8266_Demand_Steup			e_Demand_Steup;							//查询连接状态步骤
}TS_ESP8266_RESOURCES;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern TS_ESP8266_RESOURCES	s_ESP8266;

void ESP8266_Usart_printf(char* fmt,...);  
extern void ESP8266_Usart_Send(uint8_t *pBuf, uint16_t len);  
void ESP8266_Usart_Receive(uint8_t *pBuf, uint16_t len); 
uint8_t* ESP8266_Check_Cmd(char *str);
uint8_t ESP8266_Send_Cmd(char *cmd, char *ack, uint16_t waittime);
void ESP8266_STA_Config(void);
void ESP8266_Demand(void);
extern void ESP8266_Usart_Init(uint32_t BAUD_RATE);
extern void ESP8266_Drv(void);
extern void ESP8266_Binding_Enable(void);
extern uint8_t Get_ESP8266_Binding_Status(void);
extern uint8_t Get_ESP8266_Conenect_Status(void);
extern void Clean_ESP8266_RX_BUF(void);
extern void ESP8266_Send_Data(uint8_t *pBuf, uint16_t len);  
extern uint16_t ESP8266_Receive_Data(uint8_t *pBuf); 
//----------------------------------------------------------------------------------------
#endif
/***********************************END OF FILE*******************************************/



