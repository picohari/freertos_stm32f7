/*
 *
 * Copyright (c) by Alexander Neumann <alexander@bumpern.de>
 * Copyright (c) by Stefan Siegl <stesie@brokenpipe.de>
 * Copyright (c) by David Gräff <david.graeff@web.de>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (either version 2 or
 * version 3) as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

//#include <avr/io.h>
//#include <avr/interrupt.h>

#include "config.h"

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"

#include "core/periodic.h"
//#include "core/debug.h"

#ifdef FREQCOUNT_SUPPORT
//#include "services/freqcount/freqcount.h"
#endif

#ifdef BOOTLOADER_SUPPORT
uint16_t bootload_delay = CONF_BOOTLOAD_DELAY;
#endif

/* TIM handle declaration */
TIM_HandleTypeDef    TimHandle;

//extern volatile uint8_t newtick;
uint8_t milliticks;

/* Prescaler declaration */
uint32_t uwPrescalerValue = 0;

void
periodic_Init(void)
{
  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
	In this example TIM3 input clock (TIM3CLK)  is set to APB1 clock (PCLK1) x2,
	since APB1 prescaler is equal to 4.
	  TIM3CLK = PCLK1*2
	  PCLK1 = HCLK/4
	  => TIM3CLK = HCLK/2 = SystemCoreClock/2
	To get TIM3 counter clock at 10 KHz, the Prescaler is computed as follows:
	Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	Prescaler = ((SystemCoreClock/2) /10 KHz) - 1

	Note:
	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f7xx.c file.
	 Each time the core clock (HCLK) changes, user had to update SystemCoreClock
	 variable value. Otherwise, any configuration based on this variable will be incorrect.
	 This variable is updated in three ways:
	  1) by calling CMSIS function SystemCoreClockUpdate()
	  2) by calling HAL API function HAL_RCC_GetSysClockFreq()
	  3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */

  //CLOCK_SET_PRESCALER;
#ifdef CLOCK_CPU_SUPPORT
  /* init timer3 to expire after ~20ms, with Normal */

  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
  //uwPrescalerValue = (uint32_t)((SystemCoreClock / 2) / 100000);
  uwPrescalerValue = 2700;

  /* Set TIMx instance */
  TimHandle.Instance = TIM3;

  /* Initialize TIMx peripheral as follows:
	   + Period = 10000 - 1
	   + Prescaler = ((SystemCoreClock / 2)/10000) - 1
	   + ClockDivision = 0
	   + Counter direction = Up
  */
  TimHandle.Init.Period            = 400 - 1;
  TimHandle.Init.Prescaler         = uwPrescalerValue - 1;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;


  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
	/* Initialization Error */
	//Error_Handler();
  }

  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
  /* Start Channel1 */
  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
  {
	/* Starting Error */
	//Error_Handler();
  }


#else
#ifdef FREQCOUNT_SUPPORT
  /* init timer3 to run with full cpu frequency, normal mode,
   * compare and overflow int active */
  TC3_PRESCALER_1;
  freqcount_init();
  TC3_INT_COMPARE_ON;
  TC3_INT_OVERFLOW_ON;
#else
  /* init timer3 to expire after ~20ms, with CTC enabled */
  //TC3_MODE_CTC;
  //TC3_COUNTER_COMPARE = (F_CPU / CLOCK_PRESCALER / HZ) - 1;
  //TC3_INT_COMPARE_ON;
  //NTPADJDEBUG("configured OCR3A to %d\n", TC3_COUNTER_COMPARE);
#endif
#endif
}




#ifdef FREQCOUNT_SUPPORT
void
timer_expired(void)
#else
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//ISR(TC3_VECTOR_COMPARE)
#endif
{


#ifdef CLOCK_CPU_SUPPORT
  //TC3_COUNTER_COMPARE += CLOCK_TICKS;
#endif
  //newtick = 1;
  if (++milliticks >= HZ)
    milliticks -= HZ;


  BSP_LED_Toggle(LED1);

#if 0
  uint8_t ramFree[32];
  uint32_t freeRam = xPortGetFreeHeapSize();
  sprintf((char*)ramFree, "%lu B", freeRam);
  BSP_LCD_DisplayStringAt(5, 204, ramFree, RIGHT_MODE);
#endif

}


/*
 -- Ethersex META --
 header(core/periodic.h)
 init(periodic_init)
 */
