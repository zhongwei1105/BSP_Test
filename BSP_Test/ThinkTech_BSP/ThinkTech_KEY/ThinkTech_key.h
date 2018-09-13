/****************************************************************************************
**  Filename :  ThinkTech_key.h
**  Abstract :  This file implements key function.
**  By       :  钟伟
**  Date     :  2018-08-13 
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _TKT_KEY_H
#define _TKT_KEY_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "GPIO_MIZAR.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
//io宏定义
#define KEY0_PIN                        5	//GPIO5
#define KEY0_DOWN_LEVEL               	1  	//根据原理图设计，KEY0按下时引脚为高电平，所以这里设置为1

#define KEY1_PIN                        6	//GPIO6
#define KEY1_DOWN_LEVEL               	1  	//根据原理图设计，KEY0按下时引脚为高电平，所以这里设置为1

#define KEY2_PIN                        7	//GPIO7
#define KEY2_DOWN_LEVEL               	1  	//根据原理图设计，KEY0按下时引脚为高电平，所以这里设置为1

//----------------------------------------------------------------------------------------
//HAL库函数方式读取IO
#define KEY0        					((GPIO_PinRead(KEY0_PIN)==KEY0_DOWN_LEVEL)?1:0)  //KEY0按键GPIO5
#define KEY1        					((GPIO_PinRead(KEY1_PIN)==KEY1_DOWN_LEVEL)?1:0)  //KEY1按键GPIO6
#define KEY2        					((GPIO_PinRead(KEY2_PIN)==KEY2_DOWN_LEVEL)?1:0)  //KEY2按键GPIO7

#define KET_VALUE						((KEY2<<2)|(KEY1<<1)|(KEY0))	//每个位代表一个IO的输入电平
//----------------------------------------------------------------------------------------
////键值定义，KeyId
#define KEY_NONE                		0x0000 //无键按下
#define KEY0_ID 						0x0001 //对应实体IO_KEY0
#define KEY1_ID							0x0002 //对应实体IO_KEY1
#define KEY2_ID							0x0004 //对应实体IO_KEY2
#define KEY0_KEY1_ID					0x0003 //对应实体IO_KEY0+IO_KEY1
//----------------------------------------------------------------------------------------
//按键时间定义，如短按，长按，重复键
#define KEY_PRESSED_TMR               	(uint32_t)5		// 50   ms
#define KEY_LONG_TMR                	(uint32_t)200	// 2000 ms 
#define KEY_UP_TMR                		(uint32_t)6		// 60  ms
#define KEY_REPEAT_TMR                  (uint32_t)10	// 100  ms
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
//按键状态定义
typedef enum
{
	KEY_STATE_INIT = 0,
	KEY_STATE_WOBBLE,
	KEY_STATE_DWON,
	KEY_STATE_WAITUP,
} TE_KEY_STATE;

//按键事件定义
typedef enum
{
	KEY_NULL = 0x00,
	KEY_PRESSED = 0x01,
	KEY_LONG = 0x02,
	KEY_UP = 0x04,
	KEY_REPEAT = 0x08
} TE_KEY_EVENT;

//按键属性结构体
typedef struct   
{ 
	uint8_t KeyState;                     // 按键状态         
	uint16_t KeyDwTmr;                    // 按键按下的时间
	uint32_t KeyCur;                      // 记录发生按键事件时的键值
	TE_KEY_EVENT KeyEvent;				  // 按键事件
} TS_KEY_INFO; 
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern void KEY_Init(void);
extern void KEY_Drv(void);
extern bool IsKey(uint32_t KeyId, TE_KEY_EVENT KeyEvent);
//----------------------------------------------------------------------------------------
#endif
/***********************************END OF FILE*******************************************/

