/*
 * periodic.h
 *
 *  Created on: Mar 10, 2016
 *      Author: dk6yf
 */

#ifndef _PERIODIC_H
#define _PERIODIC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"

//#include <avr/io.h>

/* TIM handle declaration */
//extern TIM_HandleTypeDef    TimHandle;

/* Exported macro ------------------------------------------------------------*/
/* User can use this section to tailor TIMx instance used and associated
   resources */
 /* Definition for TIMx clock resources */
 #define TIMx                           TIM3
 #define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()


 /* Definition for TIMx's NVIC */
 #define TIMx_IRQn                      TIM3_IRQn
 #define TIMx_IRQHandler                TIM3_IRQHandler





#define MAX_OVERFLOW          65535UL   /* timer1 max value */
#define HZ                    50        /* 20ms */

#if (F_CPU/256) < MAX_OVERFLOW
#define CLOCK_PRESCALER       256UL
#define CLOCK_SET_PRESCALER   TC3_PRESCALER_256
#else
#define CLOCK_PRESCALER       1024UL
#define CLOCK_SET_PRESCALER   TC3_PRESCALER_1024
#endif


// timer ticks needed for one second
#define CLOCK_SECONDS         (F_CPU/CLOCK_PRESCALER)
// timer ticks needed for one 20ms clock tick
#define CLOCK_TICKS           (F_CPU/CLOCK_PRESCALER/HZ)

extern uint8_t milliticks;

/* initialize hardware timer */
void periodic_Init(void);

void timer_expired(void);




#endif /* _PERIODIC_H */
