// =============================================================================
//  Program : rtos_test.c
//  Author  : Chun-Jen Tsai
//  Date    : Dec/11/2021
// -----------------------------------------------------------------------------
//  Description:
//  This is a multi-thread program to demo the usage of FreeRTOS and shared
//  resource protection using a mutex.
//
//  This program is designed as one of the homework project for the course:
//  Microprocessor Systems: Principles and Implementation
//  Dept. of CS, NYCU (aka NCTU), Taiwan.
// -----------------------------------------------------------------------------
//  Revision information:
//
//  None.
// =============================================================================

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

void Task1_Handler(void *pvParameters);
void Task2_Handler(void *pvParameters);

#define COUNTER_LIMIT 10000
#define USE_MUTEX 1

#if USE_MUTEX
xSemaphoreHandle xMutex; // a mutex used to protect shared variable.
#endif

int shared_counter = 0;
int task2_finished = 0;

void vPrintString(char *s)
{
    /* Print a string, the UART device is protected in critical section. */
    taskENTER_CRITICAL();
    printf("%s", s);
    taskEXIT_CRITICAL();
}

void vPrintNumber(int n)
{
    /* Print a string, the UART device is protected in critical section. */
    taskENTER_CRITICAL();
    printf("%d", n);
    taskEXIT_CRITICAL();
}

// --------------------------------------------------------------------
//  The following function computes 1000 digits of pi.
//  We use it here merely as a busy-computing filler code.
//
const int t = 1e3;
int pi[336];

void pi_calc(int k, int a[])
{
    int p, q, j;

    if (k >= 0)
    {
        a[q = 0] += 2;
        p = 1 + 2 * k;
        for (j = 0; j < 336; j++)
        {
            // if (k == 0 && j == 0) printf("3.");
            // if (k == (j>2)) printf("%.3d", a[j-2] % t + q / p / t);
            q = a[j] * k + q % p * t;
            a[j] = q / p;
        }
    }
}
//  End of pi calculation.
// --------------------------------------------------------------------

int main(void)
{
#if USE_MUTEX
    xMutex = xSemaphoreCreateMutex();
    if (xMutex == NULL) return 1;
#endif

    xTaskCreate(Task1_Handler, "Task1", 256, NULL, 3, NULL);
    xTaskCreate(Task2_Handler, "Task2", 256, NULL, 3, NULL);

    vTaskStartScheduler();
    return 0;
}

void Task1_Handler(void *pvParameters)
{
    int task1_counter = 0;
    int done = 0, iter = 4e3;

    vPrintString("\nTask 1 start running ...\n");

    while (! done)
    {
        /* Only one thread can modify the shared variable at one time */
#if USE_MUTEX
        // xSemaphoreTake(xMutex, portMAX_DELAY);
        asm volatile ("lui t0,%hi(xMutex)");
        asm volatile ("lw t3,%lo(xMutex)(t0)");
        asm volatile ("li t0,1");
        asm volatile ("0:");
        asm volatile ("lw t1, (t3)");
        asm volatile ("bnez t1,0b");
        asm volatile ("amoswap.w.aq t1, t0, (t3)");
        asm volatile ("bnez t1,0b");
#endif
        done = (shared_counter >= COUNTER_LIMIT);
        if (!done)
        {
            shared_counter++;
            task1_counter++;
        }
#if USE_MUTEX
        // xSemaphoreGive(xMutex);
        asm volatile ("lui t0,%hi(xMutex)");
        asm volatile ("lw t3,%lo(xMutex)(t0)");
        asm volatile ("amoswap.w.aq t1, t0, (t3)");

#endif
        pi_calc(iter--, pi); // Busy computations of PI.
    }

    while (!task2_finished) vTaskDelay(1); /* busy waiting */

    vPrintString("\n\nAt the end, the shared counter = ");
    vPrintNumber(shared_counter);
    vPrintString("\nTask1 local counter = ");
    vPrintNumber(task1_counter);
    vPrintString("\nTask2 local counter = ");
    vPrintNumber(task2_finished);
    if (shared_counter != (task1_counter + task2_finished))
        vPrintString("\nTask1 counter + Task2 counter != Shared counter, the counter is corrupted.\n");
    else if (task1_counter == 0 || task2_finished == 0)
        vPrintString("\nOnly one of the threads accesses the shared counter, not good.\n");
    else
    	vPrintString("\nThe shared counter is protected well.\n");

    /* The thread has ended, we must delete this task from the task queue. */
    vTaskDelete(NULL);
}

void Task2_Handler(void *pvParameters)
{
    int task2_counter = 0;
    int done = 0;

    /* Wait a little bit for Task 1 to get a head start. */;
    vTaskDelay(1);

    vPrintString("\nTask 2 start running ...\n");

    while (!done)
    {
        /* Only one thread can modify the shared variable at one time */
#if USE_MUTEX
        // xSemaphoreTake(xMutex, portMAX_DELAY);
        asm volatile ("lui t0,%hi(xMutex)");
        asm volatile ("lw t3,%lo(xMutex)(t0)");
        asm volatile ("li t0,1");
        asm volatile ("0:");
        asm volatile ("lw t1, (t3)");
        asm volatile ("bnez t1,0b");
        asm volatile ("amoswap.w.aq t1, t0, (t3)");
        asm volatile ("bnez t1,0b");
#endif
        done = (shared_counter >= COUNTER_LIMIT);
        if (!done)
        {
            shared_counter++;
            task2_counter++;
        }
#if USE_MUTEX
        // xSemaphoreGive(xMutex);
        asm volatile ("lui t0,%hi(xMutex)");
        asm volatile ("lw t3,%lo(xMutex)(t0)");
        asm volatile ("amoswap.w.aq t1, t0, (t3)");
#endif
    }

    /* The thread has ended, we must delete this task from the task queue. */
    task2_finished = task2_counter;
    vTaskDelete(NULL);
}

void vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
       configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
       function that will get called if a call to pvPortMalloc() fails.
       pvPortMalloc() is called internally by the kernel whenever a task, queue,
       timer or semaphore is created.  It is also called by various parts of the
       demo application.  If heap_1.c or heap_2.c are used, then the size of the
       heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
       FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
       to query the size of free heap space that remains (although it does not
       provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    for (;;);
}

void vApplicationIdleHook(void)
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
       to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
       task.  It is essential that code added to this hook function never attempts
       to block in any way (for example, call xQueueReceive() with a block time
       specified, or call vTaskDelay()).  If the application makes use of the
       vTaskDelete() API function (as this demo application does) then it is also
       important that vApplicationIdleHook() is permitted to return to its calling
       function, because it is the responsibility of the idle task to clean up
       memory allocated by the kernel to any task that has since been deleted. */
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void) pcTaskName;
    (void) pxTask;

    /* Run time stack overflow checking is performed if
       configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
       function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    printf("Stack overflow error.\n");
    for (;;);
}

void vApplicationTickHook(void)
{
    /* vApplicationTickHook */
}

void vAssertCalled(void)
{
    taskDISABLE_INTERRUPTS();
    while (1)
    {
        __asm volatile ("NOP");
    }
}

void vExternalISR( uint32_t cause )
{
}

