#include "msp.h"
#include <stdio.h>
#include <stdbool.h>

#define TRIG BIT4   // P2.4 - TimerA0.1
#define ECHO BIT7   // P7.7 - TimerA1.1
#define LED BIT6    // P5.6 - TimerA2.2

#define PERIOD 45045        // Trigger Period
#define DUTY 8

void TimerA_init(void);
void Port_init(void);
void SysTick_Init_Interrupt(void);
void LED_PWM(void);


// set up global variables
volatile bool rising, falling, halfFreq = false;
uint32_t pulseWidth, inches = 0;
volatile double finches = 0;
volatile uint32_t dutyCycle, tempdutyCycle = 940;              //10% duty cycle
volatile uint32_t period = 9398;          //For 40 Hz



void main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    rising = true;              // set flags to start "catching" the rising edge of the echo signal
    falling = false;

    Port_init();
    TimerA_init();                  // initialize pins and clocks/timers
    SysTick_Init_Interrupt();

    while(1){
        inches = pulseWidth/100;    // rough conversion for inches to decide the duty cycle
        finches = pulseWidth/100.0; // number of inches to displayed to the user

        if(inches > 10){            // automatically set inches to a null value if above the 10in.
          inches = 11;
        }
//Part II --------------
//        if(halfFreq){               // if below 1in, blink LED at 2Hz
//          TIMER_A2->CCR[1]  = 0;
//          SysTick_delay(250);
//          TIMER_A2->CCR[1]   = 0XFFFF;
//          SysTick_delay(250);
//          halfFreq = false;         // reset 1in flag
//        }
//        else if(tempdutyCycle != dutyCycle){        // if dutyCycle has changed
//            TIMER_A2->CCR[1]  =  dutyCycle;         // Update duty cycle for LED
//            tempdutyCycle = dutyCycle;              // update checker value
//        }
//        else
//            ;
//-------------

//Part I --------------
        if(tempdutyCycle != dutyCycle){        // if dutyCycle has changed
                    TIMER_A2->CCR[1]  =  dutyCycle;         // Update duty cycle for LED
                    tempdutyCycle = dutyCycle;              // update checker value
                }
//--------------
        LED_PWM();                          // switch duty_cycle based on inches calculated above.
    }
}

/********************************************************************
This function sets up 3 pins to work with TimerA
*********************************************************************/
void Port_init(void){
    P2->SEL0 |= (TRIG);
    P2->SEL1 &= ~(TRIG);    //configure P2.4 for TimerA0.1
    P2->DIR |= (TRIG);      //make P2.4 an output

    P5->SEL0 |= (LED);
    P5->SEL1 &= ~(LED);     //configure P5.6 for TimerA2.2
    P5->DIR |= (LED);       //make P5.6 an output

    P7->SEL0 |= (ECHO);
    P7->SEL1 &= ~(ECHO);    //configure P7.7 for TimerA1.1
    P7->DIR &= ~(ECHO);     //make P7.7 an Input
}
/********************************************************************
This function sets 3 different modules of TimerA to be able to do PWM
for the trigger signal and LED brightness and capture/compare of the
echo signal.
*********************************************************************/
void TimerA_init (void){
    //TimerA0 (Trig)
    TIMER_A0->CCR[0]  |= PERIOD;                // PWM Period (# cycles of clock)

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK|    // use SMCLK
                    TIMER_A_CTL_MC__UP     |    // in Up mode
                    TIMER_A_CTL_ID__4      |    // Divide by 4
                    TIMER_A_CTL_CLR;            // Clear TAR to start

    //TimerA0.1 Pin2.4 (Trig)
    TIMER_A0->CCR[1] = DUTY;       // CCR1 PWM duty cycle in 10ths of percent
    TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_OUTMOD_7;     // CCR1 reset/set mode 7

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
/////////////////////////////////////////////////////////////////////////////
    // TimerA0.2 Pin2.5 (Echo)
    TIMER_A1->CCTL[1] |= TIMER_A_CCTLN_CM_3 |   // Capture on both rising and falling edges
                        TIMER_A_CCTLN_CCIS_0|   // Use CCI1A (input goes directly to pin)
                        TIMER_A_CCTLN_CCIE  |   // Enable capture interrupt
                        TIMER_A_CCTLN_CAP   |   // Enable capture mode
                        TIMER_A_CCTLN_SCS;      // Synchronous capture

    TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK     |   // use SMCLK
                    TIMER_A_CTL_MC__CONTINUOUS  |   // in Up mode
                    TIMER_A_CTL_ID__4           |   // Divide by 4
                    TIMER_A_CTL_CLR;                // Clear TAR to start

    NVIC_EnableIRQ(TA1_N_IRQn);             // enable interrupts from timerA1
    __enable_irq();
}
/********************************************************************
The handler captures the "high" time of the echo signal by clearing
the CCR, count value when a rising edge is seen and then copying the
value when the echo signal ends. The clk cycle count is converted to
inches later.
*********************************************************************/
void TA1_N_IRQHandler(void) {           // TimerA1 interrupt service routine
    if(rising){
        TIMER_A1->R = 0;                // reset TimerA counter at address/source
        TIMER_A1->CCTL[1] &= ~1;        // clear echo interrupt flag

        falling = true;
        rising = false;                 // switch state
     }
     else if(falling){
         pulseWidth = TIMER_A1->CCR[1]; // grab counter value that is stored in the CCR

         rising = true;
         falling = false;               // switch state
      }

}

 /********************************************************************
 This function initializes the SysTick Timer with interrupts at 2
 second intervals.
 *********************************************************************/
 void SysTick_Init_Interrupt (void){    //initialization of systic timer
     SysTick -> CTRL = 0;               // disable SysTick During step
     SysTick -> LOAD = 6000000;         //reload value for 2 s interrupts
     SysTick -> VAL = 0;                // any write to current clears it
     SysTick -> CTRL  = 0x00000007;     // enable systic, 3MHz, Interrupts
 }

/********************************************************************
This function calls the SysTick timer and waits for the input value
in ms.

@param delay - integer value of desired delay time in ms
*********************************************************************/
void SysTick_delay (uint16_t delay){          // Systick delay function
    SysTick -> LOAD = ((delay * 3000) - 1);                 //delay for 1 msecond per delay value
    SysTick -> VAL     = 0;                                 // any write to CVR clears it
    while ( (SysTick -> CTRL  &  0x00010000) == 0);         // wait for flag to be SET
  }


/********************************************************************
The handler prints off the distance to the user.
*********************************************************************/
void SysTick_Handler(void){
    printf("%f: in\n", (finches));  // print off pulse width value on the 2s interrupt
    //printf("%f: in\n", (finches));  // print off pulse width value on the 2s interrupt
}

/********************************************************************
This function looks at the distance then changes the value of percent
which is used to manipulate the duty cycle variable. duty cycle will
then be put into the CCR register.

The 2Hz flag is set off when distance is below 1in (0in). It is false
at every other possible distance. If any other distance is measured,
it is assume the sensor is picking up random numbers, so the percent
value is held.
*********************************************************************/
void LED_PWM(void) {

    uint32_t percent = 0;                                  //Duty cycle for LED PWM

    switch(inches) {
        case 0:
            halfFreq = true;        // go into 2Hz conditional statement
            break;
        case 1:
            percent = 100;
            halfFreq = false;       // Do not go into 2Hz conditional statement
            break;
        case 2:
            halfFreq = false;
            percent = 20;
            break;
        case 3:
            halfFreq = false;
            percent = 30;
            break;
        case 4:
            halfFreq = false;
            percent = 40;
            break;
        case 5:
            halfFreq = false;
            percent = 50;
            break;
        case 6:
            halfFreq = false;
            percent = 60;
            break;
        case 7:
            halfFreq = false;
            percent = 70;
            break;
        case 8:
            halfFreq = false;
            percent = 80;
            break;
        case 9:
            halfFreq = false;
            percent = 90;
            break;
        case 10:
            halfFreq = false;
            percent = 0;
            break;
        case 11:
            halfFreq = false;
            percent = 0;
            break;
        default:
            percent = percent;
            break;
    }
    dutyCycle = period * (percent / 100.0);        //set duty cycle as the period multiplied by the percentag
}
