;/******************************************************************************
; * @file     startup_M100.s
; * @brief    CMSIS Device System Source File for ThinkTech M100 Device Series
; * @version  V5.00
; * @date     02. Dec 2017
; ******************************************************************************/
;/*
; * Copyright (c) 2017-2019 ThinkTech Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00001000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000C00

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     TRNG_IRQHandler           ;  0: TRNG
                DCD     SSP3_RT_IRQHandler        ;  1: SSP3 Receive Timeout
                DCD     SSP3_ROR_IRQHandler       ;  2: SSP3 Receive Overrun
                DCD     SSP3_TX_IRQHandler        ;  3: SSP3 Transmit
                DCD     SSP3_RX_IRQHandler        ;  4: SSP3 Receive
                DCD     SSP3_COMBINE_IRQHandler   ;  5: SSP3 Combine
                DCD     SSP2_RT_IRQHandler        ;  6: SSP2 Receive Timeout
                DCD     SSP2_ROR_IRQHandler       ;  7: SSP2 Receive Overrun
                DCD     SSP2_TX_IRQHandler        ;  8: SSP2 Transmit
                DCD     SSP2_RX_IRQHandler        ;  9: SSP2 Receive
                DCD     SSP2_COMBINE_IRQHandler   ; 10: SSP2 Combine
                DCD     SSP1_RT_IRQHandler        ; 11: SSP1 Receive Timeout
                DCD     SSP1_ROR_IRQHandler       ; 12: SSP1 Receive Overrun
                DCD     SSP1_TX_IRQHandler        ; 13: SSP1 Transmit
                DCD     SSP1_RX_IRQHandler        ; 14: SSP1 Receive
                DCD     SSP1_COMBINE_IRQHandler   ; 15: SSP1 Combine
                DCD     SSP0_RT_IRQHandler        ; 16: SSP0 Receive Timeout
                DCD     SSP0_ROR_IRQHandler       ; 17: SSP0 Receive Overrun
                DCD     SSP0_TX_IRQHandler        ; 18: SSP0 Transmit
                DCD     SSP0_RX_IRQHandler        ; 19: SSP0 Receive
                DCD     SSP0_COMBINE_IRQHandler   ; 20: SSP0 Combine
                DCD     GPDMA_COMBINE_IRQHandler  ; 21: GPDMA Combine
                DCD     GPDMA_TC_IRQHandler       ; 22: GPDMA Terminal Count
                DCD     GPDMA_ERR_IRQHandler      ; 23: GPDMA Error
                DCD     FLASH_CACHE_IRQHandler    ; 24: Flash Cache
                DCD     CC312_IRQHandler          ; 25: CryptoCell-312
                DCD     PKI_RSA_IRQHandler        ; 26: Pki RSA
                DCD     TIMER_IRQHandler          ; 27: Timer
                DCD     WDT_IRQHandler            ; 28: Watchdog Timer
                DCD     0                         ; 29: Reserved
                DCD     I2C1_IRQHandler           ; 30: I2C 1
                DCD     I2C0_IRQHandler           ; 31: I2C 0
                DCD     UART0_COMBINE_IRQHandler  ; 32: UART0 Combine
                DCD     UART0_ERR_IRQHandler      ; 33: UART0 Error
                DCD     UART0_RT_IRQHandler       ; 34: UART0 Receive Timeout
                DCD     UART0_TX_IRQHandler       ; 35: UART0 Transmit
                DCD     UART0_RX_IRQHandler       ; 36: UART0 Receive
                DCD     UART0_MS_IRQHandler       ; 37: UART0 Modem Status
                DCD     GPIO0_IRQHandler          ; 38: GPIO 0
                DCD     GPIO1_IRQHandler          ; 39: GPIO 1
                DCD     GPIO2_IRQHandler          ; 40: GPIO 2
                DCD     GPIO3_IRQHandler          ; 41: GPIO 3
                DCD     GPIO4_IRQHandler          ; 42: GPIO 4
                DCD     GPIO5_IRQHandler          ; 43: GPIO 5
                DCD     GPIO6_IRQHandler          ; 44: GPIO 6
                DCD     GPIO7_IRQHandler          ; 45: GPIO 7
                DCD     GPIO_COMBINE_IRQHandler   ; 46: GPIO Combine
                DCD     eFLASH_IRQHandler         ; 47: eFLASH
                DCD     UART1_COMBINE_IRQHandler  ; 48: UART1 Combine
                DCD     UART1_ERR_IRQHandler      ; 49: UART1 Error
                DCD     UART1_RT_IRQHandler       ; 50: UART1 Receive Timeout
                DCD     UART1_TX_IRQHandler       ; 51: UART1 Transmit
                DCD     UART1_RX_IRQHandler       ; 52: UART1 Receive
                DCD     UART1_MS_IRQHandler       ; 53: UART1 Modem Status
                DCD     TRNG0_IRQHandler          ; 54: TRNG0
                DCD     TRNG1_IRQHandler          ; 55: TRNG1
                DCD     TRNG2_IRQHandler          ; 56: TRNG2
                DCD     TRNG3_IRQHandler          ; 57: TRNG3
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  TRNG_IRQHandler           [WEAK]
                EXPORT  SSP3_RT_IRQHandler        [WEAK]
                EXPORT  SSP3_ROR_IRQHandler       [WEAK]
                EXPORT  SSP3_TX_IRQHandler        [WEAK]
                EXPORT  SSP3_RX_IRQHandler        [WEAK]
                EXPORT  SSP3_COMBINE_IRQHandler   [WEAK]
                EXPORT  SSP2_RT_IRQHandler        [WEAK]
                EXPORT  SSP2_ROR_IRQHandler       [WEAK]
                EXPORT  SSP2_TX_IRQHandler        [WEAK]
                EXPORT  SSP2_RX_IRQHandler        [WEAK]
                EXPORT  SSP2_COMBINE_IRQHandler   [WEAK]
                EXPORT  SSP1_RT_IRQHandler        [WEAK]
                EXPORT  SSP1_ROR_IRQHandler       [WEAK]
                EXPORT  SSP1_TX_IRQHandler        [WEAK]
                EXPORT  SSP1_RX_IRQHandler        [WEAK]
                EXPORT  SSP1_COMBINE_IRQHandler   [WEAK]
                EXPORT  SSP0_RT_IRQHandler        [WEAK]
                EXPORT  SSP0_ROR_IRQHandler       [WEAK]
                EXPORT  SSP0_TX_IRQHandler        [WEAK]
                EXPORT  SSP0_RX_IRQHandler        [WEAK]
                EXPORT  SSP0_COMBINE_IRQHandler   [WEAK]
                EXPORT  GPDMA_COMBINE_IRQHandler  [WEAK]
                EXPORT  GPDMA_TC_IRQHandler       [WEAK]
                EXPORT  GPDMA_ERR_IRQHandler      [WEAK]
                EXPORT  FLASH_CACHE_IRQHandler    [WEAK]
                EXPORT  CC312_IRQHandler          [WEAK]
                EXPORT  PKI_RSA_IRQHandler        [WEAK]
                EXPORT  TIMER_IRQHandler          [WEAK]
                EXPORT  WDT_IRQHandler            [WEAK]
                EXPORT  I2C1_IRQHandler           [WEAK]
                EXPORT  I2C0_IRQHandler           [WEAK]
                EXPORT  UART0_COMBINE_IRQHandler  [WEAK]
                EXPORT  UART0_ERR_IRQHandler      [WEAK]
                EXPORT  UART0_RT_IRQHandler       [WEAK]
                EXPORT  UART0_TX_IRQHandler       [WEAK]
                EXPORT  UART0_RX_IRQHandler       [WEAK]
                EXPORT  UART0_MS_IRQHandler       [WEAK]
                EXPORT  GPIO0_IRQHandler          [WEAK]
                EXPORT  GPIO1_IRQHandler          [WEAK]
                EXPORT  GPIO2_IRQHandler          [WEAK]
                EXPORT  GPIO3_IRQHandler          [WEAK]
                EXPORT  GPIO4_IRQHandler          [WEAK]
                EXPORT  GPIO5_IRQHandler          [WEAK]
                EXPORT  GPIO6_IRQHandler          [WEAK]
                EXPORT  GPIO7_IRQHandler          [WEAK]
                EXPORT  GPIO_COMBINE_IRQHandler   [WEAK]
                EXPORT  eFLASH_IRQHandler         [WEAK]
                EXPORT  UART1_COMBINE_IRQHandler  [WEAK]
                EXPORT  UART1_ERR_IRQHandler      [WEAK]
                EXPORT  UART1_RT_IRQHandler       [WEAK]
                EXPORT  UART1_TX_IRQHandler       [WEAK]
                EXPORT  UART1_RX_IRQHandler       [WEAK]
                EXPORT  UART1_MS_IRQHandler       [WEAK]
                EXPORT  TRNG0_IRQHandler          [WEAK]
                EXPORT  TRNG1_IRQHandler          [WEAK]
                EXPORT  TRNG2_IRQHandler          [WEAK]
                EXPORT  TRNG3_IRQHandler          [WEAK]

TRNG_IRQHandler
SSP3_RT_IRQHandler
SSP3_ROR_IRQHandler
SSP3_TX_IRQHandler
SSP3_RX_IRQHandler
SSP3_COMBINE_IRQHandler
SSP2_RT_IRQHandler
SSP2_ROR_IRQHandler
SSP2_TX_IRQHandler
SSP2_RX_IRQHandler
SSP2_COMBINE_IRQHandler
SSP1_RT_IRQHandler
SSP1_ROR_IRQHandler
SSP1_TX_IRQHandler
SSP1_RX_IRQHandler
SSP1_COMBINE_IRQHandler
SSP0_RT_IRQHandler
SSP0_ROR_IRQHandler
SSP0_TX_IRQHandler
SSP0_RX_IRQHandler
SSP0_COMBINE_IRQHandler
GPDMA_COMBINE_IRQHandler
GPDMA_TC_IRQHandler
GPDMA_ERR_IRQHandler
FLASH_CACHE_IRQHandler
CC312_IRQHandler
PKI_RSA_IRQHandler
TIMER_IRQHandler
WDT_IRQHandler
I2C1_IRQHandler
I2C0_IRQHandler
UART0_COMBINE_IRQHandler
UART0_ERR_IRQHandler
UART0_RT_IRQHandler
UART0_TX_IRQHandler
UART0_RX_IRQHandler
UART0_MS_IRQHandler
GPIO0_IRQHandler
GPIO1_IRQHandler
GPIO2_IRQHandler
GPIO3_IRQHandler
GPIO4_IRQHandler
GPIO5_IRQHandler
GPIO6_IRQHandler
GPIO7_IRQHandler
GPIO_COMBINE_IRQHandler
eFLASH_IRQHandler
UART1_COMBINE_IRQHandler
UART1_ERR_IRQHandler
UART1_RT_IRQHandler
UART1_TX_IRQHandler
UART1_RX_IRQHandler
UART1_MS_IRQHandler
TRNG0_IRQHandler
TRNG1_IRQHandler
TRNG2_IRQHandler
TRNG3_IRQHandler
                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
