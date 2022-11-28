#include "msp.h"
#include "motor.h"
#include "timers.h"
#include "I2C.h"
// #define ROWS BIT0 | BIT1 | BIT2 | BIT3  //P4.0 - 4.3
// #define COLS BIT4 | BIT5 | BIT6         //P4.4 - 4.6
#define BUTTON1 BIT6 // Pin 6 (Button 1)
#define BUTTON2 BIT7 // Pin 7 (Button 2)
#define RTC_SLAVE 0b1101000
#define EEPROM_SLAVE 0b1010010 // A1 of EEPROM

/**
 * main.c
 */

void Button_init(void);
uint16_t Button1Press(void);
uint16_t Button2Press(void);

unsigned char newData[8] = {0x11, 0x59, 0x82, 0x07, 0x31, 0x12, 0x22, 0};
unsigned char data[8] = {0x11, 0x59, 0x12, 0x07, 0x31, 0x12, 0x22, 0};
//                        sec   min    Hr    Day  Date   Mon   Yr
unsigned char mem[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0};

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

    Button_init();
    I2C_init();
    //int i;

    SysTick_Init();

    SysTick_Init();

    I2C_write(EEPROM_SLAVE, 0x02, 0x1);

    I2C_read(EEPROM_SLAVE, 0x02, &data[1]);

    ms_delay(10);

    printf("Value 1: %x\n", data[1]);

    while (1)
    {
        if (Button1Press())
        {
            I2C_write(RTC_SLAVE, 0x02, data[2]);
            I2C_write(EEPROM_SLAVE, 0x02, data[2]);
            while ((P2->IN & BUTTON2) == 0);

        }

        else if (Button2Press())
        {
            I2C_read(RTC_SLAVE,0x02, &newData[2]);
            I2C_read(EEPROM_SLAVE,0x02, &newData[3]);
            printf("B2:RTC-hex: %x\n",newData[2]);
            printf("B2:EEPROM-hex: %x\n",newData[3]);

            printf("data hex: %x\n",data[2]);

            while ((P2->IN & BUTTON2) == 0);
        }

        else
        {
            ;
        }
    }
}

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
