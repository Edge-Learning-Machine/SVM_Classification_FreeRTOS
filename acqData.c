
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
#include "SVM.h"
#include "preprocess.h"

void vHeart_ReadData( void *pvParameters )
{
    float X_test[14];
    int xReceived[14];
    BaseType_t xStatus;
    EventBits_t xEventGroupValue;
    const EventBits_t xBitsToWaitFor = ( vH_1_3_pTASK_BIT | vH_4_pTASK_BIT | vH_5_14_pTASK_BIT);
    for( ;; )
    {
        /* Block to wait for event bits to become set within the event group.*/
        xEventGroupValue = xEventGroupWaitBits( /* The event group to read. */ xHeart_EventGroup,
        										/* Bits to test. */  xBitsToWaitFor,
												/* Clear bits on exit if the unblock condition is met. */ pdTRUE,
												/* Don't wait for all bits. This parameter is set to pdTRUE for the second execution. */ pdTRUE,
												/* Don't time out. */ portMAX_DELAY );

        /* Print a message for each bit that was set. */
        if( ( xEventGroupValue & vH_1_3_pTASK_BIT ) != 0 )
        {
            //vPrintString( "Bit reading task -\t Event vH_1_3_pTASK_BIT was set\r\n" );
        }
        if( ( xEventGroupValue & vH_4_pTASK_BIT ) != 0 )
        {
            //vPrintString( "Bit reading task -\t Event vH_4_pTASK_BIT was set\r\n" );
        }
        if( ( xEventGroupValue & vH_5_14_pTASK_BIT ) != 0 )
        {
            //vPrintString( "Bit reading task -\t Event vH_5_14_pTASK_BIT was set by ISR\r\n" );
        }
        for(int i=0; i<14; i++)
        {
			xStatus = xQueueReceive( xQueue[i], &xReceived[i], pdMS_TO_TICKS( 100 ) );
			if( xStatus == pdPASS )
			{
				vPrintStringAndNumber( "Queue[] = ", xReceived[i] );
				X_test[i] += xReceived[i];
			}
			else
			{
				//this should never happens because we the data are sync with the EventGroup
				vPrintString("vHeart_ReadData - Can't read the Queue\r\n");
			}
        }
		vPrintString("The Data are ready, sending to ELM for classification!!\r\n");
        vPrintString("******************************************************\r");

        float *X_t = preprocess(X_test);
        int class = svm_classification(X_t);
        vPrintStringAndNumber( "Class = ", class );
        //here you can call a function to classify the data with ELM
    }
}

void vHeart_1_3_param( void *pvParameters )
{
    BaseType_t xStatus[3];
    const TickType_t xDelay200ms = pdMS_TO_TICKS( 200UL );
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 200 );
    int data[3];
    int xReceived;


    for( ;; )
    {
        /* Delay for a short while before starting the next loop. */
        vTaskDelay( xDelay200ms );

        //generate random data
        data[0] = 29+rand()%(78-29);
        data[1] = rand()%2;
        data[2] = rand()%4;
        /* Send to the queue. The second parameter is the address of the structure being sent. The address is passed in as the task
         * parameter so pvParameters is used directly. The third parameter is the Block time, the time the task should be kept in
         * the Blocked state to wait for space to become available on the queue if the queue is already full. A block time is specified
         * because the sending tasks have a higher priority than the receiving task so the queue is expected to become full. The receiving
         * task will remove items from the queue when both sending tasks are in the Blocked state. */
        for(int i=0; i<3; i++){ xStatus[i] = xQueueSendToBack( xQueue[i], &data[i], xTicksToWait ); }

        if( xStatus[0] && xStatus[1] && xStatus[2] )
        {
        	for(int i=0; i<3; i++){ xQueueReceive( xQueue[i], &xReceived, pdMS_TO_TICKS( 100 ) ); }
            for(int i=0; i<3; i++){ xStatus[i] = xQueueSendToBack( xQueue[i], &data[i], xTicksToWait );}
        	//vPrintString("the Queue 1_3 were full.. freed and saved new data\r");
        }
		//vPrintString( "Bit setting task -\t about to set vH_1_3_pTASK_BIT.\r\n" );
		xEventGroupSetBits( xHeart_EventGroup, vH_1_3_pTASK_BIT );
    }
}

void vHeart_5_14_param( void *pvParameters )
{
    BaseType_t xStatus[10];
    const TickType_t xDelay800ms = pdMS_TO_TICKS( 800UL );
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
    int data[10];
    int xReceived;

    for( ;; )
    {
        /* Delay for a short while before starting the next loop. */
        vTaskDelay( xDelay800ms );

        //generate random data
        data[0] = 126+rand()%(565-126);
        data[1] = rand()%2;
        data[2] = rand()%3;
        data[3] = 71+rand()%(203-71);
        data[4] = rand()%2;
        data[5] = rand()%63;
        data[6] = rand()%3;
        data[7] = rand()%5;
        data[8] = rand()%4;
        data[9] = rand()%2;
        /* Send to the queue. The second parameter is the address of the structure being sent. The address is passed in as the task
         * parameter so pvParameters is used directly. The third parameter is the Block time, the time the task should be kept in
         * the Blocked state to wait for space to become available on the queue if the queue is already full. A block time is specified
         * because the sending tasks have a higher priority than the receiving task so the queue is expected to become full. The receiving
         * task will remove items from the queue when both sending tasks are in the Blocked state. */
        for(int i=0; i<10; i++){ xStatus[i] = xQueueSendToBack( xQueue[i+4], &data[i], xTicksToWait ); }

        if( xStatus[0] && xStatus[1] && xStatus[2] && xStatus[3] && xStatus[4] && xStatus[5] && xStatus[6] && xStatus[7] && xStatus[8] && xStatus[9] )
        {
        	for(int i=0; i<10; i++){ xQueueReceive( xQueue[i+4], &xReceived, pdMS_TO_TICKS( 100 ) ); }
            for(int i=0; i<10; i++){ xStatus[i] = xQueueSendToBack( xQueue[i+4], &data[i], xTicksToWait ); }
        	//vPrintString("the Queue 5_13 were full.. freed and saved new data\r");
        }
        //vPrintString( "Bit setting task -\t about to set vH_trestbps_pTASK_BIT.\r\n" );
        xEventGroupSetBits( xHeart_EventGroup, vH_5_14_pTASK_BIT );
    }
}

void vPrintString( const char *pcString )
{
	/* Write the string to stdout, suspending the scheduler as a method of mutual
	exclusion. */
	vTaskSuspendAll();//x safeRTOS � vTaskSuspendScheduler();
	{
		printf( "%s", pcString );
		fflush( stdout );
	}
	xTaskResumeAll();//x safeRTOS � xTaskResumeScheduler();
}

void vPrintStringAndNumber( const char *pcString,  uint32_t *pcNum  )
{
	/* Write the string to stdout, suspending the scheduler as a method of mutual
	exclusion. */
	vTaskSuspendAll();//x safeRTOS � vTaskSuspendScheduler();
	{
		printf( "%s", pcString );
		printf( "%d\r\n", (int)pcNum );
		fflush( stdout );
	}
	xTaskResumeAll();//x safeRTOS � xTaskResumeScheduler();
}
