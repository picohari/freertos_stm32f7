/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Netconn_RTOS/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    18-November-2015 
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"




 /**
  * The STM32 factory-programmed UUID memory.
  * Three values of 32 bits each starting at this address
  * Use like this: STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]
  */
#define STM32_UUID 			((uint32_t *)0x1FF0F420)
#define STM32_UUID_FLASH	((uint32_t *)0x1FF0F442)
#define STM32_UUID_PACK		((uint32_t *)0x1FFF7BF0)


 // TIME AND CLOCK
 #define CLOCK_DATETIME_SUPPORT		1
 #define CLOCK_DATE_SUPPORT			1
 #define CLOCK_TIME_SUPPORT			1
 #define CLOCK_CPU_SUPPORT			1




/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/ 
#define USE_DHCP   0    /* enable DHCP, if disabled static address is used*/
//#define USE_DNS    0
 
/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   164
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1 

 /*DNS Address*/
 #define DNS_ADDR0   192
 #define DNS_ADDR1   168
 #define DNS_ADDR2   1
 #define DNS_ADDR3   1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */  


#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
