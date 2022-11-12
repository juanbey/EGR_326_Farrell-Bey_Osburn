#include "msp.h"
#include <stdbool.h>
#include "timers.h"

#define BUTTON1 BIT6 // P3.6
//#define BUTTON1 BIT6 // P2.6
#define LED1 BIT7    // P2.7

void Button_init(void);      // Initializes Buttons Pin 6 (Push Buttons)
void LED_init(void);      // Initializes LED Pin 7 (Push Buttons)
uint16_t Button1Press(void);
void TA1_0_IRQHandler(void);
void timerInit(void);

bool pressed =  false;

uint16_t count = 0;
uint32_t totalTime = 0;
uint16_t i = 0;
void main(void)
{



    // TODO: configure WDT to have a 4s timeout interval, soft reset mode, ACLK using REFOCLK
    // WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;      // stop watchdog timer

    WDT_A->CTL =    0x5A00 // Watchdog Password
                    | 1<<5 //Set to ACLK
                    | 0<<4 //Set to Watchdog mode
                    | 1<<3 // Clear Timer
                    | 3; //Set to 2^19 interval (16 seconds)

    Button_init();          // initialize P2.6 for the button
    LED_init();             // initialize P2.7 for LED
    TimerA_init();
    // timerInit();

    P2->OUT |= LED1;
    while(i <= 50000)
        i++;

    i = 0;

    P2->OUT &= ~LED1;        // Blink LED at beginning of
    while(i <= 50000)
        i++;

    i = 0;
    P2->OUT |= LED1;


    while(1)
    {

        pressed = 0;

        if(totalTime >= 2000)   // count up to 1 second
        {
            count++;            // increment second counter
            totalTime = 0;      // reset counter
        }

        // TODO: if(count >= 4)...
    }

    // TODO: Blink LED at beginning of program

    // TODO: set timer interrupt for 1s
        // send timer value to LCD

    // TODO: set up GPIO interrupts for a button that resets counter and WDT


}

void LED_init(void)      // Initializes LED Pin 7 (Push Buttons)
{
    P2->SEL0 &= ~(LED1);
    P2->SEL1 &= ~(LED1); // configure P2.7 as GPIO
    P2->DIR |= (LED1);  // make P2.7 output
    P2->OUT &= ~(LED1);   // Turn off

}
//void Button_init(void)
//{
//    P2->SEL0 &= ~(BUTTON1);
//    P2->SEL1 &= ~(BUTTON1); // configure P2.6 as GPIO
//    P2->DIR &= ~(BUTTON1);  // make P2.6 input
//    P2->REN |= (BUTTON1);   // enable pull resistors
//    P2->OUT |= (BUTTON1);   // Pins are pull-up
//
//    P2->IES |= (BUTTON1); // Set pin interrupt trigger from high to low
//    P2->IFG = 0;
//    P2->IE |= (BUTTON1); // Set interrupts
//
//    NVIC_EnableIRQ(PORT2_IRQn); // Enable interrupts on port 1
//    __enable_interrupts();
//}
void Button_init(void)
{
//    P2->SEL0 &= ~(BUTTON1);
//    P2->SEL1 &= ~(BUTTON1); // configure P2.6 as GPIO
    P3->DIR &= ~(BUTTON1);  // make P2.6 input
    P3->REN |= (BUTTON1);   // enable pull resistors
    P3->OUT |= (BUTTON1);   // Pins are pull-up

    P3->IES |= (BUTTON1); // Set pin interrupt trigger from high to low
    P3->IFG = 0;
    P3->IE |= (BUTTON1); // Set interrupts

    NVIC_EnableIRQ(PORT2_IRQn); // Enable interrupts on port 2
    NVIC_EnableIRQ(PORT3_IRQn); // Enable interrupts on port 3
    __enable_interrupts();
}

//void PORT2_IRQHandler(void)
//{
//
//    if (P2->IFG & BUTTON1)
//    {
//        if (Button1Press())
//        {
//            pressed = 1;
//
//            WDT_A->CTL =    0x5A00 // Watchdog Password
//                            | 1<<5 //Set to ACLK
//                            | 0<<4 //Set to Watchdog mode
//                            | 1<<3 // Clear Timer
//                            | 4; //Set to 2^19 interval (16 seconds)
//            P2->IFG = 0; // clear flag
//            // WDT_A->CTL = (WDT_A->CTL & ~0x2100) | 0x1208;
//        }
//        else{
//            ;
//        }
//    }
//}
void PORT3_IRQHandler(void)
{

    if (P3->IFG & BUTTON1)
    {
        if (Button1Press())
        {
            pressed = !pressed;
            count = 0;

            WDT_A->CTL =    0x5A00 // Watchdog Password
                            | 1<<5 //Set to ACLK
                            | 0<<4 //Set to Watchdog mode
                            | 1<<3 // Clear Timer
                            | 3; //Set to 2^19 interval (16 seconds)
            P3->IFG = 0; // clear flag
            // WDT_A->CTL = (WDT_A->CTL & ~0x2100) | 0x1208;
        }
        else{
            ;
        }
    }
}

//uint16_t Button1Press(void)
//{                              // Switch1 on Port P2.6
//    static uint16_t State = 0; // State of switch
//
//    State = (State << 1) | ((P2->IN & BUTTON1) >> 1) | 0xfc00;
//    if (State == 0xfc00)
//    {
//        return 1;
//    }
//    return 0;
//}
uint16_t Button1Press(void)
{                              // Switch1 on Port P2.6
    static uint16_t State = 0; // State of switch

    State = (State << 1) | ((P3->IN & BUTTON1) >> 1) | 0xfc00;
    if (State == 0xfc00)
    {
        return 1;
    }
    return 0;
}

//void TimerA_init(void)
//{
//    // TimerA0.2 Pin2.5 (Echo)
//    TIMER_A1->CCR[0] |= 60060 - 1; // max of 20ms
//
//    TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK |    // use SMCLK (12MHz)
//                    TIMER_A_CTL_MC__UP | // in Up mode
//                    TIMER_A_CTL_CLR;             // Clear TAR to start
//
//    TIMER_A1->CCTL[0] |= TIMER_A_CCTLN_CCIE;      // Enable capture interrupt
//
//    NVIC_EnableIRQ(TA1_0_IRQn); // enable interrupts from timerA1
//    __enable_irq();
//}

void TA1_0_IRQHandler(void)
{ // TimerA1 interrupt service routine
    totalTime += 20;
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // Clear the flag in CCTL[0]
}
//
//void timerInit(void)
//{
//    TIMER_A0->CTL = 0b0000000111010100; // ACLK, Divide by 8, Count Up, Clear to start
//    TIMER_A0->CCR[0] = 1024;
//    TIMER_A0->CCR[1] = 1024; // Count to 1 second
//
//    // TIMER_A0->EX0 = 0b0000000000000111;           //divider of 8
//    TIMER_A0->CCTL[1] = 0b0000000011110100; // CCR1 reset/set mode 7, with interrupt.
//    TIMER_A0->CCTL[1] &= ~BIT0; // Clear Timer A0.1 interrupt flag
//    TIMER_A0->CCTL[1] &= ~BIT1; // Clear Timer A0.1 overflow flag
//
//    count = 0;
//
//    // ST7735_DrawChar(51, 64, number, 0xFFFF, 0x0000, 4); // draws the number in the center
//}
