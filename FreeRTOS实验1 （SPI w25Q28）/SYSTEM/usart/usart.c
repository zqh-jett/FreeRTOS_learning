#include "sys.h"
#include "usart.h"	  
 #include <stdio.h>
  


void uart_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 
	
	
}

void Usart1_SendByte(u8 data)
{
	USART_SendData(USART1, data);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
}

void Usart1_SendString(char *str)
{
	while (*str != '\0')
	{
		Usart1_SendByte(*str);
		str++;
	}
}
void Usart1_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		Usart1_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

int fputc(int ch, FILE *p) 
{
    // 将字符 'ch' 发送到 USART1（串口 1）。USART_SendData 负责将数据写入发送数据寄存器。
    USART_SendData(USART1, (u8)ch); 
 
    // 等待直到发送数据寄存器为空，表示数据已经成功发送。USART_FLAG_TXE 是一个标志，表示传输数据寄存器为空。
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 
 

    // 返回发送的字符 'ch'。通常用于兼容标准库的 fputc 函数，表示字符已经发送成功。
    return ch; 
}

 uint8_t USART_Rx_Buffer;
 uint8_t USART_Rx_Flag;  


void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_Rx_Flag = 1;
		USART_Rx_Buffer = USART_ReceiveData(USART1);
		USART_SendData(USART1, USART_Rx_Buffer);
		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}
