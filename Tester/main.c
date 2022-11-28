#include "msp.h"
#include <stdio.h>
#include <string.h>
#include "I2C.h"
#include "timers.h"

#define MAX 8
#define BUTTON1 BIT6 // Pin 6 (Button 1)
#define BUTTON2 BIT7 // Pin 7 (Button 2)
#define RTC_SLAVE 0b1101000
#define EEPROM_SLAVE 0b01010001 // A0 of EEPROM

void Button_init(void);
uint16_t Button1Press(void);
uint16_t Button2Press(void);
// 10ms delay for write
/**
 * main.c
 */
volatile entryNum = 0x1;
volatile currentMem = 0x00;
unsigned char Date[6][MAX] = {
    {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
}; // Entry# sec   min    Hr    Day  Date   Mon   Yr

unsigned char data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void main(void)

{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer
    //uint8_t i = 0;
    SysTick_Init();
    //Button_init();
    I2C_init();



    I2C_write(EEPROM_SLAVE, 0x02, 0x1);

    I2C_read(EEPROM_SLAVE, 0x02, &data[1]);

    ms_delay(10);

    printf("Value 1: %x\n", data[1]);

    while (1)
    {

        ;

    }
}

/********************************************************************
This function initalizes pins 6 & 7 on port 2 as input GPIO to recieve
input from a button switch.
*********************************************************************/
void Button_init(void)
{
    P2->SEL0 &= ~(BUTTON1 | BUTTON2);
    P2->SEL1 &= ~(BUTTON1 | BUTTON2); // configure P2.6 and P2.7 as GPIO
    P2->DIR &= ~(BUTTON1 | BUTTON2);  // make P2.6 and P2.7 input
    P2->REN |= (BUTTON1 | BUTTON2);   // enable pull resistors
    P2->OUT |= (BUTTON1 | BUTTON2);   // Pins are pull-up
}

/********************************************************************
This function detects a button press and debounces the button bounce
by shifting 1 into the state variable until the debounce signal passes
through which send a string of 0 into the variable which is followed by
another nibble of 1s.
*********************************************************************/
uint16_t Button1Press(void)
{                              // Switch1 on Port P2.6
    static uint16_t State = 0; // State of switch

    State = (State << 1) | ((P2->IN & BUTTON1) >> 1) | 0xfc00;
    if (State == 0xfc00)
    {
        return 1;
    }

    return 0;
}

/********************************************************************
This function detects a button press and debounces the button bounce
by shifting 1 into the state variable until the debounce signal passes
through which send a string of 0 into the variable which is followed by
another nibble of 1s.
*********************************************************************/
uint16_t Button2Press(void)
{                              // Switch1 on Port P2.6
    static uint16_t State = 0; // State of switch

    State = (State << 1) | ((P2->IN & BUTTON2) >> 1) | 0xfc00;
    if (State == 0xfc00)
    {
        return 1;
    }

    return 0;
}
