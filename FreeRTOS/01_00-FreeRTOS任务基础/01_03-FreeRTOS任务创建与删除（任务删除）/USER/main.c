#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#include "rtos_task.h"


int main(void)
{
    uart_init(115200);
    LED_Init();
    delay_init();
     printf("Hello World!\r\n");
    freertos_demo();
    while(1)
    {

        LED0 = !LED0;
        delay_ms(1000);
    }
}
