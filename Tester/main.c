#include "msp.h"
#include "motor.h"
#include "timers.h"
#include "keypad.h"
//#define ROWS BIT0 | BIT1 | BIT2 | BIT3  //P4.0 - 4.3
//#define COLS BIT4 | BIT5 | BIT6         //P4.4 - 4.6

/**
 * main.c
 */
//void keypad_irq_init(void);

volatile uint8_t speed = 0;

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

	int i;

	SysTick_Init();
	Motor_Pin_Init();
	keypad_irq_init();

	while(1)
	{
		for (i = 0; i < 4; i++)
		{
			WaveDrive(i, 1, speed);
		}
	}
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



