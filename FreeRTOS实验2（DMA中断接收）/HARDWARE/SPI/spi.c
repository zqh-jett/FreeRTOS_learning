#include "spi.h"



void SPI_FLASH_Init(void)
{
	
	SPI_GPIO_Config();
	SPI_Mode_Config();
 
}

//GPIO配置
static void SPI_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
	
		/* 使能与SPI 有关的时钟 */
		SPI_APBxClock_FUN ( SPI_CLK, ENABLE );
		SPI_GPIO_APBxClock_FUN ( SPI_GPIO_CLK, ENABLE );
		
		
	/* MISO MOSI SCK*/
		GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       
		GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       
		GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	       
		GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);
		
		//初始化CS引脚，使用软件控制，所以直接设置成推挽输出	
		GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	       
		GPIO_Init(SPI_CS_PORT, &GPIO_InitStructure);
		
		SPI_CS_HIGH();
}

//工作模式配置
static void SPI_Mode_Config(void)
{
  SPI_InitTypeDef  SPI_InitStructure; 
 
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256 ;// SPI 波特率预分频器，设置SPI时钟频率 = 系统时钟/2
	//SPI 使用模式3
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;// SPI 时钟相位配置，选择第二个边沿进行数据采样
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;// SPI 时钟极性配置，选择高电平空闲
	SPI_InitStructure.SPI_CRCPolynomial = 7;//不使用CRC功能
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;// SPI 数据帧格式，设置为8位数据
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线全双工
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;// SPI 数据传输顺序，设置为高位先行(MSB First)
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;// SPI 工作模式，设置为主设备模式
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;// NSS（片选）信号管理，设置为软件控制
	
	SPI_Init(SPIx,&SPI_InitStructure);	//写入配置到寄存器
	
	SPI_Cmd(SPIx,ENABLE);//使能SPI
	
}


uint8_t SPI_FLASH_SendByte(uint8_t data)
{
	//检查并等待至TX缓冲区为空
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_TXE) == RESET)
	{}
 
	
	//程序执行到此处，TX缓冲区已空
	SPI_I2S_SendData (SPIx,data);
	
	
	//检查并等待至RX缓冲区为非空
	while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_RXNE) == RESET)
	{}
 
		
	//程序执行到此处，说明数据发送完毕，并接收到一字字节	
	return SPI_I2S_ReceiveData(SPIx); 
 
}

























