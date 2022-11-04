#include "msp.h"
#include <stdio.h>
#include "timers.h"

/********************************************************************
This function initializes the SysTick Timer with interrupts at 1
second intervals.
*********************************************************************/
void SysTick_Init(void)
{								// Initialization of systic timer
	SysTick->CTRL = 0;			// disable SysTick During step
	SysTick->LOAD = 0x00FFFFFF; // max reload value
	SysTick->VAL = 0;			// any write to current clears it
	SysTick->CTRL = 0x00000005; // enable systic, 3MHz, No Interrupts
}

/********************************************************************
This function initializes the SysTick Timer with interrupts at 2
second intervals.
*********************************************************************/
void SysTick_Init_Interrupt(void)
{								// initialization of systic timer
	SysTick->CTRL = 0;			// disable SysTick During step
	SysTick->LOAD = 3000000;	// reload value for 1 s interrupts
	SysTick->VAL = 0;			// any write to current clears it
	SysTick->CTRL = 0x00000007; // enable systic, 3MHz, Interrupts
}


void ms_delay(uint16_t delay)
{										  // Systick delay function
	SysTick->LOAD = ((delay * 3000) - 1); // delay for 1 msecond per delay value
	SysTick->VAL = 0;					  // any write to CVR clears it
	while ((SysTick->CTRL & 0x00010000) == 0)
		; // wait for flag to be SET
}

void sec_delay(uint16_t delay)
{											 // Systick delay function
	SysTick->LOAD = ((delay * 3000000) - 1); // delay for 1 msecond per delay value
	SysTick->VAL = 0;						 // any write to CVR clears it
	while ((SysTick->CTRL & 0x00010000) == 0)
		; // wait for flag to be SET
}

void Clock_Init48MHz(void)
{
	// Configure Flash wait-state to 1 for both banks 0 & 1
	FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
						 FLCTL_BANK0_RDCTL_WAIT_1;
	FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) |
						 FLCTL_BANK1_RDCTL_WAIT_1;

	// Configure HFXT to use 48MHz crystal, source to MCLK & HSMCLK*
	PJ->SEL0 |= BIT2 | BIT3; // Configure PJ.2/3 for HFXT function
	PJ->SEL1 &= ~(BIT2 | BIT3);
	CS->KEY = CS_KEY_VAL; // Unlock CS module for register access
	CS->CTL2 |= CS_CTL2_HFXT_EN | CS_CTL2_HFXTFREQ_6 | CS_CTL2_HFXTDRIVE;
	while (CS->IFG & CS_IFG_HFXTIFG)
		CS->CLRIFG |= CS_CLRIFG_CLR_HFXTIFG;

	/* Select MCLK & HSMCLK = HFXT, no divider */
	CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK |
							CS_CTL1_DIVM_MASK |
							CS_CTL1_SELS_MASK |
							CS_CTL1_DIVHS_MASK) |
			   CS_CTL1_SELM__HFXTCLK |
			   CS_CTL1_SELS__HFXTCLK;

	CS->CTL1 = CS->CTL1 | CS_CTL1_DIVS_2; // change the SMCLK clock speed to 12 MHz.

	CS->KEY = 0; // Lock CS module from unintended accesses
}
//
// void Delay1ms(uint32_t n)
//{
//	while (n)
//	{
//		parrotdelay(5901); // 1 msec, tuned at 48 MHz
//		n--;
//	}
//}
//
//#ifdef __TI_COMPILER_VERSION__
//// Code Composer Studio Code
// void parrotdelay(unsigned long ulCount)
//{
//	__asm("pdloop:  subs    r0, #1\n"
//		  "    bne    pdloop\n");
// }
//
//#else
//// Keil uVision Code
//__asm void
// parrotdelay(unsigned long ulCount)
//{
//	subs r0, #1 bne parrotdelay
//				 bx lr
//}
//
//#endif
