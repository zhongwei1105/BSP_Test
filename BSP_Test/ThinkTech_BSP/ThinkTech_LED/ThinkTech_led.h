/****************************************************************************************
**  Filename :  ThinkTech_led.h
**  Abstract :  This file implements key function.
**  By       :  钟伟
**  Date     :  2018-08-13 
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _TKT_LED_H
#define _TKT_LED_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include <stdint.h>
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
//IO引脚定义
#define LED0_PIN              0		//GPIO0

#define LED1_PIN              1		//GPIO1

#define LED2_PIN              2		//GPIO2

#define LED3_PIN              3		//GPIO3

#define LED4_PIN              4		//GPIO4

//IO输出定义
#define	GPIO_PIN_RESET 					0
#define	GPIO_PIN_SET 					1

#define LED0_ON() 						GPIO_PinWrite(LED0_PIN,GPIO_PIN_SET) 				//高电平点亮
#define LED0_OFF() 						GPIO_PinWrite(LED0_PIN,GPIO_PIN_RESET) 

#define LED1_ON() 						GPIO_PinWrite(LED1_PIN,GPIO_PIN_SET) 				//高电平点亮
#define LED1_OFF() 						GPIO_PinWrite(LED1_PIN,GPIO_PIN_RESET) 

#define LED2_ON() 						GPIO_PinWrite(LED2_PIN,GPIO_PIN_SET) 				//高电平点亮
#define LED2_OFF() 						GPIO_PinWrite(LED2_PIN,GPIO_PIN_RESET) 

#define LED3_ON() 						GPIO_PinWrite(LED3_PIN,GPIO_PIN_SET) 				//高电平点亮
#define LED3_OFF() 						GPIO_PinWrite(LED3_PIN,GPIO_PIN_RESET) 

#define LED4_ON() 						GPIO_PinWrite(LED4_PIN,GPIO_PIN_SET) 				//高电平点亮
#define LED4_OFF() 						GPIO_PinWrite(LED4_PIN,GPIO_PIN_RESET) 
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
//LED名称重定义，顺序和上方的宏定义保持一致
typedef enum 
{
	LED0 = 0,
	LED1,
	LED2,
	LED3,
	LED4,
	LED_ID_MAX
}TE_LED_ID;

//LED工作状态
typedef enum  
{
	LED_OFF,
	LED_ON,
	LED_FLASH,
	LED_KEEP_FLASHING
}TE_LED_STATE;

//LED控制结构体
typedef struct{
	uint8_t State[LED_ID_MAX];//LED	的工作状态
	uint16_t Flash_Duty[LED_ID_MAX];//LED占空比，即点亮时间
	uint16_t Flash_Period[LED_ID_MAX];//LED周期
	uint16_t Flash_Times[LED_ID_MAX];//闪烁次数
	uint16_t Timer[LED_ID_MAX];//时间计数 1ms计数一次
}TS_LED_RESOURCES;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern void LED_Init(void);
extern void LED_Clr(void);
extern void LED_Drv(void);
extern void LED_Set(TE_LED_ID e_LedId, TE_LED_STATE e_State, uint16_t Duty, uint16_t Period, uint16_t Times);
//----------------------------------------------------------------------------------------
#endif
/***********************************END OF FILE*******************************************/
