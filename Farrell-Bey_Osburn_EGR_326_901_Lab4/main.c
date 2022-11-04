#include "msp.h"
#include <stdio.h>

#define ROWS BIT0 | BIT1 | BIT2 | BIT3
#define COLS BIT4 | BIT5 | BIT6

void keypad_init(void);         // prototype for GPIO initialization
uint8_t Read_Keypad(void);      // prototype for keypad scan subroutine
void Print_Keys(void);            // Print Key pressed
void SysTick_Init (void);
void SysTick_delay (uint16_t delay);


uint8_t num,key;



void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int pin[4];

    SysTick_Init();
    keypad_init();

    while(1){
       key = Read_Keypad();

       if(key==1){
            Print_Keys();
       }
        SysTick_delay(10);
    }
}




void keypad_init(void){
       P4->SEL0 &= ~(ROWS | COLS);
       P4->SEL1 &= ~(ROWS | COLS);                         //configure P4.0, P4.1, P4.2, P4.3, P4.4, P4.5, P4.6  as GPIO
       P4->DIR &= ~(ROWS | COLS);                          //make P4.0, P4.1, P4.2, P4.3, P4.4, P4.5, P4.6 input
       P4->REN |= (ROWS);     //enable pull resistors on rows
       P4->OUT |= (ROWS);     // Rows are pull-up
}


uint8_t Read_Keypad(void){
    uint8_t col, row;

    for ( col = 0; col < 3; col++ ){
          P4->DIR = 0x00;                 // Set Columns to inputs
          P4->DIR |= BIT(4 + col);        // Set column 3 to output
          P4->OUT &= ~BIT(4 + col);      // Set column 3 to LOW

          SysTick_delay(10);           // Delay the while loop
          row = P4->IN & 0x0F;                  // read all rows

          while (!(P4IN & BIT0) | !(P4IN & BIT1) | !( P4IN & BIT2) | !( P4IN & BIT3));

              if (row != 0x0F){
                  break;                  // if one of the input is low, some key is pressed.
              }
      }

        P4->DIR = 0x00;                             // Set Columns to inputs

        if (col == 3){
            return 0;
        }


        if (row == 0x0E){
            num = col + 1;        // key in row 0
        }

        if (row == 0x0D){
            num = 3 + col + 1;       // key in row 1
        }

        if (row == 0x0B){
            num = 6 + col + 1;       // key in row 2
        }

        if (row == 0x07){
            num = 9 + col + 1;       // key in row 3
        }

        return 1;
}



void Print_Keys(void){

    if (num < 10){              //If num is less than 10 print the value
        printf("%d\n",num);

    }

    if (num == 10){              //If num is 10 print * as the value

        printf("*\n");
       }

    if (num == 11){                //If num is 11 print 0 as the value
        printf("0\n");
    }

    if (num == 12){                 //If num is 12 print # as the value
        printf("#\n");
    }
}



void SysTick_Init (void){        //initialization of systic timer
    SysTick -> CTRL  = 0;                      // disable SysTick During step
    SysTick -> LOAD = 0x00FFFFFF;             // max reload value
    SysTick -> VAL     = 0;                  // any write to current clears it
    SysTick -> CTRL  = 0x00000005;          // enable systic, 3MHz, No Interrupts
  }


void SysTick_delay (uint16_t delay){          // Systick delay function
    SysTick -> LOAD = ((delay * 3000) - 1);               //delay for 1 msecond per delay value
    SysTick -> VAL     = 0;                              // any write to CVR clears it
    while ( (SysTick -> CTRL  &  0x00010000) == 0);    // wait for flag to be SET
  }


