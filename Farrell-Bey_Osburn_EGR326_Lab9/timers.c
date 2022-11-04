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

	CS->CTL1 = CS->CTL1 | CS_CTL1_DIVS_2; // change the SMCLK clock speed to 12 MHz.    //change to div 4 for 3 mghz

	CS->KEY = 0; // Lock CS module from unintended accesses
}

/********************************************************************
This function sets 3 different modules of TimerA to be able to do PWM
for the trigger signal and LED brightness and capture/compare of the
echo signal.
*********************************************************************/
void TimerA_init(void)
{
	// TimerA0.2 Pin2.5 (Echo)
	TIMER_A1->CCR[0] |= 60060 - 1; // max of 20ms

	TIMER_A1->CTL = TIMER_A_CTL_SSEL__SMCLK |	 // use SMCLK
					TIMER_A_CTL_MC__UP | // in Up mode
					TIMER_A_CTL_CLR;			 // Clear TAR to start

	TIMER_A1->CCTL[0] |= TIMER_A_CCTLN_CCIE;	  // Enable capture interrupt

	NVIC_EnableIRQ(TA1_0_IRQn); // enable interrupts from timerA1
	__enable_irq();
}
/********************************************************************
The handler captures the "high" time of the echo signal by clearing
the CCR, count value when a rising edge is seen and then copying the
value when the echo signal ends. The clk cycle count is converted to
inches later.
*********************************************************************/
