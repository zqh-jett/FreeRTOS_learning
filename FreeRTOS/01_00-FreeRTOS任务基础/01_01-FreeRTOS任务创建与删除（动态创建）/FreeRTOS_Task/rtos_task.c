#include "rtos_task.h"
#include "FreeRTOS.h"					//FreeRTOS使用
#include "task.h"						//FreeRTOS使用

/**************************************************************************/
/* freeRTOS任务配置 */

/***
* 任务创建任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define START_PRIO      1                     /* 任务优先级 */
#define START_STK_SIZE  64                    /* 任务堆栈大小，字为单位，1字等于4字节 */
static TaskHandle_t StartTask_Handler=NULL;   /* 创建任务的任务句柄 */
static void StartTaskCreate(void* parameter); /* 创建任务的任务函数名 */

/***
* TASK1任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define TASK1_PRIO      1              /* 任务优先级 */
#define TASK1_STK_SIZE  64             /* 任务堆栈大小，字为单位，1字等于4字节 */
 static TaskHandle_t Task1_Handle=NULL; /* 任务1的任务句柄 */
 static void Task1(void* parameter);    /* 任务1的任务函数名 */
 
/***
* TASK2任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define TASK2_PRIO      2              /* 任务优先级 */
#define TASK2_STK_SIZE  64             /* 任务堆栈大小，字为单位，1字等于4字节 */
static TaskHandle_t Task2_Handle=NULL; /* 任务2的任务句柄 */
static void Task2(void* parameter);    /* 任务2的任务函数名 */

/***
* TASK3任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define TASK3_PRIO      3              /* 任务优先级 */
#define TASK3_STK_SIZE  64             /* 任务堆栈大小，字为单位，1字等于4字节 */
static TaskHandle_t Task3_Handle=NULL; /* 任务3的任务句柄 */
static void Task3(void* parameter);    /* 任务3的任务函数名 */

/**************************************************************************/





/**********************************************************


************************************************************/
void freertos_demo(void)
{
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值 pdPASS：成功 */

    /* 动态创建创建任务的任务 */
    xReturn = xTaskCreate(
                         (TaskFunction_t )StartTaskCreate,     /* 任务函数 */
                         (const char*    )"StartTaskCreate",   /* 任务名称 */
                         (uint32_t       )START_STK_SIZE,      /* 任务堆栈大小 */
                         (void*          )NULL,                /* 传递给任务函数的参数 */
                         (UBaseType_t    )START_PRIO,          /* 任务优先级 */
                         (TaskHandle_t*  )&StartTask_Handler); /* 任务句柄 */

    if(xReturn == pdPASS) printf("StartTaskCreate任务创建成功!\r\n");
    else                  printf("StartTaskCreate任务创建失败!\r\n");

    vTaskStartScheduler();  /* 启动任务调度器 */
}

/**************************************************
 *          创建任务的任务函数
 * 
 * 
 * 
 * 
 *         这个任务创建函数是专门用来创建任务函数的，
 *         我们在这个函数里面创建我们需要的任务函数，用完删除此任务。
 ************************************************/

static void StartTaskCreate(void* parameter)
{

        BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值 pdPASS：成功 */

        taskENTER_CRITICAL();        /* 进入临界区，创建任务过程我们必须保证在临界区 */

        /* 动态创建Task1任务 */ 
        xTaskCreate(
                         (TaskFunction_t )Task1,                 /* 任务函数 */
                         (const char*    )"Task1",               /* 任务名称 */
                         (uint32_t       )TASK1_STK_SIZE,        /* 任务堆栈大小 */
                         (void*          )NULL,                  /* 传递给任务函数的参数 */
                         (UBaseType_t    )TASK1_PRIO,            /* 任务优先级 */
                         (TaskHandle_t*  )&Task1_Handle);        /* 任务1的任务句柄 */

        if(xReturn == pdPASS) printf("Task1任务创建成功!\r\n");
        else                  printf("Task1任务创建失败!\r\n");

        /* 动态创建Task2任务 */
        xTaskCreate(
                         (TaskFunction_t )Task2,                 /* 任务函数 */
                         (const char*    )"Task2",               /* 任务名称 */
                         (uint32_t       )TASK2_STK_SIZE,        /* 任务堆栈大小 */
                         (void*          )NULL,                  /* 传递给任务函数的参数 */
                         (UBaseType_t    )TASK2_PRIO,            /* 任务优先级 */
                         (TaskHandle_t*  )&Task2_Handle);        /* 任务2的任务句柄 */  
        if(xReturn == pdPASS) printf("Task2任务创建成功!\r\n");
        else                  printf("Task2任务创建失败!\r\n");

        /* 动态创建Task3任务 */
        xTaskCreate(
                         (TaskFunction_t )Task3,                 /* 任务函数 */
                         (const char*    )"Task3",               /* 任务名称 */
                         (uint32_t       )TASK3_STK_SIZE,        /* 任务堆栈大小 */
                         (void*          )NULL,                  /* 传递给任务函数的参数 */
                         (UBaseType_t    )TASK3_PRIO,            /* 任务优先级 */
                         (TaskHandle_t*  )&Task3_Handle);        /* 任务3的任务句柄 */
        if(xReturn == pdPASS) printf("Task3任务创建成功!\r\n");
        else                  printf("Task3任务创建失败!\r\n");

        vTaskDelete(StartTask_Handler); /* 删除创建任务的任务 */

        taskEXIT_CRITICAL();         /* 退出临界区 */
}

static void Task1(void* parameter)
{
    while(1)
    {
        printf("This is Task1\r\n");
        vTaskDelay(1000); /* 延时1000ms */
    }
}

static void Task2(void* parameter)
{
    while(1)
    {
        printf("This is Task2\r\n");
        vTaskDelay(1000); /* 延时1000ms */
    }
}       

static void Task3(void* parameter)
{
    while(1)
    {
        printf("This is Task3\r\n");
        vTaskDelay(1000); /* 延时1000ms */
    }
}
