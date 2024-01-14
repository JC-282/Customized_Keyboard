/*
 * init.h
 *
 *  Created on: Feb 8, 2023
 *      Author: cai282
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_

#include "stm32f1xx_hal.h"
#include "stm32f103xb.h"
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void init_usart1();
void init_tim2(void);
void enable_ports(void);
void gpio_in(GPIO_TypeDef *x, int a);
void gpio_out(GPIO_TypeDef *x, int a);
void init_eeprom(void);



void Error_Handler(void);

#endif /* INC_INIT_H_ */
