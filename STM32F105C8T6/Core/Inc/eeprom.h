/*
 * eeprom.h
 *
 *  Created on: Mar 8, 2023
 *      Author: cai282
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

#define EEPROM_CLK	6
#define EEPROM_DATA	5
#define EEPROM_DELAY 0.001
#define EEPROM_ACK_DELAY 1ready

#include "stm32f1xx_hal.h"
#include "stm32f103xb.h"

void eeprom_start();
void eeprom_stop();
void eeprom_sendbit(char bit);
uint8_t eeprom_waitACK();
void eeprom_sendbyte(uint8_t byte);
void eeprom_pageWrite(uint8_t * data, uint16_t address, int count);
void eeprom_sendcontrol(uint8_t rw);
void eeprom_byteWrite(uint8_t data, uint16_t address);
void eeprom_sendAddress(uint16_t address);
uint8_t eeprom_randomRead(uint16_t address);
void eeprom_sequentialRead(uint8_t* data, uint16_t address, int count);
void send_ACK();
void send_NACK();
uint8_t readbit();
uint8_t readbyte();


#endif /* INC_EEPROM_H_ */
