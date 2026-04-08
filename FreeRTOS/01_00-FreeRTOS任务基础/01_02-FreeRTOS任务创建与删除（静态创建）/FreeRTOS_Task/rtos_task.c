#include "rtos_task.h"
#include "FreeRTOS.h"					//FreeRTOS使用
#include "task.h"						//FreeRTOS使用
#include "timers.h"	
#include "FreeRTOSConfig.h"


/**************************************************************************/
/* freeRTOS任务配置 */



/***
* 任务创建任务配置
* 包括: 任务优先级 堆栈大小 任务堆栈 任务控制块 任务句柄 任务函数
*/
#define START_PRIO      1                               /* 任务优先级 */
#define START_STK_SIZE  64                              /* 任务堆栈大小，字为单位，1字等于4字节 */
static StaticTask_t StartTaskTCB;                       /* 任务控制块 */
static StackType_t StartTaskStack[START_STK_SIZE];      /* 任务堆栈 */
static TaskHandle_t StartTask_Handler=NULL;             /* 创建任务的任务句柄 */
static void StartTaskCreate(void* parameter);           /* 创建任务的任务函数名 */

/***
* TASK1任务配置
* 包括: 任务优先级 堆栈大小 任务堆栈 任务控制块 任务句柄 任务函数
*/
#define Task1_PRIO      1                                /* 任务优先级 */
#define Task1_STK_SIZE  64                               /* 任务堆栈大小，字为单位，1字等于4字节 */
static StaticTask_t Task1TaskTCB;                        /* 任务控制块 */
static StackType_t  Task1TaskStack[Task1_STK_SIZE];      /* 任务堆栈 */
static TaskHandle_t Task1_Handler=NULL;                  /* 创建任务的任务句柄 */
static void Task1(void* parameter);                      /* 创建任务的任务函数名 */
 
/***
* TASK2任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define Task2_PRIO      2                                /* 任务优先级 */
#define Task2_STK_SIZE  64                               /* 任务堆栈大小，字为单位，1字等于4字节 */
static StaticTask_t Task2TaskTCB;                        /* 任务控制块 */
static StackType_t  Task2TaskStack[Task2_STK_SIZE];      /* 任务堆栈 */
static TaskHandle_t Task2_Handler=NULL;                  /* 创建任务的任务句柄 */
static void Task2(void* parameter);                      /* 创建任务的任务函数名 */

/***
* TASK3任务配置
* 包括: 任务优先级 堆栈大小 任务句柄 创建任务
*/
#define Task3_PRIO      3                                /* 任务优先级 */
#define Task3_STK_SIZE  64                               /* 任务堆栈大小，字为单位，1字等于4字节 */
static StaticTask_t Task3TaskTCB;                        /* 任务控制块 */
static StackType_t  Task3TaskStack[Task3_STK_SIZE];      /* 任务堆栈 */
static TaskHandle_t Task3_Handler=NULL;                  /* 创建任务的任务句柄 */
static void Task3(void* parameter);                      /* 创建任务的任务函数名 */

/**************************************************************************/


/***
* 空闲任务和定时器服务任务配置
* 包括: 任务堆栈 任务控制块
*/
static StaticTask_t IdleTaskTCB;                                    /* 空闲任务控制块 */
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];         /* 空闲任务堆栈 */

static StaticTask_t TimerTaskTCB;                                   /* 定时器服务任务控制块 */
static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];    /* 定时器服务任务堆栈 */

/**********************************************************
* @funcName ：vApplicationGetIdleTaskMemory
*    获取空闲任务地任务堆栈和任务控制块内存
*    ppxIdleTaskTCBBuffer  (任务控制块内存)
*    ppxIdleTaskStackBuffer(任务堆栈内存)
*    pulIdleTaskStackSize  (任务堆栈大小)
* 
*               获取空闲任务地任务堆栈和任务控制块内存，因为本例程使用的
*           静态内存，因此空闲任务的任务堆栈和任务控制块的内存就应该有用
*           户来提供，FreeRTOS提供了接口函数vApplicationGetIdleTaskMemory()
*           实现此函数即可。
************************************************************/
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer, 
                                    uint32_t *pulIdleTaskStackSize )
{
        *ppxIdleTaskTCBBuffer=&IdleTaskTCB; /* 任务控制块 */
        *ppxIdleTaskStackBuffer=IdleTaskStack; /* 任务堆栈 */
        *pulIdleTaskStackSize=configMINIMAL_STACK_SIZE; /* 任务堆栈大小 */
}
/**********************************************************************
*funcName ：vApplicationGetTimerTaskMemory
*           获取定时器服务任务的任务堆栈和任务控制块内存
*           有一个宏定义需要注意：configSUPPORT_STATIC_ALLOCATION这个宏定义如果设置为1，表示支持静态内存分配
*            ppxTimerTaskTCBBuffer    任务控制块内存
*            ppxTimerTaskStackBuffer  任务堆栈内存
*            pulTimerTaskStackSize    任务堆栈大小
*            void
*
*               获取定时器服务任务的任务堆栈和任务控制块内存，因为本例程使用的
*           静态内存，因此定时器服务任务的任务堆栈和任务控制块的内存就应该有用
*           户来提供，FreeRTOS提供了接口函数vApplicationGetTimerTaskMemory()
*           实现此函数即可。
************************************************************************/
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t  **ppxTimerTaskStackBuffer,
                                    uint32_t     *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer  = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer= TimerTaskStack;
    *pulTimerTaskStackSize  = configTIMER_TASK_STACK_DEPTH;
}




/**********************************************************


************************************************************/
void freertos_demo(void)
{
        /* 静态创建创建任务的任务 */
        StartTask_Handler = xTaskCreateStatic(
                                            (TaskFunction_t   )StartTaskCreate,   /* 任务函数 */
                                            (const char*      )"StartTaskCreate", /* 任务名称 */
                                            (uint32_t         )START_STK_SIZE,    /* 任务堆栈大小 */
                                            (void*            )NULL,              /* 传递给任务函数的参数 */
                                            (UBaseType_t      )START_PRIO,        /* 任务优先级 */
                                            (StackType_t*     )StartTaskStack,    /* 任务堆栈 */
                                            (StaticTask_t*    )&StartTaskTCB);    /* 任务控制块 */

        if(StartTask_Handler != NULL) printf("StartTaskCreate任务创建成功!\r\n");
        else                          printf("StartTaskCreate任务创建失败!\r\n");
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


        taskENTER_CRITICAL();        /* 进入临界区，创建任务过程我们必须保证在临界区 */

        /* 静态创建创建task1的任务 */
        Task1_Handler = xTaskCreateStatic(
                                            (TaskFunction_t   )Task1,   /* 任务函数 */
                                            (const char*      )"Task1", /* 任务名称 */
                                            (uint32_t         )Task1_STK_SIZE,    /* 任务堆栈大小 */
                                            (void*            )NULL,              /* 传递给任务函数的参数 */
                                            (UBaseType_t      )Task1_PRIO,        /* 任务优先级 */
                                            (StackType_t*     )Task1TaskStack,    /* 任务堆栈 */
                                            (StaticTask_t*    )&Task1TaskTCB);    /* 任务控制块 */

        if(Task1_Handler != NULL) printf("Task1任务创建成功!\r\n");
        else                          printf("Task1任务创建失败!\r\n");
        /* 静态创建创建task2的任务 */
        Task2_Handler = xTaskCreateStatic(
                                            (TaskFunction_t   )Task2,   /* 任务函数 */
                                            (const char*      )"Task2", /* 任务名称 */
                                            (uint32_t         )Task2_STK_SIZE,    /* 任务堆栈大小 */
                                            (void*            )NULL,              /* 传递给任务函数的参数 */
                                            (UBaseType_t      )Task2_PRIO,        /* 任务优先级 */
                                            (StackType_t*     )Task2TaskStack,    /* 任务堆栈 */
                                            (StaticTask_t*    )&Task2TaskTCB);    /* 任务控制块 */

        if(Task2_Handler != NULL) printf("Task2任务创建成功!\r\n");
        else                          printf("Task2任务创建失败!\r\n");

        /* 静态创建创建task3的任务 */
        Task3_Handler = xTaskCreateStatic(
                                            (TaskFunction_t   )Task3,   /* 任务函数 */
                                            (const char*      )"Task3", /* 任务名称 */
                                            (uint32_t         )Task3_STK_SIZE,    /* 任务堆栈大小 */
                                            (void*            )NULL,              /* 传递给任务函数的参数 */
                                            (UBaseType_t      )Task3_PRIO,        /* 任务优先级 */
                                            (StackType_t*     )Task3TaskStack,    /* 任务堆栈 */
                                            (StaticTask_t*    )&Task3TaskTCB);    /* 任务控制块 */

        if(Task3_Handler != NULL) printf("Task3任务创建成功!\r\n");
        else                          printf("Task3任务创建失败!\r\n");

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
