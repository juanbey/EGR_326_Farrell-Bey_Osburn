/*
----------------------------------------------------------------------------------------------------
Lab 8: Seven Segment 8 Character LED Display
Authors: Jonathan Osburn, KeJuan Farrell-Bey
Date: 10/28/2022
Overview: This program uses SPI for the MSP432 to talk to the 7-seg display. A push button is 
interfaced with MSP so that it can enable or disable the timer count from incrementing. Every button 
press is logged on the left 4 digits while the timer count is shown on the right 4 digits.
Version: Initial Draft
----------------------------------------------------------------------------------------------------
*/
#include "msp.h"
#include "timers.h"
#include <stdbool.h>

#define BUTTON1 BIT6 // P2.6

#define CS BIT4	  // P9.4
#define CLK BIT5  // P9.5
#define MOSI BIT7 // P9.7

#define DECODE 0x09
#define INTENSITY 0x0A
#define SCAN_LIMIT 0x0B
#define SHUTDOWN 0x0C

#define DISPLAY_TEST 0xFF
#define SHUTDOWN_MODE 0x00
#define NORMAL_MODE 0x01

#define NO_DECODE 0x00
#define NO_DECODE71 0x01
#define NO_DECODE74 0x0F
#define CODE_B 0xFF

#define ALL_DIGITS 0x07
#define SET_INTENSITY 0x02

#define HALF_INTENSITY 0x08
#define MAX_INTENSITY 0x0F

void SPI_Init(void);							 // Set up SPI for 3-Pin mode
void WriteData(uint16_t address, uint16_t data); // SPI function that will send address and data to peripheral
void Segment_Init(void);
void Button_init(void);		 // Initializes Buttons Pins 6 & 7 (Push Buttons)
uint16_t Button1Press(void); // Function to Debounce a press on button 1 (2.6)
void Reset_Segments(void);
void displayCount(uint16_t count);
void displayTime(uint16_t count);

bool stopTime = false;
uint16_t buttonCount = 0;
uint16_t timeCount = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

	SPI_Init();				  // initialize SPI configuration
	SysTick_Init_Interrupt(); // initialize SysTick for 1s interrupts
	Segment_Init();			  // set up 7-seg to display
	Button_init();			  // button for counters
	Reset_Segments();		  // set all segments to 0

	while (1)
	{
		;
	}
}

void displayCount(uint16_t count)
{
	uint16_t thousands, hundreds, tens, ones = 0;

	thousands = (count / 1000);
	hundreds = (count % 1000) / 100;
	tens = (count % 100) / 10;
	ones = (count % 10);

	WriteData(0x05, ones);
	WriteData(0x06, tens);
	WriteData(0x07, hundreds);
	WriteData(0x08, thousands);
}

void displayTime(uint16_t count)
{
	uint16_t thousands, hundreds, tens, ones = 0;

	thousands = (timeCount / 1000);
	hundreds = (timeCount % 1000) / 100;
	tens = (timeCount % 100) / 10;
	ones = (timeCount % 10);

	WriteData(0x01, ones);
	WriteData(0x02, tens);
	WriteData(0x03, hundreds);
	WriteData(0x04, thousands);
}


/*---------------------------------------------------------------------------------------------
This function sets the CS low and sends 2 bursts of data (2 bytes). The address is what type of
instruction and the data is the value sent to the address. After the 2 bytes are sent, the CS
is released.
-----------------------------------------------------------------------------------------------*/
void WriteData(uint16_t address, uint16_t data)
{
	P9->OUT &= ~CS; // set CS chip low
	while (!(EUSCI_A3->IFG & 2))
		;					   // wait for transmit buffer empty
	EUSCI_A3->TXBUF = address; // Send address

	while (EUSCI_A3->STATW & 1)
		; // wait for transmit buffer empty

	// P9->OUT |= CS; // release cs chip

	P9->OUT &= ~CS; // set CS chip low

	while (!(EUSCI_A3->IFG & 2))
		;					// wait for transmit buffer empty
	EUSCI_A3->TXBUF = data; // Send address

	while (EUSCI_A3->STATW & 1)
		;		   // wait for transmit buffer empty
	P9->OUT |= CS; // release cs chip
}
/*---------------------------------------------------------------------------------------------
This function sets all the digits to display 0.
-----------------------------------------------------------------------------------------------*/
void Reset_Segments(void)
{
	WriteData(0x01, 0x00);
	WriteData(0x02, 0x00);
	WriteData(0x03, 0x00);
	WriteData(0x04, 0x00);
	WriteData(0x05, 0x00);
	WriteData(0x06, 0x00);
	WriteData(0x07, 0x00);
	WriteData(0x08, 0x00);
}
/*---------------------------------------------------------------------------------------------
This function sends initialization instructions to the 7-seg display
-----------------------------------------------------------------------------------------------*/
void Segment_Init(void)
{
	WriteData(DECODE, DISPLAY_TEST);	 // define decode mode
	WriteData(INTENSITY, SET_INTENSITY); // define intensity
	WriteData(SCAN_LIMIT, ALL_DIGITS);	 // determine # of digits displayed	0x02
	WriteData(SHUTDOWN, NORMAL_MODE);	 // disable "shutdown"
}

void SysTick_Handler(void)
{
	if (!stopTime)
	{
		timeCount++;
	}
	displayTime(timeCount);
}

void PORT2_IRQHandler(void)
{
	if (P2->IFG & BUTTON1)
	{
		if (Button1Press())
		{
			stopTime = !stopTime;
			buttonCount++;
			displayCount(buttonCount);
		}
	}

	P2->IFG = 0; // clear flag
}

/*---------------------------------------------------------------------------------------------
This function sets up SPI for MSB first, 8-bit, master, 3-pin SPI, synchronous, SMCLK
-----------------------------------------------------------------------------------------------
*/
void SPI_Init(void)
{
	// Select alt functions for 2 pins (CLK,MOSI)
	P9->SEL0 |= (CLK | MOSI);
	P9->SEL1 &= ~(CLK | MOSI);

	// Configure gpio for 1 pin (CS)
	P9->SEL0 &= ~CS;
	P9->SEL1 &= ~CS; // configure P9.6 and P9.7 as GPIO
	P9->DIR |= CS;
	P9->OUT |= CS; // drive high to not select

	EUSCI_A3->CTLW0 = EUSCI_A_CTLW0_SWRST; // Disable USB0 during config
										   //	EUSCI_A3->CTLW0 |= UCSSEL_1 | UCSYNC | UCMST | UCMSB | UCCKPH;

	// EUSCI_A3->CTLW0 |= (EUSCI_A_CTLW0_SSEL__ACLK				   // SMCLK as clk source			// Multiple ones??
	// 					| EUSCI_A_CTLW0_SYNC					   // Synchronous Mode
	// 					| EUSCI_A_CTLW0_MSB						   //  MSB first select 		// not sure this is the right Macro
	// 					| EUSCI_A_CTLW0_MODE_0					   // 3-pin SPI
	// 					| EUSCI_A_CTLW0_MST | EUSCI_A_CTLW0_CKPH); // Master mode select
	EUSCI_A3->CTLW0 |= UCSSEL_1 | UCSYNC | UCMST | UCMSB | UCCKPH;

	// set the baud rate for the eUSCI which gets its clock from SMCLK
	// Clock_Init48MHz() from ClockSystem.c sets SMCLK = HFXTCLK/4 = 12 MHz
	// if the SMCLK is set to 12 MHz, divide by 3 for 4 MHz baud clock

	EUSCI_A3->BRW = 2;

	EUSCI_A3->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Software reset enable
}

void Button_init(void)
{
	P2->SEL0 &= ~(BUTTON1);
	P2->SEL1 &= ~(BUTTON1); // configure P2.6 and P2.7 as GPIO
	P2->DIR &= ~(BUTTON1);	// make P2.6 and P2.7 input
	P2->REN |= (BUTTON1);	// enable pull resistors
	P2->OUT |= (BUTTON1);	// Pins are pull-up

	P2->IES |= (BUTTON1); // Set pin interrupt trigger from high to low
	P2->IFG = 0;
	P2->IE |= (BUTTON1); // Set interrupts

	NVIC_EnableIRQ(PORT2_IRQn); // Enable interrupts on port 1
	__enable_interrupts();
}

uint16_t Button1Press(void)
{							   // Switch1 on Port P2.6
	static uint16_t State = 0; // State of switch

	State = (State << 1) | ((P2->IN & BUTTON1) >> 1) | 0xfc00;
	if (State == 0xfc00)
	{
		return 1;
	}

	return 0;
}
