#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

 
 				  	    													  
#define SPIx                         SPI2
#define SPI_APBxClock_FUN            RCC_APB1PeriphClockCmd
#define SPI_CLK                      RCC_APB1Periph_SPI2
#define SPI_GPIO_APBxClock_FUN       RCC_APB1PeriphClockCmd
 
#define SPI_SCK_PORT                 GPIOB  
#define SPI_SCK_PIN                  GPIO_Pin_13


#define SPI_MISO_PORT                GPIOB 
#define SPI_MISO_PIN                 GPIO_Pin_14
 
#define SPI_MOSI_PORT                GPIOB 
#define SPI_MOSI_PIN                 GPIO_Pin_15
 

 
#define SPI_GPIO_CLK                 RCC_APB2Periph_GPIOB
#define SPI_CS_PORT                  GPIOB 
#define SPI_CS_PIN                   GPIO_Pin_12
 
//CS引脚配置
#define SPI_CS_HIGH()  		         GPIO_SetBits(SPI_CS_PORT,SPI_CS_PIN);
#define SPI_CS_LOW()		         GPIO_ResetBits(SPI_CS_PORT,SPI_CS_PIN);


void SPI_FLASH_Init(void);                      //SPI初始化函数
static void SPI_GPIO_Config(void);              //GPIO初始化函数   
static void SPI_Mode_Config(void);                   //SPI配置函数   
uint8_t SPI_FLASH_SendByte(uint8_t data);       //SPI发送一个字节函数
#endif

