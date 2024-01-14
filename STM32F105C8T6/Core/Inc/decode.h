/*
 * decode.h
 *
 *  Created on: Feb 26, 2023
 *      Author: cai282
 */

#ifndef INC_DECODE_H_
#define INC_DECODE_H_
#include "stm32f103xb.h"
uint8_t decode_usb(uint8_t input);
char decode_uart(uint16_t input);
uint16_t lookup_char(char input);
void reset_layout();
void swap(uint8_t a, uint8_t b);
uint8_t (*get_keyMap())[15];
void saveLayout();
//define for UART
#define UP_UART 0x81
#define DOWN_UART 0x82
#define LEFT_UART 0x83
#define RIGHT_UART 0x84
#define F2 0x85
#define KEY_LAYOUT_PAGE0 0x0000
#define KEY_LAYOUT_PAGE1 0x0020
#define KEY_LAYOUT_LAST 0x0040

#endif /* INC_DECODE_H_ */
