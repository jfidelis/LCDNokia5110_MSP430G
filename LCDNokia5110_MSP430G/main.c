#include <msp430.h> 

#include "PCD8544.h"

char testBlock[8] = {0x00, 0x7F, 0x7F, 0x33, 0x33, 0x03, 0x03, 0x03};
char testBlock2[8] = {0x00, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00};

void InitCPU(void){
	// disable WDT
	WDTCTL = WDTPW + WDTHOLD;
	// 1MHz clock
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
}

void InitPorts(void)
{
	P1OUT |= LCD5110_SCE_PIN + LCD5110_DC_PIN + LCD5110_RST_PIN;
	P1DIR |= LCD5110_SCE_PIN + LCD5110_DC_PIN + LCD5110_RST_PIN;

	// setup USIB
	P1SEL |= LCD5110_SCLK_PIN + LCD5110_DN_PIN;
	P1SEL2 |= LCD5110_SCLK_PIN + LCD5110_DN_PIN;

	LCD5110_RST_PIN_HI;
}

void ConfigSPI(){
	// 3-pin, 8-bit SPI master
	UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;
	// SMCLK
	UCB0CTL1 |= UCSSEL_2;
	// 1:1
	UCB0BR0 |= 0x01;
	UCB0BR1 = 0;
	// clear SW
	UCB0CTL1 &= ~UCSWRST;
}

void main(void) {

	InitCPU();
	InitPorts();
	ConfigSPI();

	_delay_cycles(500000);

 	Nokia5110_InitLCD();
	Nokia5110_ClearLCD();

	Nokia5110_WriteStringToLCD("Nokia 5110");

	_delay_cycles(2000000);

	Nokia5110_SetAddr(0, 0);

	int c = 0x20;
	while(c < (65 + 0x20)) {
		Nokia5110_WriteCharToLCD(c);
		c++;
	}

	_delay_cycles(2000000);
	Nokia5110_ClearLCD();

	c = 65 + 0x20;

	while(c < (96 + 0x20)) {
		Nokia5110_WriteCharToLCD(c);
		c++;
	}

	_delay_cycles(2000000);

	c = 0;
	Nokia5110_ClearBank(3);
	while(c < 64) {
		Nokia5110_WriteToLCD(LCD5110_DATA, 0xFF);
		c++;
		_delay_cycles(20000);
	}
	Nokia5110_ClearBank(4);
	while(c < 100) {
		Nokia5110_WriteToLCD(LCD5110_DATA, 0xFF);
		c++;
		_delay_cycles(20000);
	}
	Nokia5110_ClearBank(5);
	while(c < 184) {
		Nokia5110_WriteToLCD(LCD5110_DATA, 0xFF);
		c++;
		_delay_cycles(20000);
	}

	_delay_cycles(2000000);

	Nokia5110_ClearBank(3);
	Nokia5110_WriteGraphicToLCD(testBlock, NONE);
	Nokia5110_WriteGraphicToLCD(testBlock, FLIP_V);
	Nokia5110_WriteGraphicToLCD(testBlock, FLIP_H);
	Nokia5110_WriteGraphicToLCD(testBlock, ROTATE);
	Nokia5110_WriteGraphicToLCD(testBlock, FLIP_V | ROTATE);
	Nokia5110_WriteGraphicToLCD(testBlock, FLIP_H | ROTATE);
	Nokia5110_WriteGraphicToLCD(testBlock, ROTATE_90_CCW);
	Nokia5110_WriteGraphicToLCD(testBlock, ROTATE_180);

	Nokia5110_ClearBank(4);
	Nokia5110_WriteGraphicToLCD(testBlock2, NONE);
	Nokia5110_WriteGraphicToLCD(testBlock2, FLIP_H);
	Nokia5110_WriteGraphicToLCD(testBlock2, ROTATE);
	Nokia5110_WriteGraphicToLCD(testBlock2, ROTATE_90_CCW);

	Nokia5110_ClearBank(0);
	Nokia5110_WriteStringToLCD("NOKIA 5110");
	Nokia5110_ClearBank(1);
	Nokia5110_WriteStringToLCD("COM");
	Nokia5110_ClearBank(2);
	Nokia5110_WriteStringToLCD("MSP430G2553");
}


