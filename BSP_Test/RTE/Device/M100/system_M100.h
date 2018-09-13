/******************************************************************************
 * @file     system_M100.h
 * @brief    CMSIS Device System Source File for ThinkTech M100 Device Series
 * @version  V5.00
 * @date     02. Dec 2017
 ******************************************************************************/
/*
 * Copyright (c) 2017-2019 ThinkTech Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SYSTEM_M100_H
#define __SYSTEM_M100_H

#include <RTE_Components.h>
#include <stdint.h>
#include <stdbool.h>
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	

#ifdef __cplusplus
extern "C" {
#endif

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//   <q>Alternative Vector Table Paramters
//   <i> Enable Alternative Vector Table Paramters
#define __VTOR_PRESENT            1

#if !defined (RTE_CMSIS_RTOS) && !defined (RTE_CMSIS_RTOS2)
//   <e>SysTick Timer Enable
//   <i> Enables SysTick Timer.
#define __SYS_TIMER               1

//     <o>SysTick Timer Frequency [bytes] <0-1073741824:8>
//     <i> Defines the SysTick timer frequency.
//     <i> Default: 1000
#define HZ                        1000

//   </e>
#endif

//------------- <<< end of configuration section >>> ---------------------------

/**
  \brief Debug output operation
 */
#ifdef DEBUG
#define DEBUG_PRINT(format, ...)  printf(format, ##__VA_ARGS__)
#else
#define DEBUG_PRINT( ... )        do {} while (0)
#endif

/**
  \brief Do not support exclusive operations
 */
#define EXCLUSIVE_ACCESS          0  /* CMSIS 5.3.0 */
#define __EXCLUSIVE_ACCESS        0  /* CMSIS 5.2.0 */
	
extern uint32_t Frame_Cnt;
	
#define USART_REC_LEN  			  200  			//定义最大接收字节数 200
extern uint8_t  USART_RX_BUF[USART_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint8_t	RX_TEMP_BUF[10];
extern uint16_t USART_RX_STA;         			//接收状态标记	

uint8_t* USART_Check_Cmd(char *str);
void Clean_RX_BUF(void);	

/**
  \brief System Clock Frequency (Core Clock)
 */
extern uint32_t SystemCoreClock;

/**
  \brief Setup the microcontroller system.
 */
extern void SystemInit (void);

/**
  \brief Get system clock frequency.
 */
extern uint32_t GetSystemClock (void);

/**
  \brief  Update SystemCoreClock variable.
 */
extern void SystemCoreClockUpdate (void);

/**
  \brief Get system tick counter value.
 */
extern uint32_t GetTicks (void);

#if !defined (RTE_CMSIS_RTOS) && !defined (RTE_CMSIS_RTOS2)
/**
  \brief Delay functions.
 */
extern void udelay(uint32_t us);
extern void mdelay(uint16_t ms);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_M100_H */
