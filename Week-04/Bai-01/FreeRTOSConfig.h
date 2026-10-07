#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32f10x.h"

/* Clock của STM32F103 */
#define configCPU_CLOCK_HZ              ( ( unsigned long ) 72000000 )
#define configUSE_16_BIT_TICKS          0
/* Tick FreeRTOS: 1000 tick = 1 giây */
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )

/* Số task tối đa */
#define configMAX_PRIORITIES             5

/* Kích thước heap */
#define configTOTAL_HEAP_SIZE            ( ( size_t ) ( 10 * 1024 ) )

/* Kích thước stack mặc định */
#define configMINIMAL_STACK_SIZE         ( ( unsigned short ) 128 )

/* Tên task tối đa 16 ký tự */
#define configMAX_TASK_NAME_LEN          16

/* Cho phép cấp phát động */
#define configSUPPORT_DYNAMIC_ALLOCATION 1

/* Cho phép cấp phát tĩnh */
#define configSUPPORT_STATIC_ALLOCATION  0

/* Preemption: chuyển task theo độ ưu tiên */
#define configUSE_PREEMPTION             1

/* Cho phép Idle Hook */
#define configUSE_IDLE_HOOK              0

/* Cho phép Tick Hook */
#define configUSE_TICK_HOOK              0

/* Cho phép Mutex */
#define configUSE_MUTEXES                1

/* Cho phép Semaphore */
#define configUSE_COUNTING_SEMAPHORES    1

/* Không dùng timer software */
#define configUSE_TIMERS                 0

/* Các hàm API cần thiết */
#define INCLUDE_vTaskDelay               1
#define INCLUDE_vTaskDelete              1
#define INCLUDE_vTaskSuspend             1
#define INCLUDE_vTaskDelayUntil          1

/* Cortex-M3 sử dụng 4 bit priority */
#define configPRIO_BITS                  4

/* Priority của SysTick và PendSV */
#define configKERNEL_INTERRUPT_PRIORITY  ( 15 << ( 8 - configPRIO_BITS ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( 5 << ( 8 - configPRIO_BITS ) )

#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler
#endif
