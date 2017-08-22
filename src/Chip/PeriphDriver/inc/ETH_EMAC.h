/*
 ============================================================================
 Name        : ETH_EMAC.h
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#ifndef _ETH_EMAC_H
#define _ETH_EMAC_H

#include "stm32f4xx.h"

/* Specific defines for EXTI line, used to manage Ethernet link status */
#define ETH_LINK_EXTI_LINE             EXTI_Line0
#define ETH_LINK_EXTI_PORT_SOURCE      EXTI_PortSourceGPIOC
#define ETH_LINK_EXTI_PIN_SOURCE       EXTI_PinSource0
#define ETH_LINK_EXTI_IRQn             EXTI0_IRQn  
/* PB14 */
#define ETH_LINK_PIN                   GPIO_Pin_0
#define ETH_LINK_GPIO_PORT             GPIOC
#define ETH_LINK_GPIO_CLK              RCC_AHB1Periph_GPIOC

void vETH_EmacInit(void); 

void vETH_SetMac(u8 *mac);

#endif
