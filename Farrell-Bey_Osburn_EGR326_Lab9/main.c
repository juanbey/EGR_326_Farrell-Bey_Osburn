#include "msp.h"
#include "timers.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "ST7735.h"
#include "motor.h"
#include "keypad.h"

/* Pins to ST7735 LCD
CLK     P9.5
SDA     P9.7
RS      P9.2
RST     P9.3
CS      P9.4
*/

#define HALL BIT6 // P2.6

#define CS BIT4   // P9.4
#define CLK BIT5  // P9.5
#define MOSI BIT7 // P9.7

void Hall_init(void); // Initializes P2.6 (Hall Effect Sensor)

char *open = "OPEN\n";
char *closed = "CLOSED\n";

bool gateClosed = false;
bool first, odd = true;

volatile uint16_t rpm, totalTime, rot_count, speed = 0;

void main(void)

{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

    Clock_Init48MHz(); // set system clock to 48 MHz
    TimerA_init();
    Motor_Pin_Init();
    keypad_irq_init();
    Hall_init(); // the Hall Effect Sensor
    Output_Init();
    ST7735_FillScreen(ST7735_BLACK); // set screen to black
    ST7735_SetTextColor(ST7735_WHITE);
    Output_Clear();
    //    ST7735_DrawCharS(10 * 6, 7 * 10, 56, ST7735_WHITE, ST7735_BLACK, 4);

    while (1)
    {
        WaveDrive(speed);

        //        if (gateClosed)
        //        { // flag toggled high by port interrupt
        //            ST7735_SetCursor(3.6, 8);
        //            ST7735_OutString(open); // display OPEN on LCD
        //
        //            gateClosed = 0; // reset flag
        //        }
        //
        //        if ((P2->IN & HALL) != 0)
        //        { // if back to high
        //            ST7735_SetCursor(3.6, 8);
        //            ST7735_OutString(closed); // display CLOSED on LCD
        //        }

        if (totalTime >= 5000)
        {
            rpm = rot_count * 20;
            ST7735_DrawCharS(10 * 6, 7 * 10, (48 + rpm), ST7735_WHITE, ST7735_BLACK, 4);

            rot_count = 0;
            totalTime = 0;
            // print out rpm
        }
    }
}

void TA1_0_IRQHandler(void)
{ // TimerA1 interrupt service routine
    totalTime += 20;
    TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // Clear the flag in CCTL[0]
}

void PORT2_IRQHandler(void)
{
    P2->IFG = 0; // clear flag



    if(odd)
    {                    // starts as true
        TIMER_A1->R = 0; // reset TimerA counter at address/source
    }

    else
    {
        totalTime = TIMER_A1->CCR[1]; // grab counter value that is stored in the CCR
        TIMER_A1->R = 0;              // reset TimerA counter at address/source
    }

    odd = !odd;

    gateClosed = 1; // set flag

    // clear counter
}

void PORT4_IRQHandler(void)
{
    int num = 25;
    num = Read_Keypad();
    P4->IFG = 0;

    if (num > 5)
    {
        ;
    }
    else
    {
        speed = num;
    }
}

///*---------------------------------------------------------------------------------------------
// This function initializes a pin to read the Hall Effect Sensor
//-----------------------------------------------------------------------------------------------
//*/
void Hall_init(void)
{
    P2->SEL0 &= ~(HALL);
    P2->SEL1 &= ~(HALL); // configure P2.6 and P2.7 as GPIO
    P2->DIR &= ~(HALL);  // make P2.6 and P2.7 input
    P2->REN |= (HALL);   // enable pull resistors
    P2->OUT |= (HALL);   // Pins are pull-up

    P2->IES |= (HALL); // Set pin interrupt trigger from high to low
    P2->IFG = 0;
    P2->IE |= (HALL); // Set interrupts

    NVIC_EnableIRQ(PORT2_IRQn); // Enable interrupts on port 1
    __enable_interrupts();
}
