/*
 * main_IOT2020.c
 *
 *  Created on: 23 mar 2020
 *      Author: flavi
 */


/* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"
#include "acqData.h"

#define mainINTERRUPT_NUMBER	3

static void vInterruptGenerator( void *pvParameters );
void ISRdeferredTask( void *pvParameters );
uint32_t vHeart_chol_param_ISR( void );

void main_IOT2020(void)
{
	printf("main_IOT2020 is running with heap_4...\r\n");

	xBinarySemaphore = xSemaphoreCreateBinary();
	xHeart_EventGroup = xEventGroupCreate();
	for(int i=0; i<14; i++){ xQueue[i] = xQueueCreate( 5, sizeof( int ) ); }

	xTaskCreate( vHeart_1_3_param, "Heart age -sex- cp params", 1000, NULL, 1, NULL );
	xTaskCreate( vHeart_5_14_param, "Heart 5 to 14 param", 1000, NULL, 1, NULL );

	xTaskCreate( vHeart_ReadData, "Read Data", 1000, NULL, 2, NULL );

	xTaskCreate( ISRdeferredTask, "Heart trestbps param", 1000, NULL, 1, NULL);
	xTaskCreate( vInterruptGenerator, "Interrupt Generator", 1000, NULL, 2, NULL );
    vPortSetInterruptHandler( mainINTERRUPT_NUMBER, vHeart_chol_param_ISR );

    vTaskStartScheduler();

    for( ;; );
    return 0;
}

uint32_t vHeart_chol_param_ISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void ISRdeferredTask( void *pvParameters )
{
    int data;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );

    static const char *pcString = "Bit setting ISR -\t about to set vHeart_chol_param_ISR.\r\n";
    for( ;; )
    {
        /* Use the semaphore to wait for the event. The semaphore was created before the scheduler was started, so before this
         * task ran for the first time. The task blocks indefinitely, meaning this function call will only return once the semaphore
         * has been successfully obtained so there is no need to check the value returned by xSemaphoreTake(). */
        xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );
        data = rand()%2;
    	//vPrintString( ( const char * ) pcString );
        // Set bit 2 in the event group.
		xQueueSendToBack(xQueue[3], &data, xTicksToWait);
		xEventGroupSetBits( xHeart_EventGroup, vH_4_pTASK_BIT);
    }
}

static void vInterruptGenerator( void *pvParameters )
{
	TickType_t xLastExecutionTime;
	const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );
	/* Initialize the variable used by the call to vTaskDelayUntil(). */
	xLastExecutionTime = xTaskGetTickCount();

	for( ;; )
	{
		/* This is a periodic task.  Block until it is time to run again.
		The task will execute every 500ms. */
		vTaskDelayUntil( &xLastExecutionTime, xDelay500ms );

		/* Generate the interrupt that will set a bit in the event group. */
		vPortGenerateSimulatedInterrupt( mainINTERRUPT_NUMBER );
	}
}
