#include <msp430.h> 

#include "PCD8544.h"
#include <string.h>

#define UART_RX BIT1
#define UART_TX BIT2
#define MAX_BUFF_RX 100

char testBlock[8] = {0x00, 0x7F, 0x7F, 0x33, 0x33, 0x03, 0x03, 0x03};

/************* Down arrow *******************************************
00000000
‭00011000‬
‭00011000‬
‭00011000‬
‭01111110‬
‭00111100‬
‭00011000‬
00000000
********************************************************************/
char DownArrow[8] = {0x00, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00};

char rxBuffer[MAX_BUFF_RX] = {0};
int posBuffer = 0;
int posFind = 0;


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

void ConfigUart(){
	P1SEL |= UART_RX + UART_TX;					//Set P1.1 and P1.2 to RX to TX
	P1SEL2 |= UART_RX + UART_TX;
	UCA0CTL1 |= UCSSEL_2;						//SMCLK
	UCA0BR0 = 104;								//9600
	UCA0BR1 = 0;								//9600
	UCA0MCTL = UCBRS_1;							//Modulation
	UCA0CTL1 &= ~UCSWRST;						//Start USCI
	IE2 |= UCA0RXIE;
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
	ConfigUart();

	_delay_cycles(500000);

 	Nokia5110_InitLCD();
	Nokia5110_ClearLCD();

	Nokia5110_WriteStringToLCD("Nokia 5110");

	//Enter LPM0 with interrupts
	__bis_SR_register(CPUOFF + GIE);
}

void uart_send_byte( unsigned char data )
{
	while (!(IFG2&UCA0TXIFG));
		UCA0TXBUF = data;
}

void uart_send_string(const char *string) {
	while(*string) {
		uart_send_byte(*string++);
	}
}

void clear_rxBuffer(void){
	int i = 0;
	for(i = 0; i < MAX_BUFF_RX; i++)
		rxBuffer[i] = 0x0;
}

void writeAndResponseOK(int line){
	int i = 0;
	Nokia5110_ClearBank(line);

	for(i = 6; i < 20; i ++){
		if(rxBuffer[i] == '\0'){
			break;
		}
		Nokia5110_WriteCharToLCD(rxBuffer[i]);
	}
	uart_send_string("OK\r\n");
}

void decode_message(){


	posFind = strncmp((const char*)rxBuffer, "AT+L1=", 6);
	if(posFind == 0)
	{
		writeAndResponseOK(0);
	}

	posFind = strncmp((const char*)rxBuffer, "AT+L2=", 6);
	if(posFind == 0)
	{
		writeAndResponseOK(1);
	}

	posFind = strncmp((const char*)rxBuffer, "AT+L3=", 6);
	if(posFind == 0)
	{
		writeAndResponseOK(2);
	}

	posFind = strncmp((const char*)rxBuffer, "AT+L4=", 6);
	if(posFind == 0)
	{
		writeAndResponseOK(3);
	}

	posFind = strncmp((const char*)rxBuffer, "AT+L5=", 6);
	if(posFind == 0)
	{
		writeAndResponseOK(4);
	}

	posFind = strncmp((const char*)rxBuffer, "AT+L6=", 6);
	if(posFind == 0)
	{
		writeAndResponseOK(5);
	}

	clear_rxBuffer();

}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	char data = UCA0RXBUF;

	if(data == '\n'){

		posBuffer = 0;
		decode_message();

		uart_send_string("OK\r\n");

	}else{
		rxBuffer[posBuffer++] = data;
		if(posBuffer > MAX_BUFF_RX ){
			posBuffer = 0;
			clear_rxBuffer();
			uart_send_string("ERR:-1\r\n");
		}
	}
}

