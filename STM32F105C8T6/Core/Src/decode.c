/*
 * decode.c
 *
 *  Created on: Feb 26, 2023
 *      Author: cai282
 */

#include "decode.h"
#include "eeprom.h"
uint8_t keyarr[5][15] = {
		{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e},
		{0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e},
		{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e},
		{0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e},
		{0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e}
};


const uint8_t resetKey[5][15] = {
		{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e},
		{0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e},
		{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e},
		{0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e},
		{0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e}
};
const uint8_t lower_uart[5][15] = {
		{0x1b,0x60,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x2d,0x3d,0x08},
		{0x09,0x71,0x77,0x65,0x72,0x74,0x79,0x75,0x69,0x6f,0x70,0x5a,0x5d,0x5c,0x00},
		{0x00,0x61,0x73,0x64,0x66,0x67,0x68,0x6a,0x6b,0x6c,0x3b,0x27,0x0d,F2,0x00},
		{0x00,0x7a,0x78,0x63,0x76,0x62,0x6e,0x6d,0x2c,0x2e,0x2f,0x00,UP_UART,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,LEFT_UART,DOWN_UART,RIGHT_UART}
};
const uint8_t upper_uart[5][15] = {
		{0x1b,0x7e,0x21,0x40,0x23,0x24,0x25,0x5e,0x26,0x2a,0x28,0x29,0x5f,0x2b,0x08},
		{0x09,0x51,0x57,0x45,0x52,0x54,0x59,0x55,0x49,0x4f,0x50,0x7b,0x7d,0x7c,0x00},
		{0x00,0x41,0x53,0x44,0x46,0x47,0x48,0x4a,0x4b,0x4c,0x3a,0x22,0x0d,0x00,0x00},
		{0x00,0x5a,0x58,0x43,0x56,0x42,0x4e,0x4d,0x3c,0x3e,0x3f,0x00,UP_UART,0x00,0x00},
		{0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,LEFT_UART,DOWN_UART,RIGHT_UART}
};
const uint8_t usb_map[5][15] = {
		{41,  53, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,  45, 46,42},
		{43,  20, 26, 8,  21, 23, 28, 24, 12, 18, 19, 47,  48, 49,0xff},
		{57,  4,  22, 7,  9,  10, 11, 13, 14, 15, 51, 52,  40, 0, 0},
		{225, 29, 27, 6,  25, 5,  17, 16, 54, 55, 56, 229, 82, 0, 0},
		{224, 227,226,0,  0,  44, 0,  0,  0,  0,  230,228, 80, 81,79}
};



uint8_t decode_usb(uint8_t input)
{
	uint8_t result;
	int row, col, map_r, map_c;

	uint8_t location;
	row = (input >> 4) & 0x0f;
	col = input & 0x0f;

	//key location
	location = keyarr[row][col];
	//mapping
	map_r = (location >> 4) & 0x0f;
	map_c = location & 0x0f;
	if(input == 0xff)
	{
		result = 0;
	}
	else{
		result = usb_map[map_r][map_c];
	}

	return result;
}

char decode_uart(uint16_t input)
{
	int row, col, map_r, map_c;
	char result;
	uint8_t location;
	row = (input >> 4) & 0x0f;
	col = input & 0x0f;

	//key location
	location = keyarr[row][col];
	//mapping
	map_r = (location >> 4) & 0x0f;
	map_c = location & 0x0f;
	if((input & 0xff) == 0xff)
	{
		result = 0;
	}
	else
	{
		if((input>>13) & 0x01)
		{
			result = upper_uart[map_r][map_c];
		}
		else
		{
			result = lower_uart[map_r][map_c];
		}
	}
		return result;

}

void save_keymap(uint8_t a, uint8_t b)
{

}

uint16_t lookup_char(char input)
{
	for(int i = 0; i < 15; i++)
	{
		for(int j = 0; j < 5; j++)
		{
			if(input == lower_uart[j][i])
				return (0<<8 | usb_map[j][i]);
			else if(input == upper_uart[j][i])
				return (1<<8 | usb_map[j][i]);
		}
	}
	return 0;
}
void reset_layout()
{
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 15; j++)
		{
			keyarr[i][j] = resetKey[i][j];
		}
	}
	char message[] = "\rStart resetting layout......";
	print_message(message);
	saveLayout();
	char message2[] = "\rDone";
	print_message(message2);

	return;
}



void swap(uint8_t a, uint8_t b)
{
	uint8_t temp = keyarr[(a&0xf0)>>4][a&0xf];
	keyarr[(a&0xf0)>>4][a&0xf] = keyarr[(b&0xf0)>>4][b&0xf];
	keyarr[(b&0xf0)>>4][b&0xf] = temp;
	return;
}
uint8_t (*get_keyMap())[15]
{
	return keyarr;
}

void saveLayout()
{
	uint8_t (*keyarr)[15] = get_keyMap();
	uint8_t arr[32] = {};
	int count = 0;
	for(int i = 0; i<5; i++)
	{
		for(int j = 0; j<15; j++)
		{

			arr[count] = keyarr[i][j];
			if(count == 31)
				{

				eeprom_pageWrite(arr, i*15+j - 31, 32);

				count = 0;
				continue;
			}
			count++;
		}
	}
	eeprom_pageWrite(arr, KEY_LAYOUT_LAST, 11);


	return;
}
