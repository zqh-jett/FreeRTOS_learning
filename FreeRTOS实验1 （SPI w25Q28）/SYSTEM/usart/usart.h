#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

extern u8 Usart1_Rx_Buffer;

void uart_init(u32 bound);
void Usart1_SendByte(u8 data);
void Usart1_SendString(char *str);
void Usart1_SendArray(uint8_t *Array, uint16_t Length);

int fputc(int ch, FILE *p);
#endif


