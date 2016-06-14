/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 *
 * COPYRIGHT(c) 2015 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"

#include "lcd_log.h"
#include "core/periodic.h"
#include "hardware/rtc/rtc.h"
#include "hardware/adc/adc.h"

#include "services/httpd/httpserver-netconn.h"
#include "fs.h"


#include "lwip/dns.h"
#include "lwip/network.h"


//#include "protocols/dns/resolv.h"
#include "ip_addr.h"

static osThreadId mainTaskHandle;



static void CpuCacheEnable( void );
static void SystemClock_Config( void );
static void LCD_Init( void );
static void StartMainTask( void const * args );
static void GPIO_Init( void );
static void RTC_CalendarShow( void );

static void CLOCK_thread(void const * args);




int main( void )
{
	CpuCacheEnable();

	HAL_Init();

	SystemClock_Config();

	GPIO_Init();



	osThreadDef( startup_task, StartMainTask, osPriorityNormal, 0, configMINIMAL_STACK_SIZE );
	mainTaskHandle = osThreadCreate(osThread(startup_task), NULL );

	osThreadDef( clockd, CLOCK_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE );
    osThreadCreate( osThread(clockd), NULL );

	osThreadDef( sensor_task, ADC_Init, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE );
	osThreadCreate(osThread(sensor_task), NULL );

	osKernelStart();

	while( 1 )
	{

	}
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config( void )
{
#if 1
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;  
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Activate the OverDrive to reach the 216 MHz Frequency */  
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2; 
  
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }  

#else
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE1 );

	RCC_OscInitTypeDef oscCfg;
	oscCfg.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscCfg.HSEState = RCC_HSE_ON;
	oscCfg.PLL.PLLState = RCC_PLL_ON;
	oscCfg.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	oscCfg.PLL.PLLM = 25;
	oscCfg.PLL.PLLN = 400;
	oscCfg.PLL.PLLP = RCC_PLLP_DIV2;
	oscCfg.PLL.PLLQ = 8;
	HAL_RCC_OscConfig( &oscCfg );

	HAL_PWREx_EnableOverDrive();

	RCC_ClkInitTypeDef clkCfg;
	clkCfg.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clkCfg.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkCfg.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkCfg.APB1CLKDivider = RCC_HCLK_DIV4;
	clkCfg.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig( &clkCfg, FLASH_LATENCY_6 );

	HAL_SYSTICK_Config( HAL_RCC_GetHCLKFreq() / 1000 );

	HAL_SYSTICK_CLKSourceConfig( SYSTICK_CLKSOURCE_HCLK );

	HAL_NVIC_SetPriority( SysTick_IRQn, 0, 0 );
#endif
}

/**
 * @brief  CPU L1-Cache enable.
 *         Invalidate Data cache before enabling
 *         Enable Data & Instruction Cache
 */
void CpuCacheEnable( void )
{
	(*(uint32_t *) 0xE000ED94) &= ~0x5;
	(*(uint32_t *) 0xE000ED98) = 0x0; //MPU->RNR
	(*(uint32_t *) 0xE000ED9C) = 0x20010000 | 1 << 4; //MPU->RBAR
	(*(uint32_t *) 0xE000EDA0) = 0 << 28 | 3 << 24 | 0 << 19 | 0 << 18 | 1 << 17 | 0 << 16 | 0 << 8 | 30 << 1 | 1 << 0; //MPU->RASE  WT
	(*(uint32_t *) 0xE000ED94) = 0x5;

	SCB_InvalidateICache();

	/* Enable branch prediction */
	SCB->CCR |= (1 << 18);
	__DSB();

	SCB_EnableICache();

	SCB_InvalidateDCache();
	SCB_EnableDCache();
}




void GPIO_Init( void )
{
	/* Activate clock on external peripheric devices like ETH */
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	__HAL_RCC_ETH_CLK_ENABLE();
	__HAL_RCC_CRC_CLK_ENABLE();

	BSP_SDRAM_Init();

	/* Configure LED1 & LED3 */
	BSP_LED_Init(LED1);

	/* Configure RealTimeClock*/
	RTC_Config();

	/* Configure LCD Panel */
	LCD_Init();

	/* Read some Hardware Registers for information and fun ;) */
    uint32_t flashUID = STM32_UUID[0];
    //uint32_t idPart2 = STM32_UUID[1];
    //uint32_t idPart3 = STM32_UUID[2];
    uint8_t  uuid_tmp[32];
    sprintf((char*)uuid_tmp, "Device UID: %08lx", flashUID);

    uint32_t flashSize = STM32_UUID_FLASH[0];
    uint8_t flash_tmp[32];
    sprintf((char*)flash_tmp, "%lx", flashSize);
    uint8_t flash[6];
    sprintf ((char*)flash, &(flash_tmp[strlen(flash_tmp) - 4]));
    uint16_t size = strtol((char*)flash, NULL, 16);
    sprintf((char*)flash_tmp, "Flash Size:  %d kB", size);

    //uint32_t flashPack = STM32_UUID_PACK[0];
    //uint8_t pack_tmp[10];
    //sprintf((char*)pack_tmp, "%lx", flashPack);





    //BSP_LCD_DisplayStringAt(5, 204, pack_tmp, RIGHT_MODE);
    //BSP_LCD_DisplayStringAt(70, 204, (uint8_t *)"Free RAM:", RIGHT_MODE);
    BSP_LCD_DisplayStringAt(5, 220, uuid_tmp, RIGHT_MODE);
    BSP_LCD_DisplayStringAt(5, 236, flash_tmp, RIGHT_MODE);
}


/**
  * @brief  Initializes the STM32756G-EVAL's LCD and LEDs resources.
  * @param  None
  * @retval None
  */
void LCD_Init(void)
{
  /* Initialize the LCD */
  BSP_LCD_Init();

  /* Initialize the LCD Layers */
  BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);

  /* Set LCD Foreground Layer  */
  BSP_LCD_SelectLayer(1);

  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

  /* Initialize LCD Log module */
  LCD_LOG_Init();

  /* Show Header and Footer texts */
  LCD_LOG_SetHeader((uint8_t *)"STM32F7 Application API");
  //LCD_LOG_SetFooter((uint8_t *)"STM32746G-DISCO board");

  /* Set the LCD BOTTOM FRAME Color */
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, BSP_LCD_GetYSize() - Font12.Height - 4, BSP_LCD_GetXSize(), Font12.Height + 4);

  LCD_UsrLog ((char *)"  Setup: GPIO Initialization ...\n");
}





void StartMainTask( void const * args )
{

	NETWORK_Init();

	periodic_Init();

	for( ;; )
	{
		osThreadTerminate(mainTaskHandle);
	}
}





















void CLOCK_thread (void const * args)
{

	while(1) {

		taskENTER_CRITICAL();
		/* Set the LCD TEXT Color */
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		BSP_LCD_SetBackColor(LCD_COLOR_BLUE);

		/* Show IP Address */
		uint8_t iptab[4];
		uint8_t iptxt[20];
		iptab[0] = (uint8_t)(ethif.ip_addr.addr >> 24);
		iptab[1] = (uint8_t)(ethif.ip_addr.addr >> 16);
		iptab[2] = (uint8_t)(ethif.ip_addr.addr >> 8);
		iptab[3] = (uint8_t)(ethif.ip_addr.addr);
		sprintf((char*)iptxt, "%d.%d.%d.%d |", iptab[3], iptab[2], iptab[1], iptab[0]);
		BSP_LCD_DisplayStringAt(138, BSP_LCD_GetYSize() - Font12.Height-1, iptxt, RIGHT_MODE);

		/* Write Date and Time on LCD */
		RTC_CalendarShow();
	    BSP_LCD_DisplayStringAt(345, BSP_LCD_GetYSize() - Font12.Height-1, aShowDate, LEFT_MODE);
		BSP_LCD_DisplayStringAt(422, BSP_LCD_GetYSize() - Font12.Height-1, aShowTime, LEFT_MODE);

		/* Restore original colors */
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
		BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
		taskEXIT_CRITICAL();

	}

}




void RTC_CalendarShow(void)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);

  /* Display time Format : hh:mm:ss */
  sprintf((char*)aShowTime,"%.2d:%.2d:%.2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf((char*)aShowDate,"%.2d.%.2d.%.2d", sdatestructureget.Date, sdatestructureget.Month, 2000 + sdatestructureget.Year);
}







#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
