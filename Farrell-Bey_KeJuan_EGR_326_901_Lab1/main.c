/**************************************************************************************
* Title: Farrell-Bey_KeJuan_EGR_326_901_Lab1
* Filename: maincm
* Author: KeJuan Farrell-Bey & Jonathan Osburn
* Course: EGR 326-901 
* Date: 9/2/2022
* Assignment: Lab # 1: I/O Interfacing with the MSP432
* Instructor: Dr. Brian Krug
*
* This program is a state machone that has two parts. Part 1: when button 1 is pressed 
* the RGB LED connected to the MSP will cycle throuh the colors Red, Green, & Blue
* repeatedly until rest. Part 2: button 1 will work as in part 1 but when button 2 is
* pressed the current LED color will be held. Also if the LED is off and button 2 is 
* pressed the blue LED will be held. Finally if both button are pressed simultaneously
* the LEDs will reset and no color will be held until button 2 in pressed again.
*			 
* Revision: Initial draft
*****************************  Pins   **************************************************/


#include "msp.h"
#include <stdio.h>
#include <stdbool.h>

//Marcos for RGB light pins & state machine
#define RED BIT3     //pin 3
#define GREEN BIT4   //pin 4
#define BLUE BIT5  //pin 5
#define OFF 6   //OFF state

//Only Port 2 was used for this Project (All pin references are to port 2 i.e. 2.x)
uint16_t Button1Press(void);           //Function to Debounce a press on button 1 (2.7)
uint16_t Button2Press(void);           //Function to Debounce a press on button 2 (2.6)
void Button_init(void);                //Initializes Buttons Pins 6 & 7 (Switch)
void LED_init(void);                   //Initializes LED Pins 3, 4, & 5 
void SysTick_Init(void);               //Initializes of systic timer
void SysTick_delay (uint16_t delay);   //Function to set delay in ms


void main(void){
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     //Stop watchdog timer
    uint16_t state, color = RED;                    //current state of the machine & future state (color)
    bool pauseRed, pauseGreen, pauseBlue, bothPressed, onePress = false;    //Flags for pausing LED & if button 1 has been pressed  


    //Calling initialization funcitons
    LED_init();
    Button_init();
    SysTick_Init();
    P2->OUT &= ~(RED | BLUE | GREEN);   //Turn off all LEDs

    while(1){
        if(Button1Press()){
            onePress = true;                            //Once button 1 has been pressed the flag is always true
           while ( (P2->IN & BIT7) == 0){               //While button 1 is being held
               if(Button2Press()){                      //If button 2 is pressed
                   while ( (P2->IN & BIT6) == 0);       //Whille button 2 is being held do nothing
                   P2->OUT &= ~(RED | BLUE | GREEN);    //Turn off all LEDs
                   //Turn all pause flags to false
                   pauseRed = false;
                   pauseGreen = false;
                   pauseBlue = false;
                   color = RED;                         //Next state is red              
                   SysTick_delay(2500);                 //Delay 2.5ms
                   bothPressed = true;                  //Raise both buttons pressed flag 
                   break;
               }
           }

            if(!bothPressed){                           //If both button were not pressed
                state = color;                          //Change the current state to the next color
                switch(state){
                    // case OFF :                          //The LED should be off
                    // printf("off\n");

                    // if(pauseRed && pauseGreen && pauseBlue){    //If all pause flags are raised do nothing

                    // }
                    // else if(pauseRed && pauseGreen){            //If Red & Green pause flag raised 
                    //     P2->OUT &= ~(BLUE);                    //Turn off Blue LED
                    // }
                    // else if(pauseRed && pauseBlue){           //If Red & Blue pause flag raised 
                    //     P2->OUT &= ~(GREEN);                  //Turn off Green LED
                    // }
                    // else if(pauseGreen && pauseBlue){          //If Green & Blue pause flag raised 
                    //     //P2->OUT &= ~(RED);                    //Turn off Red LED
                    // }
                    // else if(pauseRed){                       //If Red pause flag raised 
                    //     P2->OUT &= ~(BLUE | GREEN);          //Turn off Blue & Green LEDs
                    // }
                    // else if(pauseGreen){                     //If Green pause flag raised 
                    //     P2->OUT &= ~(RED | BLUE);            //Turn off Red & Blue LEDs
                    // }
                    // else if(pauseBlue){                     //If Blue pause flag raised 
                    //     P2->OUT &= ~(RED | GREEN);          //Turn off Red & Green LEDs
                    // }
                    // else{       
                    //     P2->OUT &= ~(RED | BLUE | GREEN);   //Turn off all LEDs (R, G, B)
                    // }
                    // color = RED;                            //Next state is Red
                    // SysTick_delay(1000);                    //1ms
                    // break;

                case RED :                              //The LED should be Red
                    printf("red\n");
                    if(pauseRed && pauseGreen && pauseBlue){        //If all pause flags are raised do turn on all LEDs
                        P2->OUT |= (RED | GREEN | BLUE);
                    }
                    else if(pauseRed && pauseGreen){                //If Red & Green pause flag raised
                        P2->OUT &= ~(BLUE);                         //Turn off Blue LED
                        P2->OUT |= (RED | GREEN);                   //Turn on Red & Green LEDs
                    }
                    else if(pauseRed && pauseBlue){                 //If Red & Blue pause flag raised
                        P2->OUT &= ~(GREEN);                        //Turn off Green LED
                        P2->OUT |= (RED | BLUE);                    //Turn on Red & Blue LEDs
                    }
                    else if(pauseGreen && pauseBlue){               //If Blue & Green pause flag raised
                        P2->OUT |= (RED | GREEN | BLUE);            //Turn on Red, Blue, & Green LEDs
                    }
                    else if(pauseGreen){                             //If Green pause flag raised
                        P2->OUT &= ~(BLUE);                         //Turn off Blue LED
                        P2->OUT |= (RED | GREEN);                    //Turn on Green LEDs
                    }
                    else if(pauseBlue){                             //If Blue pause flag raised
                        P2->OUT &= ~(GREEN);                        //Turn off Green LED
                        P2->OUT |= (RED | BLUE);                    //Turn on Blue LEDs
                    }
                    else{
                        P2->OUT &= ~(RED | GREEN | BLUE);           //Turn off all LEDs (R, G, B)
                        P2->OUT |= (RED);                           //Turn on the Red LED
                    }
                    color = GREEN;                                  //The next state is Green
                    SysTick_delay(1000);                            //1ms
                    break;

                case GREEN :                                        //The color should be Green
                    if(pauseRed && pauseGreen && pauseBlue){        //If all pause flags are raised do turn on all LEDs
                        P2->OUT |= (RED | GREEN | BLUE);    
                    }
                    else if(pauseRed && pauseGreen){                //If Red & Green pause flag raised
                        P2->OUT &= ~(BLUE);                         //Turn off Blue LED
                        P2->OUT |= (RED | GREEN);                   //Turn on Red & Green LEDs
                    }
                    else if(pauseRed && pauseBlue){                 //If Red & Blue pause flag raised
                        P2->OUT |= (RED | BLUE | GREEN);            //Turn on Red, Green, & Blue LEDs
                    }
                    else if(pauseGreen && pauseBlue){               //If Blue & Green pause flag raised
                        P2->OUT &= ~(RED);                          //Turn off Red LED
                        P2->OUT |= (GREEN | BLUE);                  //Turn on Blue & Green LED
                    }
                    else if(pauseRed){                              //If Red pause flag raised
                        P2->OUT &= ~(BLUE);                         //Turn off Blue LED
                        P2->OUT |= (RED | GREEN);                   //Turn on Red & Green LEDs
                    }
                    else if(pauseBlue){                             //If Blue pause flag raised
                        P2->OUT &= ~(RED);                           //Turn off Red LED
                        P2->OUT |= (GREEN | BLUE);                  //Turn on Blue & Green LEDs
                    }
                    else{
                        P2->OUT &= ~(RED | GREEN | BLUE);          //Turn off all LEDs (R, G, B)
                        P2->OUT |= (GREEN);                        //Turn on the Green LED
                    }
                    color = BLUE;                                   //The next state is Blue
                    SysTick_delay(1000);                            //1ms
                    break;


                case BLUE :                                         //The color should be Blue
                    printf("blue\n");
                    if(pauseRed && pauseGreen && pauseBlue){        //If all pause flags are raised do turn on all LEDs
                        P2->OUT |= (RED | GREEN | BLUE);
                    }
                    else if(pauseRed && pauseGreen){                //If Red & Green pause flag raised
                        P2->OUT |= (RED | GREEN | BLUE);            //Turn on Red, Green, & Blue LEDs
                    }
                    else if(pauseRed && pauseBlue){                 //If Red & Blue pause flag raised
                        P2->OUT &= ~(GREEN);                        //Turn off Green LED
                        P2->OUT |= (RED | BLUE);                    //Turn on Red & Blue LED
                    }
                    else if(pauseGreen && pauseBlue){               //If Blue & Green pause flag raised
                        P2->OUT &= ~(RED);                          //Turn off Red LED
                        P2->OUT |= (GREEN | BLUE);                  //Turn on Blue & Green LED
                    }
                    else if(pauseRed){                              //If Red pause flag raised
                        P2->OUT &= ~(GREEN);                        //Turn off Green LED
                        P2->OUT |= (RED | BLUE);                    //Turn on Blue & Red LEDs
                    }
                    else if(pauseGreen){                            //If Green pause flag raised
                        P2->OUT &= ~(RED);                          //Turn off Red LED
                        P2->OUT |= (GREEN | BLUE);                  //Turn on Green & Blue LEDs
                    }
                    else{
                        P2->OUT &= ~(RED | GREEN | BLUE);           //Turn off all LEDs (R, G, B)
                        P2->OUT |= (BLUE);                          //Turn on Blue LED
                    }
                    color = RED;                                    //Next state is Off
                    SysTick_delay(1000);                            //1ms
                    break;
                }
            }

            else{
                bothPressed = false;                                //Reset both button pressed flag
            }
      }

        else if(Button2Press()){                                  //If button 2 is pressed
            while ( (P2->IN & BIT6) == 0){                        //While it's being held
                if(Button1Press()){                               //If button 1 is pressed
                    while ( (P2->IN & BIT7) == 0);                //While button 1 is being held do nothing
                    P2->OUT &= ~(RED | BLUE | GREEN);             //Turn off all LEDs
                    //Turn all pause flags to false
                    pauseRed = false;
                    pauseGreen = false;
                    pauseBlue = false;
                    color = RED;                                 //The next state is Red
                    SysTick_delay(2500);                         //2.5ms
                    bothPressed = true;                          //Raise both buttons pressed flag      
                    break;
                }
            }

        if(!bothPressed){                                               //If both buttons were not pressed
            if(state == OFF || state == BLUE || onePress == false){     //If the LED is off, Blue, or button 1 has not been pressed
                pauseBlue = true;                                       //Raise pause Blue flag
                P2->OUT |= BLUE;                                        //Turn on Blue LED
            }
            else if(state == RED){                                      //If LED is Red 
                pauseRed = true;                                        //Raise pause Red flag
            }
            else if(state == GREEN){                                    //If LED is Green
                pauseGreen = true;                                      //Raise pause Green flag
            }
        }

        else{
            bothPressed = false;                                        //Reset both buttons pressed flag
        }

       }
     }
}


//Function for Part 1
void Part1(void){
    uint16_t color = 3;                     //Represents the bit of each color 

    if(Button1Press()){
        P2->OUT &= ~(BIT3 | BIT4 | BIT5);    //Turn off all LEDs (R, G, B)
        P2->OUT |= BIT(color);               //Turn on "color" LED
        color++;                             //Increment color
        while ( (P2->IN & BIT7) == 0);       //While button is held do nothing
        SysTick_delay(1000);                 //1ms
        if(color == 6){                      //Reset counter after all colors are cycled
            color = 3;
        }
    }
}

/********************************************************************
This function detects a button press and debounces the button bounce
by shifting 1 into the state variable until the debounce signal passes
through which send a string of 0 into the variable which is followed by 
another nibble of 1s.
*********************************************************************/
uint16_t Button1Press(void){         // Switch1 on Port P2.7
    static uint16_t State = 0;      //State of switch

    State = (State << 1) | ((P2->IN & BIT7) >> 1) | 0xfc00;    
    SysTick_delay(5);

    if(State == 0xfc00){
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
uint16_t Button2Press(void){         // Switch1 on Port P2.6
    static uint16_t State = 0;

    State = (State << 1) | ((P2->IN & BIT6) >> 1) | 0xfc00;

    if(State == 0xfc00){
        return 1;
    }

    return 0;
}



/********************************************************************
This function initalizes pins 6 & 7 on port 2 as input GPIO to recieve
input from a button switch.
*********************************************************************/
void Button_init(void){
    P2->SEL0 &= ~(BIT6 | BIT7);
    P2->SEL1 &= ~(BIT6 | BIT7);   //configure P2.6 and P2.7 as GPIO
    P2->DIR &= ~(BIT6 | BIT7);    //make P2.6 and P2.7 input
    P2->REN |= (BIT6 | BIT7);     //enable pull resistors 
    P2->OUT |= (BIT6 | BIT7);     //Pins are pull-up
}

/********************************************************************
This function initalizes pins 3, 4, & 5 on port 2 as ouput GPIO to 
transmit power to the LED pins. 
*********************************************************************/
void LED_init(void){
    P2->SEL0 &= ~(BIT3 | BIT4 | BIT5);
    P2->SEL1 &= ~(BIT3 | BIT4 | BIT5);      //configure P2.3, P2.4, & P2.5 as GPIO
    P2->DIR |= (BIT3 | BIT4 | BIT5);       //Configure GPIO for output on P2.4 Green, P2.5 Blue, & P2.1 Red LED  on MSP4332 Launchpad
    P2->OUT &= ~(BIT3 | BIT4 | BIT5);
}

/********************************************************************
This function initalizes the SysTick timer at 3MHz w/ no interrupts
*********************************************************************/
void SysTick_Init (void){                   //Initialization of systic timer
    SysTick -> CTRL  = 0;                   // disable SysTick During step
    SysTick -> LOAD = 0x00FFFFFF;           // max reload value
    SysTick -> VAL     = 0;                 // any write to current clears it
    SysTick -> CTRL  = 0x00000005;          // enable systic, 3MHz, No Interrupts
  }

/********************************************************************
This function calls the SysTick timer and waits for the input value
in ms.

@param delay - integer value of desired delay time in ms
*********************************************************************/
void SysTick_delay (uint16_t delay){          // Systick delay function
    SysTick -> LOAD = ((delay * 3000) - 1);               //delay for 1 msecond per delay value
    SysTick -> VAL     = 0;                              // any write to CVR clears it
    while ( (SysTick -> CTRL  &  0x00010000) == 0);    // wait for flag to be SET
  }

