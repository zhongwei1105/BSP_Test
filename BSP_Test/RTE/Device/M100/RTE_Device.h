/* -------------------------------------------------------------------------- 
 * Copyright (c) 2017-2019 ThinkTech Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        02. Dec 2017
 * $Revision:    V1.0
 *
 * Project:      RTE Device Configuration for ThinkTech M100
 * -------------------------------------------------------------------------- */

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H


//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//   <e> USART0 (Universal synchronous asynchronous receiver transmitter) [Driver_USART0]
//   <i> Configuration settings for Driver_USART0 in component ::CMSIS Driver:USART
#define RTE_Drivers_UART0               1

//     <o>Transmit trigger level
//       <0=> USART_TRIG_LVL_18
//       <1=> USART_TRIG_LVL_14
//       <2=> USART_TRIG_LVL_12
//       <3=> USART_TRIG_LVL_34
//       <4=> USART_TRIG_LVL_78
//     <i> Default: USART_TRIG_LVL_18
#define   UART0_TX_TRIG_LVL             0

//     <o>Receiver trigger level
//       <0=> USART_TRIG_LVL_18
//       <1=> USART_TRIG_LVL_14
//       <2=> USART_TRIG_LVL_12
//       <3=> USART_TRIG_LVL_34
//       <4=> USART_TRIG_LVL_78
//     <i> Default: USART_TRIG_LVL_34
#define   UART0_RX_TRIG_LVL             3

//     <q>Enable RTS
//     <i> Enable RTS
#define   RTE_UART0_PIN_RTS             0

//     <q>Enable CTS
//     <i> Enable CTS
#define   RTE_UART0_PIN_CTS             0

//     <q>Select UART0 as console output
//     <i> Select UART0 as console output
#define   RTE_UART0_CONSOLE             0

//   </e>

//   <e> USART1 (Universal synchronous asynchronous receiver transmitter) [Driver_USART1]
//   <i> Configuration settings for Driver_USART1 in component ::CMSIS Driver:USART
#define RTE_Drivers_UART1               1

//     <o>Transmit trigger level
//       <0=> USART_TRIG_LVL_18
//       <1=> USART_TRIG_LVL_14
//       <2=> USART_TRIG_LVL_12
//       <3=> USART_TRIG_LVL_34
//       <4=> USART_TRIG_LVL_78
//     <i> Default: USART_TRIG_LVL_18
#define   UART1_TX_TRIG_LVL             0

//     <o>Receiver trigger level
//       <0=> USART_TRIG_LVL_18
//       <1=> USART_TRIG_LVL_14
//       <2=> USART_TRIG_LVL_12
//       <3=> USART_TRIG_LVL_34
//       <4=> USART_TRIG_LVL_78
//     <i> Default: USART_TRIG_LVL_34
#define   UART1_RX_TRIG_LVL             3

//     <q>Enable RTS
//     <i> Enable RTS
#define   RTE_UART1_PIN_RTS             0

//     <q>Enable CTS
//     <i> Enable CTS
#define   RTE_UART1_PIN_CTS             0

//     <q>Select UART1 as console output
//     <i> Select UART1 as console output
#define   RTE_UART1_CONSOLE             1

//   </e>

//   <q>Enable UART console poll mode
//   <i> Enable UART console poll mode
#define RTE_UART_CONSOLE_POLL           1

//   <q> SSP0 (Synchronous Serial Port) [Driver_SSP0]
//   <i> Select Driver_SSP0 in component ::CMSIS Driver:SSP
#define RTE_Drivers_SSP0                1

//   <q> SSP1 (Synchronous Serial Port) [Driver_SSP1]
//   <i> Select Driver_SSP1 in component ::CMSIS Driver:SSP
#define RTE_Drivers_SSP1                1

//   <q> SSP2 (Synchronous Serial Port) [Driver_SSP2]
//   <i> Select Driver_SSP2 in component ::CMSIS Driver:SSP
#define RTE_Drivers_SSP2                1

//   <q> SSP3 (Synchronous Serial Port) [Driver_SSP3]
//   <i> Select Driver_SSP3 in component ::CMSIS Driver:SSP
#define RTE_Drivers_SSP3                1

// <e>GPDMA Configuration
// <i>GPDMA Configuration
// =======================
#define RTE_Drivers_GPDMA               1

#define   RTE_GPDMA_CHANNEL_UART0_TX    0   /* UART0 transmit */
#define   RTE_GPDMA_CHANNEL_UART0_RX    1   /* UART0 receive  */
#define   RTE_GPDMA_CHANNEL_I2C0_TX     2   /* I2C0 transmit */
#define   RTE_GPDMA_CHANNEL_I2C0_RX     3   /* I2C0 receive  */
#define   RTE_GPDMA_CHANNEL_I2C1_TX     4   /* I2C1 transmit */
#define   RTE_GPDMA_CHANNEL_I2C1_RX     5   /* I2C1 receive  */
#define   RTE_GPDMA_CHANNEL_UART1_TX    6   /* UART1 transmit */
#define   RTE_GPDMA_CHANNEL_UART1_RX    7   /* UART1 receive  */
#define   RTE_GPDMA_CHANNEL_SSP0_TX     0   /* SSP0 transmit */
#define   RTE_GPDMA_CHANNEL_SSP0_RX     1   /* SSP0 receive  */
#define   RTE_GPDMA_CHANNEL_SSP1_TX     2   /* SSP1 transmit */
#define   RTE_GPDMA_CHANNEL_SSP1_RX     3   /* SSP1 receive  */
#define   RTE_GPDMA_CHANNEL_SSP2_TX     4   /* SSP2 transmit */
#define   RTE_GPDMA_CHANNEL_SSP2_RX     5   /* SSP2 receive  */
#define   RTE_GPDMA_CHANNEL_SSP3_TX     6   /* SSP3 transmit */
#define   RTE_GPDMA_CHANNEL_SSP3_RX     7   /* SSP3 receive  */

#define   RTE_UART0_DMA_TX_CH           RTE_GPDMA_CHANNEL_UART0_TX
#define   RTE_UART0_DMA_RX_CH           RTE_GPDMA_CHANNEL_UART0_RX
#define   RTE_UART1_DMA_TX_CH           RTE_GPDMA_CHANNEL_UART1_TX
#define   RTE_UART1_DMA_RX_CH           RTE_GPDMA_CHANNEL_UART1_RX
#define   RTE_I2C0_DMA_TX_CH            RTE_GPDMA_CHANNEL_I2C0_TX
#define   RTE_I2C0_DMA_RX_CH            RTE_GPDMA_CHANNEL_I2C0_RX
#define   RTE_I2C1_DMA_TX_CH            RTE_GPDMA_CHANNEL_I2C1_TX
#define   RTE_I2C1_DMA_RX_CH            RTE_GPDMA_CHANNEL_I2C1_RX
#define   RTE_SSP0_DMA_TX_CH            RTE_GPDMA_CHANNEL_SSP0_TX
#define   RTE_SSP0_DMA_RX_CH            RTE_GPDMA_CHANNEL_SSP0_RX
#define   RTE_SSP1_DMA_TX_CH            RTE_GPDMA_CHANNEL_SSP1_TX
#define   RTE_SSP1_DMA_RX_CH            RTE_GPDMA_CHANNEL_SSP1_RX
#define   RTE_SSP2_DMA_TX_CH            RTE_GPDMA_CHANNEL_SSP2_TX
#define   RTE_SSP2_DMA_RX_CH            RTE_GPDMA_CHANNEL_SSP2_RX
#define   RTE_SSP3_DMA_TX_CH            RTE_GPDMA_CHANNEL_SSP3_TX
#define   RTE_SSP3_DMA_RX_CH            RTE_GPDMA_CHANNEL_SSP3_RX

#define   RTE_GPDMA_PERISEL_UART0_TX    0   /* UART0 transmit */
#define   RTE_GPDMA_PERISEL_UART0_RX    1   /* UART0 receive  */
#define   RTE_GPDMA_PERISEL_I2C0_TX     2   /* I2C0 transmit */
#define   RTE_GPDMA_PERISEL_I2C0_RX     3   /* I2C0 receive  */
#define   RTE_GPDMA_PERISEL_I2C1_TX     4   /* I2C1 transmit */
#define   RTE_GPDMA_PERISEL_I2C1_RX     5   /* I2C1 receive  */
#define   RTE_GPDMA_PERISEL_UART1_TX    6   /* UART1 transmit */
#define   RTE_GPDMA_PERISEL_UART1_RX    7   /* UART1 receive  */
#define   RTE_GPDMA_PERISEL_SSP0_TX     8   /* SSP0 transmit */
#define   RTE_GPDMA_PERISEL_SSP0_RX     9   /* SSP0 receive  */
#define   RTE_GPDMA_PERISEL_SSP1_TX    10   /* SSP1 transmit */
#define   RTE_GPDMA_PERISEL_SSP1_RX    11   /* SSP1 receive  */
#define   RTE_GPDMA_PERISEL_SSP2_TX    12   /* SSP2 transmit */
#define   RTE_GPDMA_PERISEL_SSP2_RX    13   /* SSP2 receive  */
#define   RTE_GPDMA_PERISEL_SSP3_TX    14   /* SSP3 transmit */
#define   RTE_GPDMA_PERISEL_SSP3_RX    15   /* SSP3 receive  */

//   <q> Enable UART0 DMA Mode for Transmit
//   <i> Enable UART0 DMA Mode for Transmit
#define   RTE_UART0_DMA_TX_EN           0

//   <q> Enable UART0 DMA Mode for Receiver
//   <i> Enable UART0 DMA Mode for Receiver
#define   RTE_UART0_DMA_RX_EN           0

//   <q> Enable UART1 DMA Mode for Transmit
//   <i> Enable UART1 DMA Mode for Transmit
#define   RTE_UART1_DMA_TX_EN           0

//   <q> Enable UART1 DMA Mode for Receiver
//   <i> Enable UART1 DMA Mode for Receiver
#define   RTE_UART1_DMA_RX_EN           0

//   <q> Enable I2C0 DMA Mode for Transmit
//   <i> Enable I2C0 DMA Mode for Transmit
#define   RTE_I2C0_DMA_TX_EN            0

//   <q> Enable I2C0 DMA Mode for Receiver
//   <i> Enable I2C0 DMA Mode for Receiver
#define   RTE_I2C0_DMA_RX_EN            0

//   <q> Enable I2C1 DMA Mode for Transmit
//   <i> Enable I2C1 DMA Mode for Transmit
#define   RTE_I2C1_DMA_TX_EN            0

//   <q> Enable I2C1 DMA Mode for Receiver
//   <i> Enable I2C1 DMA Mode for Receiver
#define   RTE_I2C1_DMA_RX_EN            0

//   <q> Enable SSP0 DMA Mode for Transmit
//   <i> Enable SSP0 DMA Mode for Transmit
#define   RTE_SSP0_DMA_TX_EN            0

//   <q> Enable SSP0 DMA Mode for Receiver
//   <i> Enable SSP0 DMA Mode for Receiver
#define   RTE_SSP0_DMA_RX_EN            0

//   <q> Enable SSP1 DMA Mode for Transmit
//   <i> Enable SSP1 DMA Mode for Transmit
#define   RTE_SSP1_DMA_TX_EN            1

//   <q> Enable SSP1 DMA Mode for Receiver
//   <i> Enable SSP1 DMA Mode for Receiver
#define   RTE_SSP1_DMA_RX_EN            1

//   <q> Enable SSP2 DMA Mode for Transmit
//   <i> Enable SSP2 DMA Mode for Transmit
#define   RTE_SSP2_DMA_TX_EN            1

//   <q> Enable SSP2 DMA Mode for Receiver
//   <i> Enable SSP2 DMA Mode for Receiver
#define   RTE_SSP2_DMA_RX_EN            1

//   <q> Enable SSP3 DMA Mode for Transmit
//   <i> Enable SSP3 DMA Mode for Transmit
#define   RTE_SSP3_DMA_TX_EN            1

//   <q> Enable SSP3 DMA Mode for Receiver
//   <i> Enable SSP3 DMA Mode for Receiver
#define   RTE_SSP3_DMA_RX_EN            1

// </e>

//------------- <<< end of configuration section >>> ---------------------------

#endif  /* __RTE_DEVICE_H */
