/*
 * rtc.c
 *
 *  Created on: Mar 10, 2016
 *      Author: dk6yf
 */



#include "config.h"

#include "hardware/rtc/rtc.h"
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"


/* Private variables ---------------------------------------------------------*/

/* RTC handler declaration */
RTC_HandleTypeDef   RtcHandle;

/* Buffers used for displaying Time and Date */
uint8_t aShowTime[50] = {0}, aShowTimeStamp[50] = {0};
uint8_t aShowDate[50] = {0}, aShowDateStamp[50] = {0};




/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure the current time and date and activate timestamp.
  * @param  None
  * @retval None
  */
void RTC_TimeStampConfig(int weekday, int date, int mon, int year, int hour, int min, int sec)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Time Stamp peripheral ################################*/
  /*  RTC TimeStamp generation: TimeStamp Rising Edge on PC.13 Pin */
  HAL_RTCEx_SetTimeStamp_IT(&RtcHandle, RTC_TIMESTAMPEDGE_RISING, RTC_TIMESTAMPPIN_PC13);

  /*##-2- Configure the Date #################################################*/
  /* Set Date: Monday April 14th 2015 */
  sdatestructure.Year    = (year != 0 ? year-100 : 0);
  sdatestructure.Month   = mon+1;
  sdatestructure.Date    = (date != 0 ? date : 1);
  //sdatestructure.WeekDay = weekday;

  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure, RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  /*##-3- Configure the Time #################################################*/
  /* Set Time: 08:10:00 */
  stimestructure.Hours          = (hour != 0 ? hour+1 : 0);
  stimestructure.Minutes        = min;
  stimestructure.Seconds        = (sec != 0 ? sec : 0);
  stimestructure.SubSeconds     = 0x00;
  //stimestructure.TimeFormat     = RTC_HOURFORMAT12_AM;
  //stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if(HAL_RTC_SetTime(&RtcHandle,&stimestructure, RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }
}

/**
  * @brief  Timestamp callback
  * @param  hrtc : hrtc handle
  * @retval None
  */

#if 0
void HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc)
{
  RTC_DateTypeDef sTimeStampDateget;
  RTC_TimeTypeDef sTimeStampget;

  /* Toggle LED1 on */
  //BSP_LED_Toggle(LED1);

  HAL_RTCEx_GetTimeStamp(&RtcHandle, &sTimeStampget, &sTimeStampDateget, RTC_FORMAT_BIN);

  /* Display time Format : hh:mm:ss */
  sprintf((char*)aShowTimeStamp,"%.2d:%.2d:%.2d", sTimeStampget.Hours, sTimeStampget.Minutes, sTimeStampget.Seconds);
  /* Display date Format : mm-dd */
  sprintf((char*)aShowDateStamp,"%.2d-%.2d-%.2d", sTimeStampDateget.Date, sTimeStampDateget.Month, 2015);
}
#endif





void RTC_Config(void)
{
	/*##-1- Configure the RTC peripheral #######################################*/
	 /* Configure RTC prescaler and RTC data registers */
	 /* RTC configured as follow:
		 - Hour Format    = Format 12
		 - Asynch Prediv  = Value according to source clock
		 - Synch Prediv   = Value according to source clock
		 - OutPut         = Output Disable
		 - OutPutPolarity = High Polarity
		 - OutPutType     = Open Drain */



	 __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
	 RtcHandle.Instance = RTC;
	 RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
	 RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
	 RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
	 RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
	 RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	 RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

	 if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
	 {
	   /* Initialization Error */
	   //Error_Handler();
	 }

	 /*##-2-  Configure RTC Timestamp ############################################*/
	 RTC_TimeStampConfig(0, 0, 0, 0, 0, 0, 0);

}
