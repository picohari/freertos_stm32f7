/*
 * rtc.h
 *
 *  Created on: Mar 10, 2016
 *      Author: dk6yf
 */

#ifndef _RTC_H_
#define _RTC_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* RTC handler declaration */
extern RTC_HandleTypeDef 	RtcHandle;

/* Buffers used for displaying Time and Date */
extern uint8_t aShowTime[50], aShowTimeStamp[50];
extern uint8_t aShowDate[50], aShowDateStamp[50];


/* Defines related to Clock configuration */
/* Uncomment to enable the adaquate Clock Source */
//#define RTC_CLOCK_SOURCE_LSI
#define RTC_CLOCK_SOURCE_LSE

#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV    	0x7F
#define RTC_SYNCH_PREDIV     	0x0130
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  		0x7F
#define RTC_SYNCH_PREDIV   		0x00FF
#endif




/* Exported functions ------------------------------------------------------- */
void RTC_Config(void);
void RTC_TimeStampConfig(int weekday, int date, int mon, int year, int hour, int min, int sec);

#endif /* _RTC_H_ */
