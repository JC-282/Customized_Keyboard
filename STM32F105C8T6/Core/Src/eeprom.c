#include "eeprom.h"

void eeprom_start()
{
	GPIOA->BSRR = 1 << (EEPROM_DATA);
	HAL_Delay(EEPROM_DELAY);
	GPIOA->BSRR = 1 << (EEPROM_CLK);

	while (!( (GPIOA->IDR >> EEPROM_CLK) & 1));
	HAL_Delay(EEPROM_DELAY);
	GPIOA->BSRR = 1 << (EEPROM_DATA + 16);
	HAL_Delay(EEPROM_DELAY);
	GPIOA->BSRR = 1 << (EEPROM_CLK + 16);
	HAL_Delay(EEPROM_DELAY);
	return;
}

void eeprom_stop()
{
	GPIOA->BSRR = 1 << (EEPROM_DATA + 16);
	GPIOA->BSRR = 1 << EEPROM_CLK;
	while (!( (GPIOA->IDR >> EEPROM_CLK) & 1));
	HAL_Delay(EEPROM_DELAY);
	GPIOA->BSRR = 1 << EEPROM_DATA;
	HAL_Delay(EEPROM_DELAY);
	return;
}

void eeprom_sendbit(char bit)
{
	if (bit == 1) GPIOA->BSRR = 1 << EEPROM_DATA;
	else GPIOA->BSRR = 1 << (EEPROM_DATA + 16);
	HAL_Delay(EEPROM_DELAY);

	GPIOA->BSRR = 1 << EEPROM_CLK;
	while (!( (GPIOA->IDR >> EEPROM_CLK) & 1));
	HAL_Delay(EEPROM_DELAY);

	GPIOA->BSRR = 1 << (EEPROM_CLK + 16);
	HAL_Delay(EEPROM_DELAY);
	return;
}

uint8_t eeprom_waitACK()
{

	GPIOA->BSRR = 1 << EEPROM_CLK;
	while (!( (GPIOA->IDR >> EEPROM_CLK) & 1));
	uint8_t result = (GPIOA->IDR >> EEPROM_DATA) & 1;
	HAL_Delay(EEPROM_DELAY);

	GPIOA->BSRR = 1 << (EEPROM_CLK + 16);
	HAL_Delay(EEPROM_DELAY);

	return result;
}

void send_ACK()
{
	GPIOA->BSRR = 1 << (EEPROM_DATA + 16);
	HAL_Delay(EEPROM_DELAY);

	GPIOA->BSRR = 1 << EEPROM_CLK;
	while (!( (GPIOA->IDR >> EEPROM_CLK) & 1));
	HAL_Delay(EEPROM_DELAY);

	GPIOA->BSRR = 1 << (EEPROM_CLK + 16);
	GPIOA->BSRR = 1 << (EEPROM_DATA);
	HAL_Delay(EEPROM_DELAY);
	return;
}

void send_NACK()
{
	GPIOA->BSRR = 1 << (EEPROM_DATA);
	HAL_Delay(EEPROM_DELAY);

	GPIOA->BSRR = 1 << EEPROM_CLK;
	while (!( (GPIOA->IDR >> EEPROM_CLK) & 1));
	HAL_Delay(EEPROM_DELAY);

	GPIOA->BSRR = 1 << (EEPROM_CLK + 16);
	GPIOA->BSRR = 1 << (EEPROM_DATA);
	HAL_Delay(EEPROM_DELAY);
	return;
}

void eeprom_sendbyte(uint8_t byte)
{

	for (int i = 7; i >=0; i--)
	{
		eeprom_sendbit((byte >> i) & 1);
	}


	eeprom_waitACK();

	return;
}

void eeprom_sendcontrol(uint8_t rw)
{
	uint8_t byte = (0xa0 | rw);
	for (int i = 7; i >=0; i--)
	{
		eeprom_sendbit((byte >> i) & 1);
	}


	eeprom_waitACK();

	return;
}

void eeprom_sendAddress(uint16_t address)
{
	eeprom_sendbyte((address >> 8) & 0xff);
	eeprom_sendbyte(address & 0xff);

	return;
}


void eeprom_byteWrite(uint8_t data, uint16_t address)
{
	eeprom_start();

	eeprom_sendcontrol(0);
	eeprom_sendAddress(address);
	eeprom_sendbyte(data);

	eeprom_stop();
	return;

}

// change to page write
void eeprom_pageWrite(uint8_t * data, uint16_t address, int count)
{
	eeprom_start();
	eeprom_sendcontrol(0);
	eeprom_sendAddress(address);
	for (int i = 0; i < count; i++)
	{
		eeprom_sendbyte(*(data + i));
	}
	eeprom_stop();
	return;
}

uint8_t readbit()
{
	uint8_t result;
	GPIOA->BSRR = 1 << EEPROM_CLK;
	while (!( (GPIOA->IDR >> EEPROM_CLK) & 1));
	result = (GPIOA->IDR >> EEPROM_DATA) & 1;
	HAL_Delay(EEPROM_DELAY);

	GPIOA->BSRR = 1 << (EEPROM_CLK + 16);
	HAL_Delay(EEPROM_DELAY);

	return result;
}

uint8_t readbyte()
{
	uint8_t result;
	for(int i = 0; i < 8; i++)
	{
		result = (result << 1) | readbit();
	}

	return result;
}

uint8_t eeprom_randomRead(uint16_t address)
{
	uint8_t result;

	eeprom_start();
	eeprom_sendcontrol(0);
	eeprom_sendAddress(address);

	eeprom_start();
	eeprom_sendcontrol(1);

	result = readbyte();
	send_NACK();
	eeprom_stop();
	return result;
}


void eeprom_sequentialRead(uint8_t* data, uint16_t address, int count)
{

	eeprom_start();
	eeprom_sendcontrol(0);
	eeprom_sendAddress(address);

	eeprom_start();
	eeprom_sendcontrol(1);

	for(int i = 0; i < count; i++)
	{
		data[i] = readbyte();
		if (i < count - 1) send_ACK();
	}

	eeprom_stop();
}


