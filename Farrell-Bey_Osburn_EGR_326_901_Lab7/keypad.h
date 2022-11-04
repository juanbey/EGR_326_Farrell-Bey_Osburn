
#ifndef KEYPAD_H_
#define KEYPAD_H_
#include "msp.h"
#include <stdio.h>
#include <string.h>
#include "timers.h"

#define ROWS BIT0 | BIT1 | BIT2 | BIT3  //P4.0 - 4.3
#define COLS BIT4 | BIT5 | BIT6         //P4.4 - 4.6

//---------------------------------------------------------------------------
/*Function Prototypes*/
uint8_t Read_Keypad();
void GetPin(int, int[]);
void keypad_init();
void keypad_irq_init(void);

#endif /* KEYPAD_H_ */
