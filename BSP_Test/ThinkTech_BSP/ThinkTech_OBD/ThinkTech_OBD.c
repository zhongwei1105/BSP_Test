#include "ThinkTech_OBD.h"  
#include "Formula.h" 
#include "system_M100.h"
#include "ThinkTech_MCP2515.h" 

ErrorStatus 	RxFlay;
MenuType 		CotrolVale;
DSSTypeDef   	DSStatus[266];
uint8_t 		TxRAM[100];		// CAN�������ݻ���
uint8_t 		RxRAM[100];		// CAN�������ݻ���
uint8_t 		RxRAMBuff[100];	// CAN�������ݻ���
char    		DTCRAM[200];	// ���������ݻ���
char         	DSRAM[100];		// ���������ݻ���
char        	VINRAM[30];		// VIN���ݻ���
char 			PCBUCode[5];
uint8_t			ATCmd[100];		// ���ڽ���ATָ���

/************************************************************************
  * @����:  PCBU���ASCIIת��
  * @����:   dtc uint16_t������
  * @����ֵ: char*  PCBU��ASCII��
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
  * @����:  ����ڴ�
  * @����:   uint8_t* ram ��Ҫ��յ��ڴ�ָ�� uint32_t n ����ڴ�Ĵ�С
  * @����ֵ: NONE
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
  * @����:  ����һ֡CAN����
  * @����:  TxBuf����Ҫͨ��CAN���͵�����
			err���������ݳɹ���־
  * @����ֵ: ���յ�������
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

    for (k = 0; k < 10000000/3; k++)	//��ŵȴ�1.5��
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
  * @����:  ISO15765-4 ��ַЭ�麯������������� ISO15765-4 ��ַЭ�鹦��
  * @����:  NONE
  * @����ֵ: NONE
  **********************************************************************/
void ISO15765_4_Drv(void)
{
	ErrorStatus err;
	char* dtcram,*vinram;
	uint8_t i;
	
	switch (CotrolVale)
	{
		case SYSCFG:	/*ϵͳ����*/
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
			
		case SYSCDTC:	/*�������*/
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
			
		case SYSDTC:	/*��������*/
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
				if (strlen(dtcram) != 0) //������ڴ�û�д�ֵ ˵��û�ж���������
				{
					printf(dtcram);   //�򴮿���� ������
					printf("\r\n");
				}
				else
				{
					printf("NO DTC\r\n");  //�������� û�й��ϼ�¼
				}
				CotrolVale = SYSXX;	
			}
			else
			{
				printf("Link Break\r\n");	//��ֿ��ǵ���ȡDTC�����У����ⷢ�����¶Ͽ����ӡ�
				CotrolVale = SYSOUT;
			}
		break;
			
		case SYSDS:		/*������*/
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

		case SYSRDS:	/*������*/
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
		
		case SYSVIN:	/*��VIN��*/
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
  * @����:  ISO15765 ��ȡ��ǰ������
  * @����:  ErrorStatus* err
  * @����ֵ: char* ������ŵ�ַ										 19
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
  * @����:  ���DSPID���飬ΪҪ�������������
  * @����:  NONE
  * @����ֵ: NONE
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
    for (i = 0; i < (strlen((char*)ATCmd)-2)/6; i++)                              //Ԥ��������
    {
        n = (ATCmd[5+i*6]-0x30)*100+(ATCmd[6+i*6]-0x30)*10+ATCmd[7+i*6]-0x30;
        if (n < DSTotal)
        {
            DSStatus[n].flay = 0x01; //�������01��ʾ�ͻ���Ҫ����Ӧ��������
        }
        else
        {
            *err = ERROR;
            break;
        }
    }
}
/************************************************************************
  * @����:  ISO15765 ��ʵʱ������
  * @����:  CAN_TX_Buf:���������� ErrorStatus* err
  * @����ֵ: char* �����������ַ
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
                DSStatus[i].flay = 0xff;               //FF��ʾ������������֧��
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
    *err = SUCCESS; //������
    return (char*)DSRAM;
}
/************************************************************************
  * @����:  ISO15765 ��VIN��
  * @����:  CAN_TX_Buf:���ܺ����� ErrorStatus* err
  * @����ֵ: char* vin������ַ
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




