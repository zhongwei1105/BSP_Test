/****************************************************************************************
**  Filename :  ThinkTech_key.c
**  Abstract :  This file implements key function.
**  By       :  钟伟
**  Date     :  2018-08-13 
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "ThinkTech_key.h" 
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
static TS_KEY_INFO s_KeyInfo;
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
/*****************************************************************************************
* Function Name : KEY_Init
* Description   : 按键初始化函数
* Arguments     : NONE
* Return Value	: NONE
******************************************************************************************/
void KEY_Init(void)
{
	GPIO_PinSetDirection(KEY0_PIN, GPIO_PIN_DIRECTION_INPUT);
	GPIO_PinSetDirection(KEY1_PIN, GPIO_PIN_DIRECTION_INPUT);
	GPIO_PinSetDirection(KEY2_PIN, GPIO_PIN_DIRECTION_INPUT);
}
/*****************************************************************************************
* Function Name : GetKey
* Description   : 获取键值,返回值每一位代表一个按键按下，支持组合按键
* Arguments     : NONE
* Return Value	: 返回按键值
******************************************************************************************/
static uint32_t GetKey(void)
{
	return KET_VALUE;
} 
/*****************************************************************************************
* Function Name: KeyModuleProcess
* Description  : 按键检测流程
* Arguments    : 按键结构体指针
* Return Value : 返回TRUE表示有触发(按下或抬起)，FALSE表示无触发
******************************************************************************************/
static uint8_t KeyModuleProcess(TS_KEY_INFO* pKeyInfo) 
{ 
	static uint32_t KeyLast = 0;
	uint32_t KeyCur = 0;

	KeyCur = GetKey();
	
	switch(pKeyInfo->KeyState) 
	{ 
		case KEY_STATE_INIT: 
			KeyLast = 0;
			pKeyInfo->KeyDwTmr = 0; 
			if(KeyCur != KEY_NONE)    
			{
				pKeyInfo->KeyState = KEY_STATE_WOBBLE; 
			}				     
		break; 
        
		case KEY_STATE_WOBBLE://消抖
			pKeyInfo->KeyState = KEY_STATE_DWON;   
		break; 
        
		case KEY_STATE_DWON:
			pKeyInfo->KeyDwTmr++;
			if(KeyCur == KeyLast) 
			{  
				if(pKeyInfo->KeyDwTmr >= KEY_LONG_TMR) //长按
				{
					pKeyInfo->KeyCur = KeyLast;
					pKeyInfo->KeyEvent |= KEY_LONG;
					pKeyInfo->KeyState = KEY_STATE_WAITUP;
				}
			} 
			else 
			{ 
				if(KeyCur == KEY_NONE) 
				{
					if(pKeyInfo->KeyDwTmr >= KEY_PRESSED_TMR) //点按
					{
						pKeyInfo->KeyCur = KeyLast;
						pKeyInfo->KeyEvent |= KEY_PRESSED;
						pKeyInfo->KeyState = KEY_STATE_INIT;
					}
				}
				else
				{
					pKeyInfo->KeyState = KEY_STATE_INIT;
				}
			} 
		break;       	
        
		case KEY_STATE_WAITUP: 
			if(KeyCur == KeyLast) 
			{ 
				if(++pKeyInfo->KeyDwTmr >= KEY_REPEAT_TMR) //连发
				{
					pKeyInfo->KeyCur = KeyLast;
					pKeyInfo->KeyEvent |= KEY_REPEAT;
					pKeyInfo->KeyDwTmr = 0;
				}
			} 
			else
			{
				if(KeyCur == KEY_NONE)  
				{
					pKeyInfo->KeyCur = KeyLast;
					pKeyInfo->KeyEvent |= KEY_UP;
					pKeyInfo->KeyState = KEY_STATE_INIT; 
				}
			}
		break;
        
		default: 
			break; 
	} 
    
	if(KeyLast != KeyCur)
	{
		KeyLast = KeyCur;               //保存上次的按键值
	}
	return false; 
}  
/*****************************************************************************************
* Function Name: KeyDrv
* Description  : 按键驱动函数,需放10ms的循环时间片中
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void KEY_Drv(void)
{	 	
	KeyModuleProcess(&s_KeyInfo);	
}
/*****************************************************************************************
* Function Name: IsKey
* Description  : 按键事件判断API
* Arguments    : KeyId:按键ID / KeyEvent:按键事件类型
* Return Value : true:有KeyEvent事件发生 / fasle:无
******************************************************************************************/
bool IsKey(uint32_t KeyId, TE_KEY_EVENT KeyEvent)
{
	if(s_KeyInfo.KeyCur == KeyId)
	{
		if(s_KeyInfo.KeyEvent & KeyEvent)
		{
			s_KeyInfo.KeyEvent = KEY_NULL;
			return true;
		}
	}
	return false;
}
/***********************************END OF FILE*******************************************/

