/******************************************************************************
 * @file     system_M100.c
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

#include <string.h>

#include "MIZAR.h"
#include "RTE_Device.h"
#include "system_M100.h"
#if (defined(RTE_UART0_CONSOLE) && (RTE_UART0_CONSOLE == 1U)) || (defined(RTE_UART1_CONSOLE) && (RTE_UART1_CONSOLE == 1U))
#include "USART_MIZAR.h"
#endif

#include "ThinkTech_Timer.h" 
#include "ThinkTech_ESP8266.h" 

/*----------------------------------------------------------------------------
  Externals
 *----------------------------------------------------------------------------*/
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
extern uint32_t __Vectors;
#endif

/*----------------------------------------------------------------------------
  UART0/UART1 Console support
 *----------------------------------------------------------------------------*/
#define USART_BAUDRATE   115200UL

#if defined(RTE_UART0_CONSOLE) && (RTE_UART0_CONSOLE == 1U)
ARM_DRIVER_USART  *UART_Console = &Driver_USART0;
#elif defined(RTE_UART1_CONSOLE) && (RTE_UART1_CONSOLE == 1U)
ARM_DRIVER_USART  *UART_Console = &Driver_USART1;
#else
ARM_DRIVER_USART  *UART_Console = NULL;
#endif

uint8_t USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
uint8_t	RX_TEMP_BUF[10];
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t USART_RX_STA=0;       //接收状态标记	

//检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
uint8_t* USART_Check_Cmd(char *str)
{
	char *strx=0;
	
	if(USART_RX_STA&0X8000)				
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;	
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);
	} 
	return (uint8_t*)strx;
}

//清零串口接收缓存
void Clean_RX_BUF(void)
{
	uint8_t i;
	
	for(i=0; i<USART_REC_LEN; i++)
	{
		USART_RX_BUF[i] = 0;
	}
}

void UART_callback(uint32_t e)
{
	if (e & ARM_USART_EVENT_RECEIVE_COMPLETE) 
	{
		// set rx complete
		
		uint8_t Res;
		
		Res = RX_TEMP_BUF[0];
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  
		
		UART_Console->Receive(RX_TEMP_BUF, 1);
	}
}

/*----------------------------------------------------------------------------
  Stdout Initialize function
 *----------------------------------------------------------------------------*/
static int stdout_init (void)
{
  int32_t status;

  if (UART_Console == NULL)
    return (0);

  status = UART_Console->Initialize(UART_callback);
  if (status != ARM_DRIVER_OK) return (-1);

  status = UART_Console->PowerControl(ARM_POWER_FULL);
  if (status != ARM_DRIVER_OK) return (-1);

  status = UART_Console->Control(ARM_USART_DATA_BITS_8       |
                                 ARM_USART_PARITY_NONE       |
                                 ARM_USART_STOP_BITS_1       |
                                 ARM_USART_FLOW_CONTROL_NONE,
                                 USART_BAUDRATE);
  if (status != ARM_DRIVER_OK) return (-1);
  
  UART_Console->Receive(RX_TEMP_BUF, 1);

  return (0);
}

/*----------------------------------------------------------------------------
  Override C Library function to redirect contsole to serial
 *----------------------------------------------------------------------------*/
int stdout_putchar (int ch)
{
  static uint32_t stdout_initialized = 0U;
         uint8_t  buf[2], count;
         uint32_t primask;

  if (UART_Console == NULL)
    return (ch);

  if (!stdout_initialized) {
    stdout_init();
    stdout_initialized = 1U;
  }

  if (ch == '\n') {
    buf[0] = '\r';
    buf[1] = '\n';
    count  = 2U;
  } else {
    buf[0] = ch;
    count  = 1U;
  }

  primask = __disable_irq();

  while (UART_Console->Send(buf, count) != ARM_DRIVER_OK);

#if defined(RTE_UART_CONSOLE_POLL) && (RTE_UART_CONSOLE_POLL == 0U)
  while (UART_Console->GetTxCount() != count);
#endif

  if (primask == 0U) {
    __enable_irq();
  }

  return (ch);
}

/*----------------------------------------------------------------------------
  Acquire System Clock Frequency
 *----------------------------------------------------------------------------*/
static uint32_t faction_ms, faction_us;

uint32_t SystemCoreClock;

uint32_t GetSystemClock (void)
{
#ifdef FPGA
  return (25000000UL); /* Fixed frequency */
#else
  if (MIZAR_MISC->CLK_CTL & MIZAR_MISC_CLK_CTL_CLK_FREQ_Msk) {
    return ((MIZAR_MISC->CLK_CTL & MIZAR_MISC_CLK_CTL_CLK_FREQ_Msk) + 0x1U) * 10000000UL;
  }

  return (16000000UL); /* Default frequency */
#endif
}

void SystemCoreClockUpdate (void)
{
  SystemCoreClock = GetSystemClock();
  faction_ms      = SystemCoreClock / 1000;
  faction_us      = faction_ms / 1000;
}

#if !defined (RTE_CMSIS_RTOS) && !defined (RTE_CMSIS_RTOS2)

#if defined (__SYS_TIMER) && (__SYS_TIMER == 1U)
void udelay(uint32_t us)
{
  uint32_t ticks;
  uint32_t tick_old, tick_now, tick_cnt;
  uint32_t reload;

  ticks    = us * faction_us;
  tick_cnt = 0;
  tick_old = SysTick->VAL;
  reload   = SysTick->LOAD;

  while (1)
  {
    tick_now = SysTick->VAL;
    if (tick_now != tick_old)
    {
      if (tick_now < tick_old)
        tick_cnt += tick_old - tick_now;
      else
        tick_cnt += reload - tick_now + tick_old;
      tick_old = tick_now;
      if (tick_cnt >= ticks)
        break;
    }
  };
}
#else
void udelay(uint32_t us)
{
  uint32_t ticks;
  uint32_t tick_old, tick_now, tick_cnt;
  uint32_t reload;

  SysTick->LOAD  = faction_ms;
  SysTick->VAL   = 0x0;
  SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

  ticks    = us * faction_us;
  tick_cnt = 0;
  tick_old = SysTick->VAL;
  reload   = SysTick->LOAD;

  while (1)
  {
    tick_now = SysTick->VAL;
    if (tick_now != tick_old)
    {
      if (tick_now < tick_old)
        tick_cnt += tick_old - tick_now;
      else
        tick_cnt += reload - tick_now + tick_old;
      tick_old = tick_now;
      if (tick_cnt >= ticks)
        break;
    }
  };

  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
  SysTick->VAL   = 0x0;
}
#endif /* __SYS_TIMER */

void mdelay(uint16_t ms)
{
  udelay(ms * 1000);
}

#endif /* RTE_CMSIS_RTOS/RTE_CMSIS_RTOS2 */

/*----------------------------------------------------------------------------
  Systick Handler
 *----------------------------------------------------------------------------*/
#if defined (__SYS_TIMER) && (__SYS_TIMER == 1U)

uint32_t SystemTicks = 0x0UL;
uint32_t Frame_Cnt=0;
uint32_t TimeOut_Cnt=0;

void SysTick_Handler (void)
{
  SystemTicks ++;
	
	TMR_Drv();
	
	if(s_ESP8266.OpenFrameF == true)
	{
		Frame_Cnt++;
		if(Frame_Cnt == 100)
		{
			Frame_Cnt = 0;
			s_ESP8266.RX_STA |= 1<<15;
			s_ESP8266.OpenFrameF = false;
		}
	}
	else
	{
		Frame_Cnt = 0;
	}
	
	if(s_ESP8266.RX_STA&0X8000)	
	{
		TimeOut_Cnt++;
		if(TimeOut_Cnt == 5000)
		{
			TimeOut_Cnt = 0;
			s_ESP8266.RX_STA = 0;
			Clean_ESP8266_RX_BUF();				//接收到一帧数据后，等待5S读取数据，否则清空接收缓存
		}
	}
	else
	{
		TimeOut_Cnt = 0;	
	}
}

uint32_t GetTicks (void)
{
  return SystemTicks;
}

#endif

/*----------------------------------------------------------------------------
  Config PLL Frequency
 *----------------------------------------------------------------------------*/
#ifndef FPGA

static void SetPllFreq(void)
{
  uint32_t generalFlags, pllValue;

  /* read OTP general flag word to determine freq config */
  generalFlags   = *(volatile uint32_t *)(0x4002209cUL);
//  pllValue       = (generalFlags & MIZAR_MISC_CLK_CTL_CLK_FREQ_Msk);
  pllValue       = 5;

  /* Wait PLL to be stable */
  while (!(MIZAR_MISC->CLK_CTL & MIZAR_MISC_CLK_CTL_CLK_STABLE_Msk));

  /* Enable Flash Cache */
  MIZAR_MISC->CLK_CTL = (pllValue << MIZAR_MISC_CLK_CTL_CLK_FREQ_Pos);

  /* Wait PLL to be stable */
  while (!(MIZAR_MISC->CLK_CTL & MIZAR_MISC_CLK_CTL_CLK_STABLE_Msk));
}

static void PllConfig(void)
{
  /* if FREQ not already configured */
  if (!(MIZAR_MISC->CLK_CTL & MIZAR_MISC_CLK_CTL_CLK_FREQ_Msk)) {
    memcpy((void *)(0x20020000UL), (const void *)((uint32_t)SetPllFreq & ~0x1), 0x100);

    /* Jump to SetPllFreq function */
    __asm("blx      %0"::"r"(0x20020001UL):);
  }
}

#endif /* FPGA */

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
void SystemInit (void)
{
#ifndef FPGA
  /* Reset all modules when sysreset and lockup reset */
  MIZAR_MISC->SYSRESET_EN    = 0x0FFFFFFF;
  MIZAR_MISC->LOCKUPRESET_EN = 0x0FFFFFFF;

  /* Statically disable some modules not needed */
  MIZAR_MISC->CLK_STOP  = MIZAR_MISC_CLK_STOP_DMAC_Msk         |
                          MIZAR_MISC_CLK_STOP_WDT_Msk          |
                          MIZAR_MISC_CLK_STOP_I2C0_Msk         |
                          MIZAR_MISC_CLK_STOP_I2C1_Msk         |
                          MIZAR_MISC_CLK_STOP_UART0_Msk        |
                          MIZAR_MISC_CLK_STOP_SSP0_Msk         |
                          MIZAR_MISC_CLK_STOP_SSP1_Msk         |
                          MIZAR_MISC_CLK_STOP_SSP2_Msk         |
                          MIZAR_MISC_CLK_STOP_SSP3_Msk         |
                          MIZAR_MISC_CLK_STOP_TRNG_Msk         |
                          MIZAR_MISC_CLK_STOP_TRNG0_Msk        |
                          MIZAR_MISC_CLK_STOP_TRNG1_Msk        |
                          MIZAR_MISC_CLK_STOP_TRNG2_Msk        |
                          MIZAR_MISC_CLK_STOP_TRNG3_Msk        |
                          MIZAR_MISC_CLK_STOP_AES_Msk          |
                          MIZAR_MISC_CLK_STOP_DES_Msk          |
                          MIZAR_MISC_CLK_STOP_PKI_Msk          |
                          MIZAR_MISC_CLK_STOP_SM3_Msk          |
                          MIZAR_MISC_CLK_STOP_SM4_Msk;

  /* The Following module can also optionally dynamic stop:
   * MIZAR_MISC_SYS_SLEEP_GPIO_Msk/MIZAR_MISC_SYS_SLEEP_SRAM_Msk
   * MIZAR_MISC_SYS_SLEEP_SSP0_Msk/MIZAR_MISC_SYS_SLEEP_SSP1_Msk
   * MIZAR_MISC_SYS_SLEEP_SSP2_Msk/MIZAR_MISC_SYS_SLEEP_SSP3_Msk
   * MIZAR_MISC_SYS_SLEEP_TIMER_Msk/MIZAR_MISC_SYS_SLEEP_WDT_Msk
   * MIZAR_MISC_SYS_SLEEP_UART0_Msk/MIZAR_MISC_SYS_SLEEP_UART1_Msk
   * MIZAR_MISC_SYS_SLEEP_I2C0_Msk/MIZAR_MISC_SYS_SLEEP_I2C1_Msk
   */
  MIZAR_MISC->SYS_SLEEP = MIZAR_MISC_SYS_SLEEP_EN_Msk          |
                          MIZAR_MISC_SYS_SLEEP_TRNG_Msk        |
                          MIZAR_MISC_SYS_SLEEP_TRNG0_Msk       |
                          MIZAR_MISC_SYS_SLEEP_TRNG1_Msk       |
                          MIZAR_MISC_SYS_SLEEP_TRNG2_Msk       |
                          MIZAR_MISC_SYS_SLEEP_TRNG3_Msk       |
                          MIZAR_MISC_SYS_SLEEP_EFLASH_Msk      |
                          MIZAR_MISC_SYS_SLEEP_FLASH_CACHE_Msk |
                          MIZAR_MISC_SYS_SLEEP_AES_Msk         |
                          MIZAR_MISC_SYS_SLEEP_DES_Msk         |
                          MIZAR_MISC_SYS_SLEEP_PKI_Msk         |
                          MIZAR_MISC_SYS_SLEEP_SM3_Msk         |
                          MIZAR_MISC_SYS_SLEEP_SM4_Msk;
#endif

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t) &__Vectors;
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

#ifndef FPGA
  PllConfig (); /* PLL Initialization */
#endif

#if defined (__SYS_TIMER) && (__SYS_TIMER == 1U)
  SysTick_Config (GetSystemClock() / HZ);
#endif
}
