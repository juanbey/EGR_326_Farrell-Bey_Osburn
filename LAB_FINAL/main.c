#include "msp.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DEFAULT 0
#define MENU 1
#define TIME 2
#define DATE 3
#define PLAY 4

#define BUTTON1 BIT6    // P2.6
#define BUTTON2 BIT7    // P2.7

void Button_init(void);
uint16_t Button1Press(void);
uint16_t Button2Press(void);

uint16_t state = 0;

void main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    Button_init();
//    P2->SEL0 &= ~(BUTTON1 | BUTTON2);
//    P2->SEL1 &= ~(BUTTON1 | BUTTON2); // configure P2.6 and P2.7 as GPIO
//    P2->DIR &= ~(BUTTON1 | BUTTON2);  // make P2.6 and P2.7 input
//    P2->REN |= (BUTTON1 | BUTTON2);   // enable pull resistors
//    P2->OUT |= (BUTTON1 | BUTTON2);   // Pins are pull-up
//
//    P2->IES |= (BUTTON1 | BUTTON2); // Set pin interrupt trigger from high to low
//    P2->IFG = 0;
//    P2->IE |= (BUTTON1 | BUTTON2); // Set interrupts
//
//    NVIC_EnableIRQ(PORT2_IRQn); // Enable interrupts on port 2
//    __enable_interrupts();

    while(1){

        if (Button2Press())
        {
            state = MENU;
            while ((P2->IN & BUTTON2) == 0);
        }
        else if (Button1Press())
        {
            state = DEFAULT;
            while ((P2->IN & BUTTON1) == 0);
        }
        else
            ;

        switch(state)
        {
        case DEFAULT:
            ;
            break;

        case MENU:
            ;
            break;
        }
    }
}
//void PORT2_IRQHandler(void)
//{
//    P2->IFG = 0; // clear flag
//
//    if(P2->IN & BUTTON2){
//        state = MENU;
//    }
//
//    if(P2->IN & BUTTON1){
//        state = DEFAULT;
//    }
//
//    P2->IFG = 0; // clear flag
//}

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

