#include "W25Q128.h"
#include "delay.h"

#include <string.h>
#include "usart.h"

void W25QXX_Init(void)                           // W25QXX初始化函数
{
    SPI_FLASH_Init(); // 初始化SPI接口

}
u8 W25Q128_SendByte(u8 byte)                      // 单字节收发函数（兼容u8）
{
    return SPI_FLASH_SendByte(byte); // 调用SPI发送函数
}
/**
 * @brief  读取W25Q128的设备ID（验证Flash通信是否正常）
 * @note   W25Q128标准ID：0xEF17（厂家ID=0xEF，设备ID=0x17）
 * @param  无
 * @retval 16位设备ID：高8位=厂家ID，低8位=设备ID
 */
uint16_t W25Q128_ReadDeviceId(void)
{
    uint16_t ID = 0;  // 存储最终的设备ID
	
    // 1. 拉低CS引脚，选中Flash（片选有效）
    SPI_CS_LOW();
    delay_us(1);      // 短暂延时，确保片选稳定（满足时序要求）
	
    // 2. 发送读取ID指令序列：0x90（指令） + 3个0x00（地址字节）
    W25Q128_SendByte(0x90);  // 发送"读取JEDEC ID"指令
    W25Q128_SendByte(0x00);  // 地址字节1：A23-A16（固定0）
    W25Q128_SendByte(0x00);  // 地址字节2：A15-A8（固定0）
    W25Q128_SendByte(0x00);  // 地址字节3：A7-A0（固定0）
	
    // 3. 接收设备ID：先收厂家ID（左移8位到高字节），再收设备ID（低字节）
    ID = W25Q128_SendByte(0xFF) << 8;  // 发0xFF（无效数据），收厂家ID
    ID |= W25Q128_SendByte(0xFF);      // 发0xFF，收设备ID并拼接
	
    // 4. 拉高CS引脚，取消选中Flash
    SPI_CS_HIGH();
	
    // 5. 返回16位设备ID
    return ID;
}

/**
 * @brief  W25QXX写使能函数（擦除/写入前必须执行）
 * @note   向Flash发送0x06指令，解锁写操作（状态寄存器WEL位置1）
 * @param  无
 * @retval 无
 */
void W25QXX_Write_Enable(void)
{
    SPI_CS_LOW();                      // 选中Flash
    W25Q128_SendByte(W25X_WriteEnable); // 发送写使能指令
    SPI_CS_HIGH();                     // 取消选中，提交指令
}	

/**
 * @brief  W25QXX写禁止函数（操作完成后执行，防误写）
 * @note   向Flash发送0x04指令，锁定写操作（状态寄存器WEL位清0）
 * @param  无
 * @retval 无
 */
void W25QXX_Write_Disable(void)	
{
    SPI_CS_LOW();                       // 选中Flash
    W25Q128_SendByte(W25X_WriteDisable); // 发送写禁止指令
    SPI_CS_HIGH();                      // 取消选中，提交指令
}

/**
 * @brief  读取W25QXX状态寄存器（判断Flash是否忙）
 * @note   状态寄存器BIT0=1：忙（擦除/写入中）；BIT0=0：空闲
 * @param  无
 * @retval 状态寄存器值（8位）
 */
uint8_t W25QXX_ReadSR(void) // 统一返回值类型为uint8_t（原u8需确认typedef）
{
    uint8_t status = 0;                 // 存储状态寄存器值
	
    SPI_CS_LOW();                       // 选中Flash
    W25Q128_SendByte(W25X_ReadStatusReg); // 发送读状态寄存器指令
    status = W25Q128_SendByte(0xFF);    // 发0xFF，接收状态寄存器值
    SPI_CS_HIGH();                      // 取消选中
	
    return status;                      // 返回状态值
}	

/**
 * @brief  W25QXX扇区擦除函数（4KB/扇区，擦除后全为0xFF）
 * @note   1. 地址必须4KB对齐（如0x000000、0x001000、0x002000...）
 *         2. 擦除耗时约100ms，需等待忙标志清零
 * @param  Address：扇区起始地址（必须4KB对齐）
 * @retval 无
 */
void W25QXX_Erase_Sector(uint32_t Address)
{
    // 1. 写使能：解锁擦除操作
    W25QXX_Write_Enable();
    delay_ms(1);                        // 短延时，确保使能生效
	
    // 2. 选中Flash，准备发送擦除指令
    SPI_CS_LOW();	
	
    // 3. 发送扇区擦除指令
    W25Q128_SendByte(W25X_SectorErase);
	
    // 4. 发送3字节擦除地址（24位地址，按字节拆分）
    W25Q128_SendByte((Address & 0xFF0000) >> 16); // 地址高8位（A23-A16）
    W25Q128_SendByte((Address & 0x00FF00) >> 8);  // 地址中8位（A15-A8）
    W25Q128_SendByte(Address & 0x0000FF);         // 地址低8位（A7-A0）
	
    // 5. 释放CS，提交擦除指令（关键：必须先拉高CS才能轮询忙标志）
    SPI_CS_HIGH();         
    delay_us(1);                        // 满足时序要求
	
    // 6. 轮询忙标志：等待擦除完成（BIT0=0）
	
	W25QXX_Wait_Busy();
    //	while(W25QXX_ReadSR() & 0x01);
	
    // 7. 擦除完成，写禁止：锁定写操作，防误写
    W25QXX_Write_Disable();
}

/**
 * @brief  从W25QXX读取指定长度的数据
 * @note   支持跨页读取，无字节数限制（只要地址不超Flash容量）
 * @param  Address：读取起始地址（24位）
 * @param  RecvBuf：接收数据的缓冲区指针
 * @param  ReadLen：要读取的字节数
 * @retval 无
 */
void W25QXX_Read_Data(uint32_t Address, uint8_t *RecvBuf, uint32_t ReadLen)
{
    SPI_CS_LOW();                       // 选中Flash
	
    // 1. 发送普通读取指令
    W25Q128_SendByte(W25X_ReadData);
	
    // 2. 发送3字节读取起始地址
    W25Q128_SendByte((Address & 0xFF0000) >> 16); // 地址高8位
    W25Q128_SendByte((Address & 0x00FF00) >> 8);  // 地址中8位
    W25Q128_SendByte(Address & 0x0000FF);         // 地址低8位
	
    // 3. 循环读取指定长度的字节（SPI全双工：发0xFF收有效数据）
    while(ReadLen--)
    {
        *RecvBuf++ = W25Q128_SendByte(0xFF); // 读取1字节，缓冲区指针后移
    }
	
    SPI_CS_HIGH();                      // 取消选中，读取完成
}

/**
 * @brief  W25QXX页编程函数（向Flash写入数据，每页最大256字节）
 * @note   1. 写入前必须先擦除扇区（未擦除区域无法写入0→1）
 *         2. 单次写入不能跨页（地址+长度≤页边界，256字节/页）
 *         3. 写入耗时约300μs，需等待忙标志清零
 * @param  Address：写入起始地址（24位）
 * @param  RecvBuf：要写入的数据缓冲区指针
 * @param  ReadLen：要写入的字节数（建议≤256）
 * @retval 无
 */
void W25QXX_WritePageProgram(uint32_t Address, uint8_t *RecvBuf, uint32_t ReadLen)
{
    // 1. 写使能：解锁写入操作
    W25QXX_Write_Enable();
    delay_ms(1);                        // 短延时，确保使能生效
	
    // 2. 选中Flash，准备发送写指令
    SPI_CS_LOW();	
	
    // 3. 发送页编程指令（0x02）
    W25Q128_SendByte(W25X_PageProgram);
	
    // 4. 发送3字节写入起始地址
    W25Q128_SendByte((Address & 0xFF0000) >> 16); // 地址高8位
    W25Q128_SendByte((Address & 0x00FF00) >> 8);  // 地址中8位
    W25Q128_SendByte(Address & 0x0000FF);         // 地址低8位
	
    // 5. 循环发送要写入的字节（逐个写入Flash）
    while(ReadLen--)
    {
        W25Q128_SendByte(*RecvBuf);     // 发送1字节数据
        RecvBuf++;                      // 缓冲区指针后移，指向下一个字节
    }
	
    // 6. 拉高CS，提交写入指令
    SPI_CS_HIGH();
	
    // 7. 轮询忙标志：等待写入完成（BIT0=0）
    W25QXX_Wait_Busy();
	
    // 8. 写入完成，写禁止：锁定写操作，防误写
    W25QXX_Write_Disable();
}
//等待空闲
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);  		// 等待BUSY位清空
}  

/*SPI 读写测试*/
uint8_t i;
uint16_t ID;
uint8_t W25QXX_ReceBuf[11]={0};			//收到数据
uint8_t Write_Buf[]={"helloword"};		//写入数据

void spi_Text(void)
{		
//	获取芯片ID	
	ID=W25Q128_ReadDeviceId();		
	printf("16位ID：0x%04X\r\n", ID);

//	擦除数据后输出	0XFF	
	W25QXX_Erase_Sector(0x000000);	
	W25QXX_Read_Data(0x000000,W25QXX_ReceBuf,10);
	printf("擦除的数据后输出0xff	%#x \r\n",W25QXX_ReceBuf[i]);	
	
//	将擦除数据归零	
	memset(W25QXX_ReceBuf,0,11);
	W25QXX_Read_Data(0x000000,W25QXX_ReceBuf,11);
	printf("数据归零：	%s \r\n",W25QXX_ReceBuf);
	
//	写入数据	helloword	
	W25QXX_WritePageProgram(0x000000,Write_Buf,10);
	printf("写入数据:\t%s\r\n",Write_Buf);
// 读取数据		helloword
	W25QXX_Read_Data(0x000000,W25QXX_ReceBuf,11);
	printf("读取数据:%s \r\n",W25QXX_ReceBuf);
		
	delay_ms(800);	
}
