/****************************************************************************************
**  Filename :  ThinkTech_MCP2515.c
**  Abstract :  This file implements key function.
**  By       :  钟伟
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
* Description  : SPI中断回调函数
* Arguments    : e:中断事件
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
* Description  : 通过SPI读取一个字节数据
* Arguments    : NONE
* Return Value : rByte(读取到的一个字节数据)
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
* Description  : SPI发送一个字节数据
* Arguments    : dt:待发送的数据
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
* Description  : 通过SPI向MCP2515指定地址寄存器写1个字节数据
* Arguments    : addr:MCP2515寄存器地址
								 dat:待写入的数据
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
* Description  : 通过SPI从MCP2515指定地址寄器读1个字节数据
* Arguments    : addr:MCP2515寄存器地址
* Return Value : rByte:读取到寄存器的1个字节数据
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
* Description  : 发送复位指令软件复位MCP2515
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
* Description  : MCP2515初始化配置
* Arguments    : CANStype：Std_ID-标准帧，Ext_ID-扩展帧，
                 velocity：波特率
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

	MCP2515_Reset();	//发送复位指令软件复位MCP2515
	
	mdelay(10);	//复位后延时10ms

	//设置波特率为125Kbps
	//set CNF1,SJW=00,长度为1TQ,BRP=3,TQ=[2*(BRP+1)]/Fsoc=2*4/8M=1us,bps=1/(1+3+1+3)=125K
	MCP2515_WriteByte(CNF1,velocity);
	//set CNF2,SAM=0,在采样点对总线进行一次采样，PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ
	MCP2515_WriteByte(CNF2,0x80|PHSEG1_3TQ|PRSEG_1TQ);
	//set CNF3,PHSEG2=(2+1)TQ=3TQ,同时当CANCTRL.CLKEN=1时设定CLKOUT引脚为时间输出使能位
	MCP2515_WriteByte(CNF3,PHSEG2_3TQ);
	
	if (CANStype == Std_ID)
	{
		buff = Std_Send_ID<<5;
		
		MCP2515_WriteByte(TXB0SIDH,(uint8_t)(buff>>8));//发送缓冲器0标准标识符高位
		MCP2515_WriteByte(TXB0SIDL,(uint8_t)buff);//发送缓冲器0标准标识符低位
		
		MCP2515_WriteByte(RXB0SIDH,0x00);//清空接收缓冲器0的标准标识符高位
		MCP2515_WriteByte(RXB0SIDL,0x00);//清空接收缓冲器0的标准标识符低位
		MCP2515_WriteByte(RXB0CTRL,0x20);//仅仅接收标准标识符的有效信息
		MCP2515_WriteByte(RXB0DLC,DLC_8);//设置接收数据的长度为8个字节
		
		buff = Std_Receive_ID<<5;
		
		MCP2515_WriteByte(RXF0SIDH,(uint8_t)(buff>>8));//配置验收滤波寄存器n标准标识符高位
		MCP2515_WriteByte(RXF0SIDL,(uint8_t)buff);//配置验收滤波寄存器n标准标识符低位
		
		buff = Std_Receive_Filter<<5;
		
		MCP2515_WriteByte(RXM0SIDH,(uint8_t)(buff>>8));//配置验收屏蔽寄存器n标准标识符高位
		MCP2515_WriteByte(RXM0SIDL,(uint8_t)buff);//配置验收屏蔽寄存器n标准标识符低位
	}
	else if(CANStype == Ext_ID)
	{
		buff = (Ext_Send_ID>>16)<<3;
		buff1 = (buff&0xE0)|((buff&0x18)>>3);
		buff1 = buff1 | 0x08;
		
		MCP2515_WriteByte(TXB0SIDH,(uint8_t)(buff>>8));//发送缓冲器0标准标识符高位
		MCP2515_WriteByte(TXB0SIDL,(uint8_t)buff1);//发送缓冲器0标准标识符低位(第3位为发送拓展标识符使能位)
		MCP2515_WriteByte(TXB0EID8,(uint8_t)(Ext_Send_ID>>8));//发送缓冲器0拓展标识符高位
		MCP2515_WriteByte(TXB0EID0,(uint8_t)Ext_Send_ID);//发送缓冲器0拓展标识符低位
		
		MCP2515_WriteByte(RXB0SIDH,0x00);//清空接收缓冲器0的标准标识符高位
		MCP2515_WriteByte(RXB0SIDL,0x00);//清空接收缓冲器0的标准标识符低位
		MCP2515_WriteByte(RXB0EID8,0x00);//清空接收缓冲器0的拓展标识符高位
		MCP2515_WriteByte(RXB0EID0,0x00);//清空接收缓冲器0的拓展标识符低位
		MCP2515_WriteByte(RXB0CTRL,0x40);//仅仅接收拓展标识符的有效信息
		MCP2515_WriteByte(RXB0DLC,DLC_8);//设置接收数据的长度为8个字节
		
		buff = (Ext_Receive_ID>>16)<<3;
		buff1 = (buff&0xE0)|((buff&0x18)>>3);
		buff1 = buff1 | 0x08;
		
		MCP2515_WriteByte(RXF0SIDH,(uint8_t)(buff>>8));//配置验收滤波寄存器n标准标识符高位
		MCP2515_WriteByte(RXF0SIDL,(uint8_t)buff1);//配置验收滤波寄存器n标准标识符低位(第3位为接收拓展标识符使能位)
		MCP2515_WriteByte(RXF0EID8,(uint8_t)(Ext_Receive_ID>>8));//配置验收滤波寄存器n拓展标识符高位
		MCP2515_WriteByte(RXF0EID0,(uint8_t)Ext_Receive_ID);//配置验收滤波寄存器n拓展标识符低位
		
		buff = (Ext_Receive_Filter>>16)<<3;
		buff1 = (buff&0xE0)|((buff&0x18)>>3);

		MCP2515_WriteByte(RXM0SIDH,(uint8_t)(buff>>8));//配置验收屏蔽寄存器n标准标识符高位
		MCP2515_WriteByte(RXM0SIDL,(uint8_t)buff1);//配置验收屏蔽寄存器n标准标识符低位
		MCP2515_WriteByte(RXM0EID8,(uint8_t)(Ext_Receive_Filter>>8));//配置验收滤波寄存器n拓展标识符高位
		MCP2515_WriteByte(RXM0EID0,(uint8_t)Ext_Receive_Filter);//配置验收滤波寄存器n拓展标识符低位
	}
	
	MCP2515_WriteByte(CANINTF,0x00);//清空CAN中断标志寄存器的所有位(必须由MCU清空)
	MCP2515_WriteByte(CANINTE,0x01);//配置CAN中断使能寄存器的接收缓冲器0满中断使能,其它位禁止中断
	
	MCP2515_WriteByte(CANCTRL,REQOP_NORMAL|CLKOUT_ENABLED);//将MCP2515设置为正常模式,退出配置模式
	
	temp=MCP2515_ReadByte(CANSTAT);//读取CAN状态寄存器的值
	if(OPMODE_NORMAL!=(temp&0xE0))//判断MCP2515是否已经进入正常模式
	{
		MCP2515_WriteByte(CANCTRL,REQOP_NORMAL|CLKOUT_ENABLED);//再次将MCP2515设置为正常模式,退出配置模式
	}
}
/*****************************************************************************************
* Function Name: CAN_Send_Buffer
* Description  : CAN发送指定长度的数据
* Arguments    : *CAN_TX_Buf:待发送数据缓冲区指针
				 len:待发送数据长度
* Return Value : NONE
******************************************************************************************/
void CAN_Send_Buffer(unsigned char *CAN_TX_Buf,unsigned char len)
{
	uint8_t j,dly,count;

	count=0;
	while(count<len)
	{
		dly=0;
		while((MCP2515_ReadByte(TXB0CTRL)&0x08) && (dly<50))//快速读某些状态指令,等待TXREQ标志清零
		{
			dly++;
		}
				
		for(j=0;j<8;)
		{
			MCP2515_WriteByte(TXB0D0+j,CAN_TX_Buf[count++]);//将待发送的数据写入发送缓冲寄存器
			j++;
			if(count>=len) break;
		}
		MCP2515_WriteByte(TXB0DLC,j);//将本帧待发送的数据长度写入发送缓冲器0的发送长度寄存器

		MCP2515_WriteByte(TXB0CTRL,0x08);//请求发送报文
	}
}
/*****************************************************************************************
* Function Name: CAN_Receive_Buffer
* Description  : CAN接收一帧数据
* Arguments    : *CAN_TX_Buf:待接收数据缓冲区指针
* Return Value : len:接收到数据的长度,1~8字节，0表示没有接收到数据
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
			len=MCP2515_ReadByte(RXB0DLC);//读取接收缓冲器0接收到的数据长度(0~8个字节)
			while(i<len)
			{	
				CAN_RX_Buf[i]=MCP2515_ReadByte(RXB0D0+i);//把CAN接收到的数据放入指定缓冲区
				i++;
			}
		}
		MCP2515_WriteByte(CANINTF,0);//清除中断标志位(中断标志寄存器必须由MCU清零)
	}
	
	return len;
}
/*****************************************************************************************
* Function Name: MCP2515_INT_IRQHandler
* Description  : INT引脚中断处理函数
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void MCP2515_INT_IRQHandler(void)
{
	CAN_flag = 1;
	
	GPIO_PinIntClear(MCP2515_INT_PIN);
}
/***********************************END OF FILE*******************************************/



