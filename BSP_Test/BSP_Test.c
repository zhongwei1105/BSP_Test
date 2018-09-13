/*----------------------------------------------------------------------------
 * Name:    SpiMasterMain.c
 * Purpose: SSP verification program 2 for M100 ASIC device
 *----------------------------------------------------------------------------
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
 *----------------------------------------------------------------------------*/
 
#include <stdio.h> 
#include <Mizar.h>
#include "ThinkTech_Timer.h" 
#include "ThinkTech_key.h"
#include "ThinkTech_MCP2515.h" 
#include "ThinkTech_cmd.h" 
#include "ThinkTech_OBD.h"  

int main (void)
{
	SystemCoreClockUpdate();
	
	mdelay(10);
	
	printf("\rProcessor work at %dMHz\n", SystemCoreClock/1000/1000);
	printf("OBD_ISO15765-4_Test\r\n");
	printf("Please Send \"AT+HELP\"\r\n");
		
//	TMR_Open(TMR_10MS_Base, true, TMR_10MS_Base_TIME);
	
	while(1)
	{
//		if(TMR_IsOut(TMR_10MS_Base) == true)	
//		{
//			TMR_Open(TMR_10MS_Base, true, TMR_10MS_Base_TIME);
//
//		}		
		
		CMD_Drv();
		ISO15765_4_Drv();
	}
}
