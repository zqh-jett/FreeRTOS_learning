#include "rtos_task.h"
#include "key.h"
#include "delay.h"
#include "led.h"
#include "FreeRTOS.h"    // FreeRTOS使用
#include "task.h"       // FreeRTOS使用
#include "queue.h"      // FreeRTOS使用

/**************************************************************************/
/* freeRTOS任务配置 */
/*** 任务创建任务配置
 * 包括: 任务优先级 堆栈大小 任务句柄 创建任务
 */
#define START_PRIO      8                     /* 任务优先级 */
#define START_STK_SIZE  64                    /* 任务堆栈大小，字为单位，1字等于4字节 */
static TaskHandle_t StartTask_Handler = NULL;   /* 创建任务的任务句柄 */
static void StartTaskCreate(void* parameter);   /* 创建任务的任务函数名 */

/*** TASK1任务配置
 * 包括: 任务优先级 堆栈大小 任务句柄 创建任务
 */
#define TASK1_PRIO      3                     /* 任务优先级 */
#define TASK1_STK_SIZE  64                    /* 任务堆栈大小，字为单位，1字等于4字节 */
static TaskHandle_t Task1_Handle = NULL;        /* 任务1的任务句柄 */
static void Task1(void* parameter);             /* 任务1的任务函数名 */

/*** TASK2任务配置
 * 包括: 任务优先级 堆栈大小 任务句柄 创建任务
 */
#define TASK2_PRIO      1                     /* 任务优先级 */
#define TASK2_STK_SIZE  64                    /* 任务堆栈大小，字为单位，1字等于4字节 */
static TaskHandle_t Receive_Task_Handle = NULL; /* 接收任务句柄 */
static void Receive_Task(void* parameter);    /* 任务2的任务函数名 */

/*** TASK3任务配置
 * 包括: 任务优先级 堆栈大小 任务句柄 创建任务
 */
#define TASK3_PRIO      2                     /* 任务优先级 */
#define TASK3_STK_SIZE  64                    /* 任务堆栈大小，字为单位，1字等于4字节 */
static TaskHandle_t Send_Task_Handle = NULL;    /* 发送任务句柄 */
static void Send_Task(void* parameter);         /* 任务3的任务函数名 */


QueueHandle_t Test_Queue = NULL;              // 定义一个队列句柄

char buff[100] = {"大askdfhjka"};            // 缓冲区定义，未被使用

/**************************************************
 *          FreeRTOS任务创建
 *
 *   这个函数是专门用来创建任务的，
 *   我们在这个函数里面创建我们需要的任务函数，用完删除此函数。
 ************************************************/
void freertos_demo(void) {
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值 pdPASS：成功 */

    // 创建队列，队列长度为10，队列中每个元素的大小为4字节（uint32_t）
    Test_Queue = xQueueCreate(10, sizeof(uint32_t));
    if (Test_Queue != NULL)
        printf("队列创建成功!\r\n");
    else
        printf("队列创建失败!\r\n");

    /* 动态创建创建任务的任务 */
    xReturn = xTaskCreate(
        (TaskFunction_t)StartTaskCreate,     /* 任务函数 */
        (const char *)"StartTaskCreate",      /* 任务名称 */
        (uint32_t)START_STK_SIZE,            /* 任务堆栈大小 */
        (void *)NULL,                        /* 传递给任务函数的参数 */
        (UBaseType_t)START_PRIO,             /* 任务优先级 */
        (TaskHandle_t *)&StartTask_Handler); /* 任务句柄 */

    if (xReturn == pdPASS)
        printf("StartTaskCreate任务创建成功!\r\n");
    else
        printf("StartTaskCreate任务创建失败!\r\n");

    vTaskStartScheduler();  /* 启动任务调度器 */
}

/**************************************************
 *              创建任务的任务函数
 *
 *    1. xQueueCreate(10, sizeof(uint32_t));  // 创建一个队列，队列长度为10，队列中每个元素的大小为4字节
 *    2. vQueueDelete(Test_Queue); // 删除一个队列
 *           这个任务创建函数是专门用来创建任务函数的，
 *          我们在这个函数里面创建我们需要的任务函数，用完删除此任务。
 ************************************************/
static void StartTaskCreate(void* parameter) {
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值 pdPASS：成功 */
    taskENTER_CRITICAL();        /* 进入临界区，创建任务过程必须保证在临界区 */

    /* 动态创建Task1任务 */
    xReturn = xTaskCreate(
        (TaskFunction_t)Task1,                 /* 任务函数 */
        (const char *)"Task1",                 /* 任务名称 */
        (uint32_t)TASK1_STK_SIZE,              /* 任务堆栈大小 */
        (void *)NULL,                          /* 传递给任务函数的参数 */
        (UBaseType_t)TASK1_PRIO,               /* 任务优先级 */
        (TaskHandle_t *)&Task1_Handle);        /* 任务1的任务句柄 */

    if (xReturn == pdPASS)
        printf("Task1任务创建成功!\r\n");
    else
        printf("Task1任务创建失败!\r\n");

    /* 动态创建Task2任务 */
    xReturn = xTaskCreate(
        (TaskFunction_t)Receive_Task,          /* 任务函数 */
        (const char *)"Receive_Task",           /* 任务名称 */
        (uint32_t)TASK2_STK_SIZE,              /* 任务堆栈大小 */
        (void *)NULL,                          /* 传递给任务函数的参数 */
        (UBaseType_t)TASK2_PRIO,               /* 任务优先级 */
        (TaskHandle_t *)&Receive_Task_Handle); /* 任务2的任务句柄 */

    if (xReturn == pdPASS)
        printf("Receive_Task任务创建成功!\r\n");
    else
        printf("Receive_Task任务创建失败!\r\n");

    /* 动态创建Task3任务 */
    xReturn = xTaskCreate(
        (TaskFunction_t)Send_Task,             /* 任务函数 */
        (const char *)"Send_Task",              /* 任务名称 */
        (uint32_t)TASK3_STK_SIZE,              /* 任务堆栈大小 */
        (void *)NULL,                          /* 传递给任务函数的参数 */
        (UBaseType_t)TASK3_PRIO,               /* 任务优先级 */
        (TaskHandle_t *)&Send_Task_Handle);    /* 任务3的任务句柄 */

    if (xReturn == pdPASS)
        printf("Send_Task任务创建成功!\r\n");
    else
        printf("Send_Task任务创建失败!\r\n");

    vTaskDelete(StartTask_Handler); /* 删除创建任务的任务 */
    taskEXIT_CRITICAL();              /* 退出临界区 */
}

/**************************************************
 * 队列发送和接收
 *
 * 这段代码确保发送和接收的数据类型与队列中每个元素的大小一致。
 * 由于队列元素大小为4字节（uint32_t），因此使用uint32_t类型。
 ************************************************/
static void Task1(void* parameter) {
    printf("Task1任务正在运行!\r\n");

    while(1) {
        LED0 = !LED0;       // 切换LED0的状态
        delay_ms(500);        // 延时500毫秒
    }
}

static void Receive_Task(void* parameter) {
    BaseType_t xReturn = pdTRUE;
    uint32_t r_queue;     // 使用uint32_t类型与队列元素大小一致

    printf("Receive_Task任务正在运行!\r\n");

    while(1) {
        xReturn = xQueueReceive(
            Test_Queue,    /* 消息队列的句柄 */
            &r_queue,      /* 接收缓冲区，注意这里取了地址 */
            portMAX_DELAY); /* 等待时间 一直等 */

        if (pdTRUE == xReturn)
            printf("本次接收到的数据是%d\r\n", r_queue);  // 使用%d格式说明符，并传递变量而不是其地址
        else
            printf("数据接收出错,错误代码0x%lx\n", xReturn); // 输出错误代码

        delay_ms(1000);       // 延时1000毫秒
    }
}

static void Send_Task(void* parameter) {
    uint8_t key = 0;
    BaseType_t xReturn = pdPASS;
    uint32_t send_data1 = 1;  // 使用uint32_t类型
    uint32_t send_data2 = 2;  // 使用uint32_t类型

    printf("Send_Task任务正在运行!\r\n");

    while (1) {
        key = KEY_Scan(0);   // 扫描按键
        if (key == 1) {      // K1 被按下
            printf("发送消息send_data1!!\r\n");
            xReturn = xQueueSend(
                Test_Queue, /* 消息队列的句柄 */
                &send_data1,/* 发送的消息内容，注意这里取了地址 */
                0);         /* 等待时间 0 */

            if (pdPASS == xReturn)
                printf("消息send_data1发送成功 %d! \r\n", send_data1);
        }
        if (key == 2) {      // K2 被按下
            printf("发送消息send_data2! \r\n");
            xReturn = xQueueSend(
                Test_Queue, /* 消息队列的句柄 */
                &send_data2,/* 发送的消息内容，注意这里取了地址 */
                0);         /* 等待时间 0 */

            if (pdPASS == xReturn)
                printf("消息send_data2发送成功 %d! \r\n", send_data2);
        }
        delay_ms(500);        // 延时500毫秒
    }
}
