#include "ThinkTech_OBD.h"  
#include "Formula.h" 
#include "system_M100.h"
#include "ThinkTech_MCP2515.h" 

ErrorStatus 	RxFlay;
MenuType 		CotrolVale;
DSSTypeDef   	DSStatus[266];
uint8_t 		TxRAM[100];		// CAN发送数据缓存
uint8_t 		RxRAM[100];		// CAN接收数据缓存
uint8_t 		RxRAMBuff[100];	// CAN接收数据缓存
char    		DTCRAM[200];	// 错误流数据缓存
char         	DSRAM[100];		// 数据流数据缓存
char        	VINRAM[30];		// VIN数据缓存
char 			PCBUCode[5];
uint8_t			ATCmd[100];		// 串口接收AT指令缓存

/************************************************************************
  * @描述:  PCBU码的ASCII转换
  * @参数:   dtc uint16_t的数据
  * @返回值: char*  PCBU的ASCII码
  **********************************************************************/
char* PCBU(uint16_t dtc)
{
    if ( dtc > 0 && dtc < 0x4000)
    {
        strcpy((char*)PCBUCode,"P");
        sprintf((char*)(PCBUCode + strlen((char*)PCBUCode)),"%04x;",dtc);
    }
    else if ( dtc > 0x4000 && dtc < 0x8000)
    {
        strcpy((char*)PCBUCode,"C");
        sprintf((char*)(PCBUCode + strlen((char*)PCBUCode)),"%04x;",dtc - 0x4000);
    }
    else if (dtc > 0x8000 && dtc < 0xc000)
    {
        strcpy((char*)PCBUCode,"B");
        sprintf((char*)(PCBUCode + strlen((char*)PCBUCode)),"%04x;",dtc - 0x8000);
    }
    else if (dtc > 0xc000 && dtc < 0xffff)
    {
        strcpy((char*)PCBUCode,"U");
        sprintf((char*)(PCBUCode + strlen((char*)PCBUCode)),"%04x;",dtc - 0xc000);
    }
    return &PCBUCode[0];
}
/************************************************************************
  * @描述:  清空内存
  * @参数:   uint8_t* ram 需要清空的内存指针 uint32_t n 清空内存的大小
  * @返回值: NONE
  **********************************************************************/
void ClearRAM(uint8_t* ram, uint32_t n)
{
    uint32_t i;
    for (i = 0; i < n; i++)
    {
        ram[i] = 0x00;
    }
}
/************************************************************************
  * @描述:  发送一帧CAN数据
  * @参数:  TxBuf：需要通过CAN发送的数据
			err：接收数据成功标志
  * @返回值: 接收到的数据
  **********************************************************************/
uint8_t* Send_CANFrame(uint8_t *TxBuf, ErrorStatus* err)
{
	uint8_t len=0;
	uint32_t k;
	uint8_t i,j;
	uint8_t FI = 0;
	uint8_t FLCAN = 0;
	uint8_t Rceive_ID_Buff[4];
	uint16_t Std_Send_ID_Buff;
	uint32_t Ext_Send_ID_Buff;
	uint16_t buff,buff1=0;
	uint8_t buff2=0;
    RxFlay = ERROR;

    ClearRAM(RxRAM, 100);
	CAN_Send_Buffer(TxBuf, 8);

    for (k = 0; k < 10000000/3; k++)	//大概等待1.5秒
    {	
		len = CAN_Receive_Buffer(RxRAMBuff);
		if (len == 8) 
		{
			if(RxRAMBuff[0] == 0x10)
			{
				Rceive_ID_Buff[0] = MCP2515_ReadByte(RXB0SIDH);
				buff2 = Rceive_ID_Buff[1] = MCP2515_ReadByte(RXB0SIDL);
				Rceive_ID_Buff[2] = MCP2515_ReadByte(RXB0EID8);
				Rceive_ID_Buff[3] = MCP2515_ReadByte(RXB0EID0);
				
				if(buff2 & 0x80)
				{
					Rceive_ID_Buff[1] = (Rceive_ID_Buff[1] & 0xE0) | ((Rceive_ID_Buff[1]&0x03)<<3);
					Ext_Send_ID_Buff = (uint16_t)Rceive_ID_Buff[0]<<8 | Rceive_ID_Buff[1];
					Ext_Send_ID_Buff = (Ext_Send_ID_Buff<<13) | ((uint16_t)Rceive_ID_Buff[2]<<8 | Rceive_ID_Buff[3]);
					Ext_Send_ID_Buff = Ext_Send_ID_Buff - 0xE01F;
									
					buff = (Ext_Send_ID_Buff>>16)<<3;
					buff1 = (buff&0xE0)|((buff&0x18)>>3);
					buff1 = buff1 | 0x08;
					
					MCP2515_WriteByte(TXB0SIDH,(uint8_t)(buff>>8));
					MCP2515_WriteByte(TXB0SIDL,(uint8_t)buff1);
					MCP2515_WriteByte(TXB0EID8,(uint8_t)(Ext_Send_ID_Buff>>8));
					MCP2515_WriteByte(TXB0EID0,(uint8_t)Ext_Send_ID_Buff);
				}
				else
				{
					Std_Send_ID_Buff = (uint16_t)Rceive_ID_Buff[0]<<8 | Rceive_ID_Buff[1];
					Std_Send_ID_Buff = Std_Send_ID_Buff>>5;
					Std_Send_ID_Buff = Std_Send_ID_Buff -8;
					Std_Send_ID_Buff = Std_Send_ID_Buff<<5;
					
					MCP2515_WriteByte(TXB0SIDH,(uint8_t)(Std_Send_ID_Buff>>8));
					MCP2515_WriteByte(TXB0SIDL,(uint8_t)Std_Send_ID_Buff);
				}				
							
				TxRAM[0] = 0x30;
				TxRAM[1] = 0x00;
				TxRAM[2] = 0x00;
				TxRAM[3] = 0x00;
				TxRAM[4] = 0x00;
				TxRAM[5] = 0x00;
				TxRAM[6] = 0x00;
				TxRAM[7] = 0x00;
				CAN_Send_Buffer(TxRAM, 8);
				
				if ((RxRAMBuff[1]+1)%7 > 0)
				{
					FLCAN = (RxRAMBuff[1]+1)/7 + 1;
				}
				else
				{
					FLCAN = (RxRAMBuff[1]+1)/7;
				}
				FI = 0;
			}
			for(i = 0; i < 8; i++)
			{
				RxRAM[i+FI*8] = RxRAMBuff[i];
			}
			if (FLCAN	!= 0)
			{
				FI++;
				if (FLCAN  <= FI)
				{
					FI = 0;
					for (j = 0; j < FLCAN; j++)
					{
						for (i = 0; i < 7; i++)
						{
							RxRAM[i+7*j] = RxRAM[i+1+7*j+j];
						}
					}
					RxFlay = SUCCESS;
					FLCAN = 0;
					
					if(buff2 & 0x80)
					{
						buff = (Ext_Send_ID>>16)<<3;
						buff1 = (buff&0xE0)|((buff&0x18)>>3);
						buff1 = buff1 | 0x08;
						
						MCP2515_WriteByte(TXB0SIDH,(uint8_t)(buff>>8));
						MCP2515_WriteByte(TXB0SIDL,(uint8_t)buff1);
						MCP2515_WriteByte(TXB0EID8,(uint8_t)(Ext_Send_ID>>8));
						MCP2515_WriteByte(TXB0EID0,(uint8_t)Ext_Send_ID);
					}
					else
					{
						MCP2515_WriteByte(TXB0SIDH,(uint8_t)((Std_Send_ID<<5)>>8));
						MCP2515_WriteByte(TXB0SIDL,(uint8_t)(Std_Send_ID<<5));
					}
					
					break;
				}
			}
			else
			{
				RxFlay = SUCCESS;
				break;
			}
		}
    }
    *err = RxFlay;
    return (uint8_t*)&RxRAM[0];
}
void ISO15765_4_Config(uint8_t CANStype, uint8_t velocity)
{
	MCP2515_Init(CANStype, velocity);
}
/************************************************************************
  * @描述:  ISO15765-4 地址协议函数，调用它完成 ISO15765-4 地址协议功能
  * @参数:  NONE
  * @返回值: NONE
  **********************************************************************/
void ISO15765_4_Drv(void)
{
	ErrorStatus err;
	char* dtcram,*vinram;
	uint8_t i;
	
	switch (CotrolVale)
	{
		case SYSCFG:	/*系统激活*/
			TxRAM[0] = 0x02;
			TxRAM[1] = 0x01;
			TxRAM[2] = 0x00;
			TxRAM[3] = 0x00;
			TxRAM[4] = 0x00;
			TxRAM[5] = 0x00;
			TxRAM[6] = 0x00;
			TxRAM[7] = 0x00;
			Send_CANFrame(TxRAM, &err);
			if (err == SUCCESS)
			{
				printf("LINK OK\r\n");
				for (i = 0; i < 8; i++) 
				{
					printf("Rx_Buf[%d] = 0x%x\n", i, RxRAM[i]);
				}
				CotrolVale = SYSXX;
			}
			else if (err == ERROR)
			{
				printf("LINK FAIL\r\n");
				CotrolVale = SYSOUT;				
			}
		break;
			
		case SYSCDTC:	/*清故障码*/
			TxRAM[0] = 0x01;
			TxRAM[1] = 0x04;
			TxRAM[2] = 0x00;
			TxRAM[3] = 0x00;
			TxRAM[4] = 0x00;
			TxRAM[5] = 0x00;
			TxRAM[6] = 0x00;
			TxRAM[7] = 0x00;
			Send_CANFrame(TxRAM, &err);
			if (err == ERROR)
			{
				printf("Link Break\r\n");
				CotrolVale = SYSOUT;
			}
			mdelay(500);
		break;
			
		case SYSDTC:	/*读故障码*/
			TxRAM[0] = 0x01;
			TxRAM[1] = 0x03;
			TxRAM[2] = 0x00;
			TxRAM[3] = 0x00;
			TxRAM[4] = 0x00;
			TxRAM[5] = 0x00;
			TxRAM[6] = 0x00;
			TxRAM[7] = 0x00;
			dtcram = ISO15765_4_DTC(TxRAM, &err);
			if (err == SUCCESS)
			{
				if (strlen(dtcram) != 0) //如果该内存没有存值 说明没有读到故障码
				{
					printf(dtcram);   //向串口输出 读码结果
					printf("\r\n");
				}
				else
				{
					printf("NO DTC\r\n");  //车子正常 没有故障记录
				}
				CotrolVale = SYSXX;	
			}
			else
			{
				printf("Link Break\r\n");	//充分考虑到读取DTC过程中，意外发生导致断开连接。
				CotrolVale = SYSOUT;
			}
		break;
			
		case SYSDS:		/*数据流*/
			ReadyDSStatus(DSTotalX,&err);
			if (err == SUCCESS)
			{
				printf("READING\r\n");
				CotrolVale = SYSRDS;
			}
			else
			{
				printf("ERROR-001\r\n");
				CotrolVale = SYSXX;		
			}
			ClearRAM(ATCmd,100);
		break;

		case SYSRDS:	/*数据流*/
			TxRAM[0] = 0x02;
			TxRAM[1] = 0x01;
			TxRAM[2] = 0x00;
			TxRAM[3] = 0x00;
			TxRAM[4] = 0x00;
			TxRAM[5] = 0x00;
			TxRAM[6] = 0x00;
			TxRAM[7] = 0x00;
			ISO15765_4_DS(TxRAM, &err);
//			if (err == SUCCESS)
//			{
//				CotrolVale = SYSOUT;
//			}
		break;
		
		case SYSVIN:	/*读VIN码*/
			TxRAM[0] = 0x02;
			TxRAM[1] = 0x09;
			TxRAM[2] = 0x02;
			TxRAM[3] = 0x00;
			TxRAM[4] = 0x00;
			TxRAM[5] = 0x00;
			TxRAM[6] = 0x00;
			TxRAM[7] = 0x00;
			vinram = ISO15765_4_VIN(TxRAM, &err);
			if (err == SUCCESS)
			{
				printf(vinram);
				printf("\r\n");
			}
			else
			{
				printf("Not support\r\n");
			}
			CotrolVale = SYSXX;
		break;
			
		case SYSSTOPDS:
			printf("STOPDS\r\n");
			CotrolVale = SYSXX;			
		break;	
		
		case SYSXX:	
			break;
			
		case SYSOUT:	
			break;
		default:
			break;
	}
}
/************************************************************************
  * @描述:  ISO15765 读取当前故障码
  * @参数:  ErrorStatus* err
  * @返回值: char* 故障码号地址										 19
  **********************************************************************/
char* ISO15765_4_DTC(uint8_t *CAN_TX_Buf, ErrorStatus* err)
{
    uint8_t *ram,i;
    uint16_t dtc;
    ClearRAM(DTCRAM,200);
    ram = Send_CANFrame(CAN_TX_Buf, err);
    if (*err == SUCCESS)
    {
        for(i = 0; i < ram[2]; i++)
        {
            dtc = (uint16_t)(*(ram+3+2*i)<<8 | *(ram+4+2*i));
            if (dtc != 0)
            {
                strcpy((char*)(DTCRAM+strlen((char*)DTCRAM)),PCBU(dtc));
            }
        }
    }
    return (char*)DTCRAM;
}
/************************************************************************
  * @描述:  标记DSPID数组，为要读的数据流标记
  * @参数:  NONE
  * @返回值: NONE
  **********************************************************************/
void ReadyDSStatus(uint8_t DSTotal,ErrorStatus* err)
{
    uint8_t i;
    uint32_t n;
    *err = SUCCESS;
    for(i = 0; i < DSTotal; i++)
    {
        DSStatus[i].flay = 0;
    }
    for (i = 0; i < (strlen((char*)ATCmd)-2)/6; i++)                              //预读数据流
    {
        n = (ATCmd[5+i*6]-0x30)*100+(ATCmd[6+i*6]-0x30)*10+ATCmd[7+i*6]-0x30;
        if (n < DSTotal)
        {
            DSStatus[n].flay = 0x01; //该数组存01表示客户需要读对应的数据流
        }
        else
        {
            *err = ERROR;
            break;
        }
    }
}
/************************************************************************
  * @描述:  ISO15765 读实时数据流
  * @参数:  CAN_TX_Buf:数据流命令 ErrorStatus* err
  * @返回值: char* 数据流结果地址
  **********************************************************************/
char* ISO15765_4_DS(uint8_t *CAN_TX_Buf, ErrorStatus* err)
{
	uint8_t i;
    uint8_t *ram;

    for (i = 0; i < DSTotalX; i++)
    {
        if (DSStatus[i].flay == 1)
        {
            CAN_TX_Buf[2] = DSControl[i].PIDByte;
            ram = Send_CANFrame(CAN_TX_Buf, err);
            if (*err == SUCCESS)
            {
                if (DSControl[i].Type == Numeric)
                {
                    DSStatus[i].result = DSControl[i].Equation0(ram + DSControl[i].FineByte);
                }
                else
                {
                    DSStatus[i].string = DSControl[i].Equation1(ram + DSControl[i].FineByte);
                }
            }
            else
            {
                DSStatus[i].flay = 0xff;               //FF表示该项数据流不支持
            }
            mdelay(150);
        }
    }
    DSRAM[0]=0x00;
    for (i = 0; i < DSTotalX; i++)
    {
        if (DSStatus[i].flay == 1)
        {
            sprintf((char*)(DSRAM+strlen((char*)DSRAM)),"DS%03d:",i);
            if (DSControl[i].Type == Numeric)
            {
                sprintf((char*)(DSRAM+strlen((char*)DSRAM)),(char*)DSControl[i].Format,DSStatus[i].result);
            }
            else
            {
                strcpy((char*)(DSRAM+strlen((char*)DSRAM)),(char*)DSStatus[i].string);
            }
        }
        else if(DSStatus[i].flay == 0xff)
        {
            sprintf((char*)(DSRAM+strlen((char*)DSRAM)),"DS%03d:",i);
            strcpy((char*)(DSRAM+strlen((char*)DSRAM)),"---;");
        }
    }
    if (CotrolVale == SYSRDS)
    {
		printf((char*)DSRAM);
		printf("\r\n");
		mdelay(100);
    }
    *err = SUCCESS; //不采用
    return (char*)DSRAM;
}
/************************************************************************
  * @描述:  ISO15765 读VIN码
  * @参数:  CAN_TX_Buf:车架号命令 ErrorStatus* err
  * @返回值: char* vin码结果地址
  **********************************************************************/
char* ISO15765_4_VIN(uint8_t *CAN_TX_Buf, ErrorStatus* err)
{
	uint8_t* ram;

	ClearRAM(VINRAM,30);
	ram = Send_CANFrame(CAN_TX_Buf, err);
	if (*err == SUCCESS)
    {
		strncpy((char*)VINRAM,(const char*)ram+4,17);			
	}
    return (char*)VINRAM;
}




