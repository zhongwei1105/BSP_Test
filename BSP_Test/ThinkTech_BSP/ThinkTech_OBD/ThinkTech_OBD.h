#ifndef _TKT_OBD_H
#define _TKT_OBD_H

#include <stdint.h>
#include <stdbool.h>
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;
typedef enum {Numeric = 0, Character} DataType;
typedef enum {SYSOUT = 0, SYSXX,SYSCFG,SYSDTC,SYSCDTC,SYSDS,SYSRDS,SYSSTOPDS,SYSVIN} MenuType;
typedef struct
{
    DataType	Type;
    uint8_t 	PIDByte;
    uint8_t 	FineByte;
    char 		*Format;
    float 		(*Equation0)(uint8_t* data);
    char* 		(*Equation1)(uint8_t* data);
} DSControlTypeDef;
typedef struct
{
    uint8_t    	flay;
    float 		result;
    char 		*string;
} DSSTypeDef;

extern ErrorStatus 	RxFlay;
extern MenuType CotrolVale;
extern uint8_t 	RxRAM[100];
extern char    	DTCRAM[200];
extern uint8_t	ATCmd[100];

char* PCBU(uint16_t dtc);
void ClearRAM(uint8_t* ram, uint32_t n);
uint8_t* Send_CANFrame(uint8_t *CAN_TX_Buf, ErrorStatus* err);
void ReadyDSStatus(uint8_t DSTotal,ErrorStatus* err);
char* ISO15765_4_DTC(uint8_t *CAN_TX_Buf, ErrorStatus* err);
char* ISO15765_4_DS(uint8_t *CAN_TX_Buf, ErrorStatus* err);
char* ISO15765_4_VIN(uint8_t *CAN_TX_Buf, ErrorStatus* err);
void ISO15765_4_Config(uint8_t CANStype, uint8_t velocity);
void ISO15765_4_Drv(void);

#endif
