/**************************************************************************************
 * Title: Farrell-Bey_KeJuan_EGR_326_901_Lab2
 * Filename: main.c
 * Author: KeJuan Farrell-Bey & Jonathan Osburn
 * Course: EGR 326-901
 * Date: 9/2/2022
 * Assignment: Lab # 2:
 * Instructor: Dr. Brian Krug
 *
 * This program is toggle an external LED continuosly if button 1 is pressed and held the RGB
 *			 LED will cycle and blink every second.
 * Revision: Initial draft
 *****************************  Pins   **************************************************/

#include "msp.h"
#include <stdio.h>
#include <stdbool.h>

// Marcos for RGB light pins & buttons machine
#define RED BIT0     // Pin 3 (Red LED)
#define GREEN BIT1   // pin 4 (Green LED)
#define BLUE BIT2    // pin 5 (Blue LED)
#define BUTTON1 BIT6 // Pin 6 (Button 1)
#define BUTTON2 BIT7 // Pin 7 (Button 2)
#define MAXLEDS 3    // Maximum number of LED

void Button_init(void);            // Initializes Buttons Pins 6 & 7 (Push Buttons)
void LED_init(void);               // Initializes RGB LEDs Pin 0, 1, & 2
void SysTick_Init_Interrupt(void); // Initializes the SysTick Timer w/ Interrupts
uint16_t Button1Press(void);       // Function to Debounce a press on button 1 (2.6)
uint16_t Button2Press(void);       // Function to Debounce a press on button 2 (2.7)

/**
 * main.c
 */
volatile bool tryDebounce1, tryDebounce2, firstPress = true; // Flags for Debounce of each button & first button 1 press
volatile uint32_t timeout = 1000;                            // Variable for SysTick Handler

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

    // Initialize functions
    Button_init();
    LED_init();
    SysTick_Init_Interrupt();

    uint32_t i = 0;                               // Counter variable for color array
    uint64_t color[MAXLEDS] = {RED, GREEN, BLUE}; // Color array for LED Pin values
    timeout = 0;                                  // Reset timeout flag to 0
    P2->OUT &= ~(RED | GREEN | BLUE);             // Turn off LEDs

    while (1)
    {
        if (tryDebounce2 && Button2Press())
        { // If Flag Raised & Button 2 Press Debounced
            while ((P2->IN & BUTTON2) == 0)
            { // Detect hold
                if (firstPress)
                { // If button 1 hasn't been pressed
                  // Do Nothing
                }
                else if (i == 0)
                { // If next index (color) is 0 (red )
                    if (timeout)
                    {                                  // If SysTick TImer flag raised
                        P2->OUT ^= color[MAXLEDS - 1]; // Toggle current (blue) LED
                        timeout = 0;                   // Lower flag
                    }
                }
                else
                {
                    if (timeout)
                    {                            // If SysTick TImer flag raised
                        P2->OUT ^= color[i - 1]; // Toggle current LED
                        timeout = 0;             // Lower flag
                    }
                }
            }
            if (i == 0)
            {                                  // If Next color is red
                P2->OUT |= color[MAXLEDS - 1]; // Keep blue on
            }

            else
            {
                P2->OUT |= color[i - 1]; // Keep current color on
            }
            tryDebounce2 = false; // Reset debounce flag
        }

        else if (tryDebounce1 && Button1Press())
        {                                     // If Flag Raised & Button 1 Press Debounced
            P2->OUT &= ~(RED | GREEN | BLUE); // Turn off LEDs
            if (firstPress)
            {                        // If this is the first press
                i = 0;               // Ensure index is 0 (red)
                P2->OUT |= color[i]; // Turn on LED
                i++;                 // Increment index
                firstPress = false;  // lower flag
            }

            else
            {
                P2->OUT |= color[i]; // Turn on current indexed color
                i++;                 // Increment index
            }

            while ((P2->IN & BUTTON1) == 0)
                ; // While button 1 is still being held do nothing

            if (i >= 3)
            { // If max reached reset cycle
                i = 0;
            }
            tryDebounce1 = false; // Reset debounce flag
        }
    }
}

/********************************************************************
Interrupt Serivce Ruoutine for port 2 that sets the tryDebounce flag(s)
*********************************************************************/
void PORT2_IRQHandler(void)
{
    if (P2->IFG & BUTTON2)
    {
        tryDebounce2 = true;
    }

    else if (P2->IFG & BUTTON1)
    {
        tryDebounce1 = true;
    }

    P2->IFG = 0; // clear flag
}

/********************************************************************
Interrupt Serivce Ruoutine for SysTick Timer that sets the timeout flag
*********************************************************************/
void SysTick_Handler(void)
{
    timeout = 1;
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
{                              // Switch1 on Port P2.7
    static uint16_t State = 0; // State of switch

    State = (State << 1) | ((P2->IN & BUTTON2) >> 1) | 0xfc00;
    if (State == 0xfc00)
    {
        return 1;
    }

    return 0;
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
This function initalizes pins 3, 4, & 5 on port 2 as ouput GPIO to
transmit power to the LED pins.
*********************************************************************/
void LED_init(void)
{
    P2->SEL0 &= ~(RED | GREEN | BLUE);
    P2->SEL1 &= ~(RED | GREEN | BLUE); // configure P2.3, P2.4, & P2.5 as GPIO
    P2->DIR |= (RED | GREEN | BLUE);   // Configure GPIO for output on P2.4 Green, P2.5 Blue, & P2.1 Red LED  on MSP4332 Launchpad
    P2->OUT &= ~(RED | GREEN | BLUE);
}

/********************************************************************
This function initializes the SysTick Timer with interrupts at 1
second intervals.
*********************************************************************/
void SysTick_Init_Interrupt(void)
{                               // initialization of systic timer
    SysTick->CTRL = 0;          // disable SysTick During step
    SysTick->LOAD = 2250000;    // reload value for .75 s interrupts
    SysTick->VAL = 0;           // any write to current clears it
    SysTick->CTRL = 0x00000007; // enable systic, 3MHz, Interrupts
}
