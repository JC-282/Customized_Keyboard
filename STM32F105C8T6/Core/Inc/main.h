#ifndef __MAIN_H
#define __MAIN_H
#include "stm32f1xx_hal.h"
#include "keyscan.h"
#include "init.h"

typedef struct
{
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
}subKeyBoard;

extern uint8_t keyarr[5][15];
void usb_printKey(uint16_t output);
void usb_printSeq(uint16_t *sequence);
void usb_printChar(uint16_t c);
void UI_modify();
void reset_sequence();
void setup_sequence();
void swap_key();
void display_key_sequence();
void print_message(char *msg);
void display_layout();
void saveSeq(int entry);
void loadSeq();
//void saveLayout();
void loadLayout();
void uart_printChar(uint32_t c);
void uart_printSeq(uint16_t* sequence);
void display_sequence(uint32_t c);

#define HAL_DELAY 2
#define SEQUENCE_LENGTH 16

#define KEY_LAYOUT_PAGE0 0x0000
#define KEY_LAYOUT_PAGE1 0x0020
#define KEY_LAYOUT_LAST 0x0040
#define KEY_SEQUENCE_PAGE_START 0x0060
#endif
