/****************************************************************************************
**  Filename :  ThinkTech_Timer.c
**  Abstract :  This file implements led function.
**  By       :  钟伟
**  Date     :  2018-08-13 
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "ThinkTech_Timer.h" 
/*****************************************************************************************
								Variables definitions
*****************************************************************************************/
static TS_TMR_RESOURCES		s_TMR[TMR_MAX];
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
/*****************************************************************************************
* Function Name: TMR_open
* Description  : TMR初始化，激活TMR
* Arguments    : e_TMR_ID：ID 
				 ResetCounterF：复位计数值，1：复位 
				 timeout: 设置超时时间
* Return Value : NONE
******************************************************************************************/
void TMR_Open(TE_TMR_ID e_TMR_ID, bool ResetCounterF, uint32_t TimeOut)
{
	if ((e_TMR_ID >= TMR_MAX) || (TimeOut >= TMR_OUT_MAX))
	{
		return;
	}
	
	s_TMR[e_TMR_ID].IsValidF = true;
	
	if (ResetCounterF)
	{
		s_TMR[e_TMR_ID].ActiveTime = 0;
	}

	s_TMR[e_TMR_ID].TimeOut = TimeOut;
}
/*****************************************************************************************
* Function Name: TMR_close
* Description  : 关闭TMR
* Arguments    : e_TMR_ID：ID 
				 ResetCounterF：复位计数值，1：复位 
* Return Value : NONE
******************************************************************************************/
void TMR_Close(TE_TMR_ID e_TMR_ID, bool ResetCounterF)
{
	s_TMR[e_TMR_ID].IsValidF = false;

	if (ResetCounterF)
	{
		s_TMR[e_TMR_ID].ActiveTime = 0;
	}
}
/*****************************************************************************************
* Function Name: TMR_IsOut
* Description  : 判断是否超时
* Arguments    : e_TMR_ID：ID 
* Return Value : true: 超时
				 false: 未超时
******************************************************************************************/
bool TMR_IsOut(TE_TMR_ID e_TMR_ID)
{
	if (s_TMR[e_TMR_ID].ActiveTime >= s_TMR[e_TMR_ID].TimeOut)
	{
		return true;
	}

	return false;
}
/*****************************************************************************************
* Function Name: TMR_GetCurVal
* Description  : 获取当前计数值
* Arguments    : e_TMR_ID：ID 
* Return Value : 当前计数值
******************************************************************************************/
uint32_t TMR_GetCurVal(TE_TMR_ID e_TMR_ID)
{
	return s_TMR[e_TMR_ID].ActiveTime;
}
/*****************************************************************************************
* Function Name: TMR_Drv
* Description  : TMR进程，需放在1ms时间片中
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void TMR_Drv(void)
{
	uint8_t timerPos = 0;
	for (timerPos = 0; timerPos < TMR_MAX; timerPos ++)
	{
		if ((s_TMR[timerPos].IsValidF == true) && (s_TMR[timerPos].ActiveTime < s_TMR[timerPos].TimeOut))	
		{
			s_TMR[timerPos].ActiveTime += 1;
		}
	}
}
/***********************************END OF FILE*******************************************/


