#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#include "rtos_task.h"

uint8_t redata;

int main(void)
{
    uart_init(115200);
    LED_Init();
    delay_init();
     printf("Hello World!\r\n");
     

   App_StartTask();
    while(1)
    {


    }
}
