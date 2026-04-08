#include "rtos_task.h"
#include "usart.h"
#include "FreeRTOS.h"					//FreeRTOS使用
#include "task.h"						//FreeRTOS使用
 
 
 
TaskHandle_t   LEDTask1handle;
TaskHandle_t   LEDTask2handle;
 
 void LEDTask1( void * pvParameters );
  
 void LEDTask2 (void * pvParameters );
 
 
 
/*********************任务函数**********************/
 
 
void App_StartTask(void)                    /*创建任务的函数*/
{
      BaseType_t  xReturn =pdPASS;       //建立一个变量来接收创建任务的返回值
 
      taskENTER_CRITICAL();        /* 进入临界区，创建任务过程我们必须保证在临界区 */
 
    /* 动态创建创建任务的任务 */
    xReturn = xTaskCreate(
                         (TaskFunction_t )LEDTask1,              /* 任务函数 */
                         (const char*    )"LEDTask1",            /* 任务名称 */
                         (uint32_t       )128,                   /* 任务堆栈大小 */
                         (void*          )NULL,                  /* 传递给任务函数的参数 */
                         (UBaseType_t    )3,                     /* 任务优先级 */
                         (TaskHandle_t*  )&LEDTask1handle);      /* 任务句柄 */
 
    if(xReturn == pdPASS) printf("LEDTask1任务创建成功!\r\n");
    else                  printf("LEDTask1任务创建失败!\r\n");
 
 
   /* 动态创建创建任务的任务 */
    xReturn = xTaskCreate(LEDTask2, "LEDTask2", 128, NULL, 2, &LEDTask2handle);
 
    if(xReturn == pdPASS) printf("LEDTask2任务创建成功!\r\n");
    else                  printf("LEDTask2任务创建失败!\r\n");
 
        taskEXIT_CRITICAL();         /* 退出临界区 */
 
 
        vTaskStartScheduler();		            //启动任务调度器
}
 

 
void LEDTask1(void *pvParameters) {
    uint16_t cnt = 0;
    TickType_t xFirstTime;              /* 保存延时前的系统时钟节拍 */
    TickType_t xNextTime;               /* 保存退出任务阻塞态的系统时钟节拍 */
 
    while(1) {
        xFirstTime = xTaskGetTickCount();              /* 获取当前任务进入点的系统时钟节拍 */
        xNextTime = xFirstTime;                       /* 保存当前任务进入点的系统时钟节拍 */
 
        vTaskDelay(200);                        /* 任务延时200ms */
 
        vTaskDelayUntil(&xNextTime, 1000);        /* 以周期的方式执行任务，周期为1000ms */
 
        cnt = xTaskGetTickCount() - xFirstTime;              /* 计算任务执行的时间，单位为ms */
 
        taskENTER_CRITICAL();         /* 进入临界区，打印任务执行时间 */
        printf("LEDTask1 is running! 绝对延时 cnt = %d\r\n", cnt++);
        taskEXIT_CRITICAL();         /* 退出临界区 */
    }
}
 
void LEDTask2 (void * pvParameters )
{
    uint16_t cnt = 0;
    TickType_t xFirstTime;              /* 保存延时前的系统时钟节拍 */
  
    while(1)
    {
        xFirstTime = xTaskGetTickCount();              /* 获取当前任务进入点的系统时钟节拍 */
  
  
        vTaskDelay(200);  
                                   /* 任务延时200ms */ 
        vTaskDelay( 1000 );        /* 以周期的方式执行任务，周期为1000ms */
  
        cnt = xTaskGetTickCount() - xFirstTime;              /* 计算任务执行的时间，单位为ms */ 
  
        taskENTER_CRITICAL();         /* 进入临界区，打印任务执行时间 */
         printf("LEDTask2 is running! 相对延时 cnt = %d\r\n", cnt++);
         taskEXIT_CRITICAL();         /* 退出临界区 */
    }
  
}