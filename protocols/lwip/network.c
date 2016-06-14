/**
 ******************************************************************************
  * File Name          : LWIP.c
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
  ******************************************************************************
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
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "lwip/network.h"
#include "config.h"
#include "libs/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"

#include "lcd_log.h"

#include "stdbool.h"

#include "services/httpd/httpserver-netconn.h"
#include "services/ntp/sntp.h"


/* ETH Variables initialization ----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/




/* Variables Initialization */
struct netif ethif;

//static uint8_t macAddress[6] = {MAC_ADDR0, MAC_ADDR1, MAC_ADDR2, MAC_ADDR3, MAC_ADDR4, MAC_ADDR5};

struct ip_addr ipaddr = {0};
struct ip_addr netmask = {0};
struct ip_addr gw = {0};
struct ip_addr dns = {0};


/* Private functions ---------------------------------------------------------*/
#if LWIP_DHCP
osSemaphoreId DHCPSEM;                         // Semaphore ID
osSemaphoreDef(DHCPSEM);                       // Semaphore definition

//static void lwipStatusCallback(struct netif *netif);
static bool networkDhcpStart(int timeout);
static bool networkDhcpStop(int timeout);
#endif // LWIP_DHCP





#if 0
void network_check_status_cb(struct netif *netif)
{
  
  //if (netif == &ethif)
  //  osSemaphoreRelease(DHCPSEM);


  if (netif_is_link_up(&ethif))
  {
      /* When the netif is fully configured this function must be called */
      netif_set_up(&ethif);
      LCD_UsrLog ((char *)"  eth0: up ...\n");
  }
  else
  {
      /* When the netif link is down this function must be called */
      netif_set_down(&ethif);
      LCD_UsrLog ((char *)"  eth0: down ...\n");
  } 
}
#endif


/* init function */
void NETWORK_Init(void)
{ 

  DHCPSEM = osSemaphoreCreate(osSemaphore(DHCPSEM) , 1 );

  /* TCP/IP protocol settings */
  tcpip_init( NULL, NULL );	
  LCD_UsrLog ((char *)"  Setup: TCP/IP STACK ok...\n");
 
  /* Add a Network interface */
  netif_add(&ethif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  ethif.hostname = "stm32f7";

  /* Register the default network interface */
  netif_set_default(&ethif);

  if (netif_is_link_up(&ethif))
  {
      /* When the netif is fully configured this function must be called */
      netif_set_up(&ethif);
      //LCD_UsrLog ((char *)"  eth0: up ...\n");
  }
  else
  {
      /* When the netif link is down this function must be called */
      netif_set_down(&ethif);
      //LCD_UsrLog ((char *)"  eth0: down ...\n");
  }
  
  
  #if LWIP_DHCP
    //ethif.status_callback = lwipStatusCallback;

    netif_set_status_callback(&ethif, network_callback);

    networkDhcpStart(5000);
    //dhcp_start(&ethif);



#if 0
    }
    else
    {
      /* Stop DHCP client */
      dhcp_stop(&ethif);

	  /* Take static IP address settings */
	  IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
	  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	  IP4_ADDR(&dns, DNS_ADDR0, DNS_ADDR1, DNS_ADDR2, DNS_ADDR3);

      netif_set_addr(&ethif, &ipaddr , &netmask, &gw);

      sprintf((char*)iptxt, "%d.%d.%d.%d", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
      LCD_UsrLog ("Static IP address : %s\n", iptxt);      

    }
#endif

  #endif


  /* Initialize DNS client */
  //resolv_init(&dns);
  dns_init();
  LCD_UsrLog ((char *)  "  Setup: DNS ok ...\n");

  sntp_init();
  sntp_request(NULL);
  LCD_UsrLog ((char *)  "  Setup: NTP ok ...\n");

  /* Initialize webserver demo */
  http_server_netconn_init();
  LCD_UsrLog ((char *)  "  Setup: HTTPD SERVER ok ...\n");

  sntp_request(NULL);
}












#if LWIP_DHCP

void network_callback(struct netif *netif)
{
	if (netif == &ethif)
		osSemaphoreRelease(DHCPSEM);
}


void networkSetDhcp(bool enabled, int timeout)
{
  if (enabled) {
    networkDhcpStart(timeout);
    //eepromWrite(EEPROM_DHCP_ENABLED, enabled);
  }
  else {
    networkDhcpStop(timeout);
    //eepromWrite(EEPROM_DHCP_ENABLED, enabled);
    //int a, m, g;
    //networkLastValidAddress(&a, &m, &g);
    //networkSetAddress(a, m, g);
  }
}


bool networkDhcpStart(int timeout)
{
  netif_set_down(&ethif); // note - dhcp_start brings it back up
  //if (netifapi_dhcp_start(ethif) != ERR_OK)
  err_t rv = dhcp_start(&ethif);
  LCD_UsrLog ((char *)  "  Setup: DHCP ok ...\n");
  if ( rv != ERR_OK){
    LCD_UsrLog ((char *)"  State: DHCP ERR: %d !!\n", rv);
    return false;
  }

  return (osSemaphoreWait(DHCPSEM, timeout) == ERR_OK);


}


bool networkDhcpStop(int timeout)
{
  dhcp_stop(&ethif);
  //bool rv = (osSemaphoreWait(DHCPSEM, timeout) == ERR_OK);
  netif_set_up(&ethif); // bring the interface back up, as dhcp_release() takes it down
  return 0;
}


#endif /* LWIP_DHCP */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
