#ifndef __W25Q128_H
#define __W25Q128_H
#include "spi.h"
#include "sys.h"

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

#define NM25Q80 	0X5213
#define NM25Q16 	0X5214
#define NM25Q32 	0X5215
#define NM25Q64 	0X5216
#define NM25Q128	0X5217
#define NM25Q256 	0X5218

extern u16 W25QXX_TYPE;					//定义W25QXX芯片型号		   

#define	W25QXX_CS 		PBout(12)  		//W25QXX的片选信号
				 
////////////////////////////////////////////////////////////////////////////
 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 


	// 基础初始化与通信函数
void W25QXX_Init(void);                            // W25QXX初始化函数
u8 W25Q128_SendByte(u8 byte);                      // 单字节收发函数（兼容u8）
u16 W25Q128_ReadDeviceId(void);                    // 读取W25Q128设备ID

// 写使能/禁止函数
void W25QXX_Write_Enable(void);                    // 写使能
void W25QXX_Write_Disable(void);                   // 写禁止

// 状态寄存器操作函数
u8 W25QXX_ReadSR(void);                            // 读取状态寄存器（返回u8兼容原代码）

// 擦除操作函数
void W25QXX_Erase_Sector(u32 Address);             // 扇区擦除（4KB）

// 读写操作函数
void W25QXX_Read_Data(u32 Address, u8 *RecvBuf, u32 ReadLen); // 读取数据
void W25QXX_WritePageProgram(u32 Address, u8 *RecvBuf, u32 ReadLen); // 页编程写数据

void W25QXX_Wait_Busy(void);           	//等待空闲


void spi_Text(void);                    //测试SPI和w25q128通信函数	 				    
#endif
