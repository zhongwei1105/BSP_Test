/****************************************************************************************
**  Filename :  ThinkTech_MCP2515.c
**  Abstract :  This file implements key function.
**  By       :  ��ΰ
**  Date     :  2018-08-13 
**  Changelog:1.First Create
*****************************************************************************************/
/*****************************************************************************************
									Includes
*****************************************************************************************/
#include <stdio.h>
#include <Mizar.h>
#include "Driver_SPI.h"
#include "ThinkTech_MCP2515.h" 
/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
extern ARM_DRIVER_SPI Driver_SPI0;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI0;
uint8_t spi_flag=0;
uint8_t CAN_flag=0;
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/
/*****************************************************************************************
* Function Name: SPI_callback_func
* Description  : SPI�жϻص�����
* Arguments    : e:�ж��¼�
* Return Value : NONE
******************************************************************************************/
void SPI_callback_func (uint32_t event)
{
	switch(event)
	{
		case ARM_SPI_EVENT_TRANSFER_COMPLETE:
			// Success
			spi_flag = 1;
			break;
		case ARM_SPI_EVENT_DATA_LOST:
			// Occurs in slave mode when data is requested/sent by master
			// but send/receive/transfer operation has not been started
			// and indicates that data is lost.
			// Occurs also in master mode when driver cannot transfer data
			// fast enough.

			break;
		case ARM_SPI_EVENT_MODE_FAULT:
			// Occurs in master mode when Slave Select is deactivated and
			// indicates Master Mode Fault.
			break;
	}
}
/*****************************************************************************************
* Function Name: SPI_ReadByte
* Description  : ͨ��SPI��ȡһ���ֽ�����
* Arguments    : NONE
* Return Value : rByte(��ȡ����һ���ֽ�����)
******************************************************************************************/
unsigned char SPI_ReadByte(void)
{
	uint8_t rByte=0;
	uint32_t time_out=0;
	
	spi_flag = 0;
	time_out = 0xffff;
	SPIdrv->Receive(&rByte, 1);
	while(spi_flag==0)
	{
		time_out -= 1;
		if(time_out == 0)
		break;
	}
	
	return rByte;
}
/*****************************************************************************************
* Function Name: SPI_SendByte
* Description  : SPI����һ���ֽ�����
* Arguments    : dt:�����͵�����
* Return Value : NONE
******************************************************************************************/
void SPI_SendByte(unsigned char dt)
{
	uint32_t time_out=0;
	
	spi_flag = 0;
	time_out = 0xffff;
	SPIdrv->Send(&dt, 1);
	while(spi_flag==0)
	{
		time_out -= 1;
		if(time_out == 0)
		break;
	}	
}
/*****************************************************************************************
* Function Name: MCP2515_WriteByte
* Description  : ͨ��SPI��MCP2515ָ����ַ�Ĵ���д1���ֽ�����
* Arguments    : addr:MCP2515�Ĵ�����ַ
								 dat:��д�������
* Return Value : NONE
******************************************************************************************/
void MCP2515_WriteByte(unsigned char addr,unsigned char dat)
{
	uint8_t tx_buff[3];
	uint8_t rx_buff[3];
	uint32_t time_out=0;
	
	tx_buff[0] = CAN_WRITE;
	tx_buff[1] = addr;
	tx_buff[2] = dat;
	spi_flag = 0;
	time_out = 0xffff;
	SPIdrv->Transfer(&tx_buff, &rx_buff, 3);
	while(spi_flag==0)
	{
		time_out -= 1;
		if(time_out == 0)
		break;
	}	
}
/*****************************************************************************************
* Function Name: MCP2515_ReadByte
* Description  : ͨ��SPI��MCP2515ָ����ַ������1���ֽ�����
* Arguments    : addr:MCP2515�Ĵ�����ַ
* Return Value : rByte:��ȡ���Ĵ�����1���ֽ�����
******************************************************************************************/
unsigned char MCP2515_ReadByte(unsigned char addr)
{
	uint8_t tx_buff[3];
	uint8_t rx_buff[3];
	uint32_t time_out=0;
	
	tx_buff[0] = CAN_READ;
	tx_buff[1] = addr;
	tx_buff[2] = 0xff;
	spi_flag = 0;
	time_out = 0xffff;
	SPIdrv->Transfer(&tx_buff, &rx_buff, 3);
	while(spi_flag==0)
	{
		time_out -= 1;
		if(time_out == 0)
		break;
	}
	
	return rx_buff[2];
}
/*****************************************************************************************
* Function Name: MCP2515_Reset
* Description  : ���͸�λָ�������λMCP2515
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void MCP2515_Reset(void)
{
	uint8_t tx_buff[1];
	uint8_t rx_buff[1];
	uint32_t time_out=0;
	
	tx_buff[0] = CAN_RESET;
	spi_flag = 0;
	time_out = 0xffff;
	SPIdrv->Transfer(&tx_buff, &rx_buff, 1);
	while(spi_flag==0)
	{
		time_out -= 1;
		if(time_out == 0)
		break;
	}	
}
/*****************************************************************************************
* Function Name: MCP2515_Init
* Description  : MCP2515��ʼ������
* Arguments    : CANStype��Std_ID-��׼֡��Ext_ID-��չ֡��
                 velocity��������
* Return Value : NONE
******************************************************************************************/
void MCP2515_Init(uint8_t CANStype, uint8_t velocity)
{
	uint8_t temp=0;
	uint16_t buff,buff1=0;
	uint32_t pad_sel;
	
	pad_sel = MIZAR_MISC->PAD_SEL;
	printf("MIZAR_MISC->PAD_SEL: 0x%.8x\n", pad_sel);
	pad_sel = pad_sel | 0xf0;
	MIZAR_MISC->PAD_SEL = pad_sel;
	printf("MIZAR_MISC->PAD_SEL: 0x%.8x\n", MIZAR_MISC->PAD_SEL);
	
	GPIO_PinSetDirection(MCP2515_INT_PIN, GPIO_PIN_DIRECTION_INPUT);
	GPIO_PinIntClear(MCP2515_INT_PIN);
	GPIO_PinIntCfg(MCP2515_INT_PIN, MCP2515_INT_TYPE);
	GPIO_PinIntEnable(MCP2515_INT_PIN);
	NVIC_EnableIRQ(MCP2515_INT_IRQn);
	
	// Initialization SPI driver
	printf("SPIdrv->Initialize(SPI_callback_func)\n");
	SPIdrv->Initialize(SPI_callback_func);

	// Power up the SPI peripheral
	printf("SPIdrv->PowerControl(ARM_POWER_FULL)\n");
	SPIdrv->PowerControl(ARM_POWER_FULL);

	SPIdrv->Control(ARM_SPI_SET_DEFAULT_TX_VALUE, 0xff);

	// Configure the SPI to Master, 8-bit mode @1.25 mBits/sec
	printf("Configure the SPI to Master, 8-bit mode @ %d bps\n", TRANSFER_SPEED);
	SPIdrv->Control(ARM_SPI_MODE_MASTER | 
					POLARITY_CONFIG | 
					ARM_SPI_MSB_LSB | 
					ARM_SPI_SS_MASTER_UNUSED | 
					ARM_SPI_DATA_BITS(8), 
					TRANSFER_SPEED);								

	MCP2515_Reset();	//���͸�λָ�������λMCP2515
	
	mdelay(10);	//��λ����ʱ10ms

	//���ò�����Ϊ125Kbps
	//set CNF1,SJW=00,����Ϊ1TQ,BRP=3,TQ=[2*(BRP+1)]/Fsoc=2*4/8M=1us,bps=1/(1+3+1+3)=125K
	MCP2515_WriteByte(CNF1,velocity);
	//set CNF2,SAM=0,�ڲ���������߽���һ�β�����PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ
	MCP2515_WriteByte(CNF2,0x80|PHSEG1_3TQ|PRSEG_1TQ);
	//set CNF3,PHSEG2=(2+1)TQ=3TQ,ͬʱ��CANCTRL.CLKEN=1ʱ�趨CLKOUT����Ϊʱ�����ʹ��λ
	MCP2515_WriteByte(CNF3,PHSEG2_3TQ);
	
	if (CANStype == Std_ID)
	{
		buff = Std_Send_ID<<5;
		
		MCP2515_WriteByte(TXB0SIDH,(uint8_t)(buff>>8));//���ͻ�����0��׼��ʶ����λ
		MCP2515_WriteByte(TXB0SIDL,(uint8_t)buff);//���ͻ�����0��׼��ʶ����λ
		
		MCP2515_WriteByte(RXB0SIDH,0x00);//��ս��ջ�����0�ı�׼��ʶ����λ
		MCP2515_WriteByte(RXB0SIDL,0x00);//��ս��ջ�����0�ı�׼��ʶ����λ
		MCP2515_WriteByte(RXB0CTRL,0x20);//�������ձ�׼��ʶ������Ч��Ϣ
		MCP2515_WriteByte(RXB0DLC,DLC_8);//���ý������ݵĳ���Ϊ8���ֽ�
		
		buff = Std_Receive_ID<<5;
		
		MCP2515_WriteByte(RXF0SIDH,(uint8_t)(buff>>8));//���������˲��Ĵ���n��׼��ʶ����λ
		MCP2515_WriteByte(RXF0SIDL,(uint8_t)buff);//���������˲��Ĵ���n��׼��ʶ����λ
		
		buff = Std_Receive_Filter<<5;
		
		MCP2515_WriteByte(RXM0SIDH,(uint8_t)(buff>>8));//�����������μĴ���n��׼��ʶ����λ
		MCP2515_WriteByte(RXM0SIDL,(uint8_t)buff);//�����������μĴ���n��׼��ʶ����λ
	}
	else if(CANStype == Ext_ID)
	{
		buff = (Ext_Send_ID>>16)<<3;
		buff1 = (buff&0xE0)|((buff&0x18)>>3);
		buff1 = buff1 | 0x08;
		
		MCP2515_WriteByte(TXB0SIDH,(uint8_t)(buff>>8));//���ͻ�����0��׼��ʶ����λ
		MCP2515_WriteByte(TXB0SIDL,(uint8_t)buff1);//���ͻ�����0��׼��ʶ����λ(��3λΪ������չ��ʶ��ʹ��λ)
		MCP2515_WriteByte(TXB0EID8,(uint8_t)(Ext_Send_ID>>8));//���ͻ�����0��չ��ʶ����λ
		MCP2515_WriteByte(TXB0EID0,(uint8_t)Ext_Send_ID);//���ͻ�����0��չ��ʶ����λ
		
		MCP2515_WriteByte(RXB0SIDH,0x00);//��ս��ջ�����0�ı�׼��ʶ����λ
		MCP2515_WriteByte(RXB0SIDL,0x00);//��ս��ջ�����0�ı�׼��ʶ����λ
		MCP2515_WriteByte(RXB0EID8,0x00);//��ս��ջ�����0����չ��ʶ����λ
		MCP2515_WriteByte(RXB0EID0,0x00);//��ս��ջ�����0����չ��ʶ����λ
		MCP2515_WriteByte(RXB0CTRL,0x40);//����������չ��ʶ������Ч��Ϣ
		MCP2515_WriteByte(RXB0DLC,DLC_8);//���ý������ݵĳ���Ϊ8���ֽ�
		
		buff = (Ext_Receive_ID>>16)<<3;
		buff1 = (buff&0xE0)|((buff&0x18)>>3);
		buff1 = buff1 | 0x08;
		
		MCP2515_WriteByte(RXF0SIDH,(uint8_t)(buff>>8));//���������˲��Ĵ���n��׼��ʶ����λ
		MCP2515_WriteByte(RXF0SIDL,(uint8_t)buff1);//���������˲��Ĵ���n��׼��ʶ����λ(��3λΪ������չ��ʶ��ʹ��λ)
		MCP2515_WriteByte(RXF0EID8,(uint8_t)(Ext_Receive_ID>>8));//���������˲��Ĵ���n��չ��ʶ����λ
		MCP2515_WriteByte(RXF0EID0,(uint8_t)Ext_Receive_ID);//���������˲��Ĵ���n��չ��ʶ����λ
		
		buff = (Ext_Receive_Filter>>16)<<3;
		buff1 = (buff&0xE0)|((buff&0x18)>>3);

		MCP2515_WriteByte(RXM0SIDH,(uint8_t)(buff>>8));//�����������μĴ���n��׼��ʶ����λ
		MCP2515_WriteByte(RXM0SIDL,(uint8_t)buff1);//�����������μĴ���n��׼��ʶ����λ
		MCP2515_WriteByte(RXM0EID8,(uint8_t)(Ext_Receive_Filter>>8));//���������˲��Ĵ���n��չ��ʶ����λ
		MCP2515_WriteByte(RXM0EID0,(uint8_t)Ext_Receive_Filter);//���������˲��Ĵ���n��չ��ʶ����λ
	}
	
	MCP2515_WriteByte(CANINTF,0x00);//���CAN�жϱ�־�Ĵ���������λ(������MCU���)
	MCP2515_WriteByte(CANINTE,0x01);//����CAN�ж�ʹ�ܼĴ����Ľ��ջ�����0���ж�ʹ��,����λ��ֹ�ж�
	
	MCP2515_WriteByte(CANCTRL,REQOP_NORMAL|CLKOUT_ENABLED);//��MCP2515����Ϊ����ģʽ,�˳�����ģʽ
	
	temp=MCP2515_ReadByte(CANSTAT);//��ȡCAN״̬�Ĵ�����ֵ
	if(OPMODE_NORMAL!=(temp&0xE0))//�ж�MCP2515�Ƿ��Ѿ���������ģʽ
	{
		MCP2515_WriteByte(CANCTRL,REQOP_NORMAL|CLKOUT_ENABLED);//�ٴν�MCP2515����Ϊ����ģʽ,�˳�����ģʽ
	}
}
/*****************************************************************************************
* Function Name: CAN_Send_Buffer
* Description  : CAN����ָ�����ȵ�����
* Arguments    : *CAN_TX_Buf:���������ݻ�����ָ��
				 len:���������ݳ���
* Return Value : NONE
******************************************************************************************/
void CAN_Send_Buffer(unsigned char *CAN_TX_Buf,unsigned char len)
{
	uint8_t j,dly,count;

	count=0;
	while(count<len)
	{
		dly=0;
		while((MCP2515_ReadByte(TXB0CTRL)&0x08) && (dly<50))//���ٶ�ĳЩ״ָ̬��,�ȴ�TXREQ��־����
		{
			dly++;
		}
				
		for(j=0;j<8;)
		{
			MCP2515_WriteByte(TXB0D0+j,CAN_TX_Buf[count++]);//�������͵�����д�뷢�ͻ���Ĵ���
			j++;
			if(count>=len) break;
		}
		MCP2515_WriteByte(TXB0DLC,j);//����֡�����͵����ݳ���д�뷢�ͻ�����0�ķ��ͳ��ȼĴ���

		MCP2515_WriteByte(TXB0CTRL,0x08);//�����ͱ���
	}
}
/*****************************************************************************************
* Function Name: CAN_Receive_Buffer
* Description  : CAN����һ֡����
* Arguments    : *CAN_TX_Buf:���������ݻ�����ָ��
* Return Value : len:���յ����ݵĳ���,1~8�ֽڣ�0��ʾû�н��յ�����
******************************************************************************************/
unsigned char CAN_Receive_Buffer(unsigned char *CAN_RX_Buf)
{
	uint8_t i=0,len=0,temp=0;

	if(CAN_flag == 1)
	{
		CAN_flag = 0;
		
		temp = MCP2515_ReadByte(CANINTF);
		if(temp & 0x01)
		{
			len=MCP2515_ReadByte(RXB0DLC);//��ȡ���ջ�����0���յ������ݳ���(0~8���ֽ�)
			while(i<len)
			{	
				CAN_RX_Buf[i]=MCP2515_ReadByte(RXB0D0+i);//��CAN���յ������ݷ���ָ��������
				i++;
			}
		}
		MCP2515_WriteByte(CANINTF,0);//����жϱ�־λ(�жϱ�־�Ĵ���������MCU����)
	}
	
	return len;
}
/*****************************************************************************************
* Function Name: MCP2515_INT_IRQHandler
* Description  : INT�����жϴ�����
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void MCP2515_INT_IRQHandler(void)
{
	CAN_flag = 1;
	
	GPIO_PinIntClear(MCP2515_INT_PIN);
}
/***********************************END OF FILE*******************************************/



