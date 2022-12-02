#include "msp.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "ST7735.h"
#include "timers.h"

#define LED BIT6    // P5.6 - TimerA2.2

// Photoresistor: P5.5

/* Pins to ST7735 LCD
CLK     P9.5
SDA     P9.7
RS      P9.2
RST     P9.3
CS      P9.4
*/


volatile uint32_t period = 76;          //For ~120kHz

uint32_t result = 0;
uint32_t voltage = 0;
uint16_t dutyCycle = 76;
uint16_t temp_intensity, intensity = 0;


void adcsetup(void);
void Port_init(void);
void TimerA_init(void);

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	Clock_Init48MHz();  // change MCLK to 48MHz and SMCLK to 12MHz
	Output_Init();      // set LCD
	SysTick_Init();     // initialize SysTick
	adcsetup();         // Initialize ADC module
	TimerA_init();      // initialize PWM for LED
	Port_init();        // Initialize ports

	ST7735_SetRotation(2);

	ST7735_FillScreen(ST7735_BLACK);    // set screen to black
    ST7735_SetTextColor(ST7735_WHITE);  // set text color to white
    Output_Clear();                     // clear LCD to start

    ST7735_SetCursor(10 * 2, 3 * 10);

    while(1)
    {
        ADC14->CTL0 |=1; 				//start a conversion
        while(!ADC14->IFGR0); 			// wait until conversion complete
        result = ADC14->MEM[5]; 		// immediately store value in a variable


        // change LCD and PWM based of of voltage from photodiode

        if((result >= 1500*0 + 100) && (result < 1500*1 + 100))
        {
            dutyCycle = 76;
            intensity = 10;
        }
        else if((result >= 1500*1 + 100) && (result < 1500*2 + 100))
        {
            dutyCycle = 70;
            intensity = 9;
        }
        else if((result >= 1500*2 + 100) && (result < 1500*3 + 100))
        {
            dutyCycle = 63;
            intensity = 8;
        }
        else if((result >= 1500*3 + 100) && (result < 1500*4 + 100))
        {
            dutyCycle = 56;
            intensity = 7;
        }
        else if((result >= 1500*4 + 100) && (result < 1500*5 + 100))
        {
            dutyCycle = 49;
            intensity = 6;
        }
        else if((result >= 1500*5 + 100) && (result < 1500*6 + 100))
        {
            dutyCycle = 42;
            intensity = 5;
        }
        else if((result >= 1500*6 + 100) && (result < 1500*7 + 100))
        {
            dutyCycle = 35;
            intensity = 4;
        }
        else if((result >= 1500*7 + 100) && (result < 1500*8 + 100))
        {
            dutyCycle = 28;
            intensity = 3;
        }
        else if((result >= 1500*8 + 100) && (result < 1500*9 + 100))
        {
            dutyCycle = 21;
            intensity = 2;
        }
        else if((result >= 1500*9 + 100) && (result < 1500*10 + 100))
        {
            dutyCycle = 14;
            intensity = 1;
        }
        else if(result > 1500*10)
        {
            dutyCycle = 7;
            intensity = 0;
        }
        else
            ;

        if(temp_intensity != intensity)         // if intensity changes
        {
            Output_Clear();                     // clear LCD before writing characters

            if(intensity == 10){
                 ST7735_DrawCharS(10 * .7, 5 * 10, '1', ST7735_WHITE, ST7735_BLACK, 10);        // print 10 on LCD
                 ST7735_DrawCharS(10 * 6.5, 5 * 10, '0', ST7735_WHITE, ST7735_BLACK, 10);
            }
            else
                ST7735_DrawCharS(10 * 4, 5 * 10, intensity + 48, ST7735_WHITE, ST7735_BLACK, 10);// print 0-9 on LCD

            TIMER_A2->CCR[1]  =  dutyCycle;         // Update duty cycle for LED
        }

        temp_intensity  = intensity;            // update temp so it does not unnecessarily run again

        ms_delay48(1000);                       // wait a second before recalculating

    }
}
void Port_init(void){
    P5->SEL0 |= (LED);
    P5->SEL1 &= ~(LED);     //configure P5.6 for TimerA2.2
    P5->DIR |= (LED);       //make P5.6 an output
}

void adcsetup(void)
{
ADC14->CTL0 = 0x00000010; 	//power on and disabled during configuration
ADC14->CTL0 |= 0x04D80300; 	// S/H pulse mode, MCLCK, 32 sample clocks,
							//sw trigger, /4 clock divide
ADC14->CTL1 = 0x00000030; 	// 14-bit resolution
ADC14->MCTL[5] = 0; 		// A0 input, single ended, vref=avcc
P5->SEL1 |= 0x20; 			// configure pin 5.5 for AO
P5->SEL0 |= 0x20;
ADC14->CTL1 |= 0x00050000; 	//start convert at mem reg 5
ADC14->CTL0 |= 2; 			//enable ADC after configuration
}

void TimerA_init (void){
////////////////////////////////////////////////////////////////////////////
    //TimerA2 (LED)
    TIMER_A2->CCR[0]  |= period;                // PWM Period (# cycles of clock)

    //TimerA2.2 Pin5.6 (LED)
    TIMER_A2->CCR[1]   =  dutyCycle;           // Initial duty cycle
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;         // Reset/set
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK|    // use SMCLK
                    TIMER_A_CTL_MC__UP     |    // in Up mode
                    TIMER_A_CTL_ID_3      |    // Divide by 8
                    TIMER_A_CTL_CLR;            // Clear TAR to start

    // NVIC_EnableIRQ(TA1_N_IRQn);             // enable interrupts from timerA1
    // __enable_irq();
    TIMER_A2->CCR[1]  =  dutyCycle;         // Set duty cycle for LED
}
