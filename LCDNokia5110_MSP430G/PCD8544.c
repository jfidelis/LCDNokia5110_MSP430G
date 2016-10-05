/*
 * PCD8544.c
 *
 *  Created on: 05/10/2016
 *      Author: Jeferson IP Fidelis
 */
#include "PCD8544.h"

void Nokia5110_WriteStringToLCD(const char *string) {
	while(*string) {
		Nokia5110_WriteCharToLCD(*string++);
	}
}

void Nokia5110_WriteCharToLCD(char c) {
	unsigned char i;
	for(i = 0; i < 5; i++) {
		Nokia5110_WriteToLCD(LCD5110_DATA, font[c - 0x20][i]);
	}
	Nokia5110_WriteToLCD(LCD5110_DATA, 0);
}

void Nokia5110_WriteBlockToLCD(char *byte, unsigned char length) {
	unsigned char c = 0;
	while(c < length) {
		Nokia5110_WriteToLCD(LCD5110_DATA, *byte++);
		c++;
	}
}

void Nokia5110_WriteGraphicToLCD(char *byte, unsigned char transform) {
	int c = 0;
	char block[8];

	if(transform & FLIP_V) {
		SPI_LSB_FIRST;
	}
	if(transform & ROTATE) {
		c = 1;
		while(c != 0) {
			(*byte & 0x01) ? (block[7] |= c) : (block[7] &= ~c);
			(*byte & 0x02) ? (block[6] |= c) : (block[6] &= ~c);
			(*byte & 0x04) ? (block[5] |= c) : (block[5] &= ~c);
			(*byte & 0x08) ? (block[4] |= c) : (block[4] &= ~c);
			(*byte & 0x10) ? (block[3] |= c) : (block[3] &= ~c);
			(*byte & 0x20) ? (block[2] |= c) : (block[2] &= ~c);
			(*byte & 0x40) ? (block[1] |= c) : (block[1] &= ~c);
			(*byte & 0x80) ? (block[0] |= c) : (block[0] &= ~c);
			*byte++;
			c <<= 1;
		}
	} else {
		while(c < 8) {
			block[c++] = *byte++;
		}
	}

	if(transform & FLIP_H) {
		c = 7;
		while(c > -1) {
			Nokia5110_WriteToLCD(LCD5110_DATA, block[c--]);
		}
	} else {
		c = 0;
		while(c < 8) {
			Nokia5110_WriteToLCD(LCD5110_DATA, block[c++]);
		}
	}
	SPI_MSB_FIRST;
}

void Nokia5110_WriteToLCD(unsigned char dataCommand, unsigned char data) {
	LCD5110_SELECT;
	if(dataCommand) {
		LCD5110_SET_DATA;
	} else {
		LCD5110_SET_COMMAND;
	}
	UCB0TXBUF = data;
	while(!(IFG2 & UCB0TXIFG));
	LCD5110_DESELECT;
}

void Nokia5110_ClearLCD() {
	Nokia5110_SetAddr(0, 0);
	int c = 0;
	while(c < PCD8544_MAXBYTES) {
		Nokia5110_WriteToLCD(LCD5110_DATA, 0);
		c++;
	}
	Nokia5110_SetAddr(0, 0);
}

void Nokia5110_ClearBank(unsigned char bank) {
	Nokia5110_SetAddr(0, bank);
	int c = 0;
	while(c < PCD8544_HPIXELS) {
		Nokia5110_WriteToLCD(LCD5110_DATA, 0);
		c++;
	}
	Nokia5110_SetAddr(0, bank);
}

void Nokia5110_SetAddr(unsigned char xAddr, unsigned char yAddr) {
	Nokia5110_WriteToLCD(LCD5110_COMMAND, PCD8544_SETXADDR | xAddr);
	Nokia5110_WriteToLCD(LCD5110_COMMAND, PCD8544_SETYADDR | yAddr);
}

void Nokia5110_InitLCD() {
	Nokia5110_WriteToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
	Nokia5110_WriteToLCD(LCD5110_COMMAND, PCD8544_SETVOP | 0x3F);
	Nokia5110_WriteToLCD(LCD5110_COMMAND, PCD8544_SETTEMP | 0x02);
	Nokia5110_WriteToLCD(LCD5110_COMMAND, PCD8544_SETBIAS | 0x03);
	Nokia5110_WriteToLCD(LCD5110_COMMAND, PCD8544_FUNCTIONSET);
	Nokia5110_WriteToLCD(LCD5110_COMMAND, PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);
}


