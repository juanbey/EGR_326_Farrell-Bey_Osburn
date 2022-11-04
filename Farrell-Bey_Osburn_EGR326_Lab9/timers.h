#ifndef TIMERS_H_
#define TIMERS_H_

#include "msp.h"
#include <stdio.h>


void SysTick_Init(void); // Initializes the SysTick Timer w/ Interrupts
void ms_delay(uint16_t delay);
void sec_delay(uint16_t delay);
void Clock_Init48MHz(void);
//void Delay1ms(uint32_t n);
//void parrotdelay(unsigned long ulCount);
void TimerA_init(void);

#endif /* KEYPAD_H_ */
