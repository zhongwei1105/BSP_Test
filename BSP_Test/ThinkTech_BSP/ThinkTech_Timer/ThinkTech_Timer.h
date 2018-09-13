/****************************************************************************************
**  Filename :  ThinkTech_Timer.h
**  Abstract :  This file implements key function.
**  By       :  钟伟
**  Date     :  2018-08-13 
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _TKT_TIMER_H
#define _TKT_TIMER_H
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
#define TMR_OUT_MAX    			(0xFFFFFFFF)

#define TMR_10MS_Base_TIME		((uint8_t)10)		
#define TMR_100MS_Base_TIME		((uint8_t)100)
#define TMR_500MS_Base_TIME		((uint8_t)500)
#define TMR_1MS_Base_TIME		((uint8_t)1000)
/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
//TMR ID定义
typedef enum
{ 
	TMR_10MS_Base = 0,
	TMR_100MS_Base = 0,
	TMR_500MS_Base = 0,
	TMR_1S_Base = 0,
	
	TMR_MAX
}TE_TMR_ID;

//TMR控制结构体
typedef struct
{  
	uint8_t  	IsValidF:1; 	/* Timer是否有效 */
	uint32_t  ActiveTime;  		/* 计时参数 */
	uint32_t  TimeOut;     		/* 超时时间 */
}TS_TMR_RESOURCES;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern void TMR_Open(TE_TMR_ID e_TMR_ID, bool ResetCounterF, uint32_t TimeOut);
extern void TMR_Close(TE_TMR_ID e_TMR_ID, bool ResetCounterF);
extern bool TMR_IsOut(TE_TMR_ID e_TMR_ID);
extern uint32_t TMR_GetCurVal(TE_TMR_ID e_TMR_ID);
extern void TMR_Drv(void);
//----------------------------------------------------------------------------------------
#endif
/***********************************END OF FILE*******************************************/



