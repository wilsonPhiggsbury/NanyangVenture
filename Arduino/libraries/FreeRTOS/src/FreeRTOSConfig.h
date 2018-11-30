/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <avr/io.h>

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

// And on to the things the same no matter the AVR type...
#define configUSE_PREEMPTION                1
#define configUSE_IDLE_HOOK                 1
#define configUSE_TICK_HOOK                 0
#define configCPU_CLOCK_HZ                  ( ( uint32_t ) F_CPU )			// This F_CPU variable set by the environment
#define configMAX_PRIORITIES                4
#define configMINIMAL_STACK_SIZE            ( ( UBaseType_t ) 85 )
#define configIDLE_STACK_SIZE               ( ( UBaseType_t ) 192 )
#define configMAX_TASK_NAME_LEN             ( 8 )
#define configUSE_TRACE_FACILITY            0
#define configUSE_16_BIT_TICKS              1
#define configIDLE_SHOULD_YIELD             1

#define configUSE_MUTEXES                   1
#define configUSE_RECURSIVE_MUTEXES         1
#define configUSE_COUNTING_SEMAPHORES       1
#define configUSE_QUEUE_SETS                0
#define configQUEUE_REGISTRY_SIZE           0
#define configUSE_TIME_SLICING              1
#define configCHECK_FOR_STACK_OVERFLOW      1
#define configUSE_MALLOC_FAILED_HOOK        1

#define configSUPPORT_DYNAMIC_ALLOCATION    1
#define configSUPPORT_STATIC_ALLOCATION     0
#define configTOTAL_HEAP_SIZE				2000

/* Timer definitions. */
#define configUSE_TIMERS                    1
#define configTIMER_TASK_PRIORITY           ( ( UBaseType_t ) 3 )
#define configTIMER_QUEUE_LENGTH            ( ( UBaseType_t ) 10 )
#define configTIMER_TASK_STACK_DEPTH        configMINIMAL_STACK_SIZE

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES               0
#define configMAX_CO_ROUTINE_PRIORITIES     ( (UBaseType_t ) 2 )

/* Set the stack pointer type to be uint16_t, otherwise it defaults to unsigned long */
#define portPOINTER_SIZE_TYPE			uint16_t

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          0
#define INCLUDE_xTaskGetIdleTaskHandle          0 // create an idle task handle.
#define INCLUDE_xTaskGetCurrentTaskHandle       0
#define INCLUDE_uxTaskGetStackHighWaterMark     1

// Debugging
/* Set configGENERATE_RUN_TIME_STATS to 1 to enable collection of run-time statistics.  
When this is done, both portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() and portGET_RUN_TIME_COUNTER_VALUE() or portALT_GET_RUN_TIME_COUNTER_VALUE(x) 
must also be defined. */ 
//#define configGENERATE_RUN_TIME_STATS 1 
 
/* portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() is defined to call the function that sets  up the hypothetical 16-bit timer 
(the function’s implementation is not shown). */ 
//void vSetupTimerForRunTimeStats( void ); 
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vSetupTimerForRunTimeStats() 
 
/* portALT_GET_RUN_TIME_COUNTER_VALUE() is defined to set its parameter to the current run-time counter/time value.  
The returned time value is 32-bits long, and is formed by shifting the count of 16-bit timer overflows into the top two bytes of a 32-bit number, 
then bitwise ORing the result with the current 16-bit counter  value. */ 
//#define portALT_GET_RUN_TIME_COUNTER_VALUE( ulCountValue )                   \     {                                                                        \     extern volatile unsigned long ulOverflowCount;                           \                                                                              \         /* Disconnect the clock from the counter so it does not change       \         while its value is being used. */                                    \         PauseTimer();                                                        \                                                                              \         /* The number of overflows is shifted into the most significant      \         two bytes of the returned 32-bit value. */                           \         ulCountValue = ( ulOverflowCount << 16UL );                          \                                                                              \         /* The current counter value is used as the least significant        \         two bytes of the returned 32-bit value. */                           \         ulCountValue |= ( unsigned long ) ReadTimerCount();                  \                                                                              \         /* Reconnect the clock to the counter. */                            \         ResumeTimer();                                                       \     }

/* for resuming tasks early */
#define INCLUDE_xTaskAbortDelay					1
#define INCLUDE_eTaskGetState					1
#endif /* FREERTOS_CONFIG_H */
