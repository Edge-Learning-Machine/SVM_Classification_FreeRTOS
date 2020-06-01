/*
 * acqData.h
 *
 *  Created on: 29 mag 2020
 *      Author: flavi
 */

#ifndef ACQDATA_H_
#define ACQDATA_H_

#define vH_1_3_pTASK_BIT		( 1UL << 0UL ) /* Event bit 0, which is set by a task. */
#define vH_4_pTASK_BIT	( 1UL << 1UL ) /* Event bit 0, which is set by a task. */
#define vH_5_14_pTASK_BIT		( 1UL << 2UL ) /* Event bit 0, which is set by a task. */

//age,sex,cp,trestbps,chol,fbs,restecg,thalach,exang,oldpeak,slope,ca,thal,target
void vHeart_1_3_param( void *pvParameters );
void vHeart_4_param( void *pvParameters );
void vHeart_5_14_param( void *pvParameters );

void vHeart_ReadData( void *pvParameters );
void vPrintStringAndNumber( const char *pcString,  uint32_t *pcNum  );
void vPrintString( const char *pcString );


EventGroupHandle_t xHeart_EventGroup;
QueueHandle_t xQueue[14];
SemaphoreHandle_t xBinarySemaphore;

#endif /* ACQDATA_H_ */
