
#include "stm32f10x.h"                  // Device header
#include "EXTI.h"
#include "delay.h"
#include "LED.h"
#include "KEY.h"


/**
 * @brief  外部中断初始化函数（EXTI2）
 * @note   配置GPIOE Pin2为外部中断输入，下降沿触发，配置中断优先级并使能
 * @param  无
 * @retval 无
 */
void EXTIX_Init(void)
{
	// 1. 定义结构体变量：EXTI初始化结构体、NVIC初始化结构体
	// EXTI_InitTypeDef：STM32标准库中用于配置外部中断的结构体类型
	// NVIC_InitTypeDef：STM32标准库中用于配置中断优先级的结构体类型
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// 2. 按键端口初始化（需提前实现KEY_Init函数）
	// 作用：将GPIOE Pin2配置为输入模式（上拉/下拉），为外部中断提供硬件基础
	// 注：如果KEY_Init未实现，会报"Undefined symbol KEY_Init"错误
	 KEY_Init();	 //	按键端口初始化

  	// 3. 使能AFIO时钟（必须！）
	// AFIO：复用功能IO，负责GPIO引脚与EXTI线的映射关系
	// STM32中，EXTI中断线与GPIO引脚的映射必须开启AFIO时钟才能生效
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	// 4. 配置GPIO引脚与EXTI中断线的映射关系
	// GPIO_EXTILineConfig(端口源, 引脚源)：将指定GPIO引脚映射到EXTI中断线
	// 参数1：GPIO_PortSourceGPIOE → 选择GPIOE端口
	// 参数2：GPIO_PinSource2 → 选择GPIOE的Pin2引脚
	// 效果：GPIOE Pin2 绑定到 EXTI_Line2 中断线
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource2);
	
	// 5. 配置EXTI中断线参数
	EXTI_InitStruct.EXTI_Line=EXTI_Line2;          // 指定配置EXTI2中断线
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;           // 使能EXTI2中断线
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt; // 中断模式（而非事件模式）
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling; // 下降沿触发（按键按下时电平从高→低）
	
	// 6. 应用EXTI配置（将结构体参数写入寄存器）
	EXTI_Init(&EXTI_InitStruct);
	
	
	
  
	// 7. 配置NVIC（嵌套向量中断控制器）：管理中断优先级和使能
	NVIC_InitStruct.NVIC_IRQChannel=EXTI2_IRQn;    // 指定中断通道：EXTI2中断
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;     // 使能EXTI2中断通道
	// 抢占优先级：2（数值越小优先级越高，范围由NVIC分组决定）
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	// 子优先级：2（抢占优先级相同的情况下，子优先级高的先响应）
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	
	// 8. 应用NVIC配置（将优先级参数写入寄存器）
	NVIC_Init(&NVIC_InitStruct);
	
}
/**
 * @brief  EXTI2外部中断服务函数（对应KEY2按键）
 * @note   当GPIOE Pin2检测到下降沿时，CPU自动进入该函数执行
 * @param  无
 * @retval 无
 */
void EXTI2_IRQHandler(void)
{
	// 1. 软件消抖（关键！解决机械按键的抖动问题）
	// 延时10ms：跳过按键按下时的电平抖动阶段（机械按键按下/松开时电平会抖动5~20ms）
	// 注：需确保delay_ms()函数已实现（基于SysTick或定时器），否则会报未定义错误
	delay_ms(10);//消抖
	
	// 2. 判断KEY2是否真的被按下（排除误触发）
	// KEY2：宏定义/变量，对应GPIOE Pin2的电平状态（0=按下，1=松开）
	// 例：KEY2的典型宏定义：#define KEY2 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)
	if(KEY2==0)	  //按键KEY2被按下（电平为低）
	{
		// 3. 业务逻辑：翻转LED0的状态（0→1 或 1→0）
		// LED0：控制LED亮灭的宏/变量（如#define LED0 GPIO_WriteBit(GPIOA,GPIO_Pin_0, !GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_0))）
		LED0=!LED0; // 取反操作：LED亮→灭，灭→亮
	}		 
	
	// 4. 清除EXTI2中断标志位（必须！否则中断会无限触发）
	// EXTI中断触发后，标志位会被置1，需手动清除，否则CPU会一直认为有中断待处理
	EXTI_ClearITPendingBit(EXTI_Line2);  //清除LINE2上的中断标志位  
}
