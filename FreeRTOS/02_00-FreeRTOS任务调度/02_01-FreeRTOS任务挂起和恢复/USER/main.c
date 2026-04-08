#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "rtos_task.h"


int main(void)
{
    uart_init(115200);
    LED_Init();
    delay_init();
    KEY_Init();
     printf("Hello World!\r\n");
    freertos_demo();

}
