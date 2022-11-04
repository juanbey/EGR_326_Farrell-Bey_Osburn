#include "msp.h"
#include "motor.h"
#include <stdio.h>
#include <stdbool.h>
#include "timers.h"

// TODO:change title to unipolar motor
void Motor_Pin_Init(void)
{
	P8->SEL0 &= ~(INT1 | INT2 | INT3 | INT4);
	P8->SEL1 &= ~(INT1 | INT2 | INT3 | INT4); // GPIO
	P8->DIR |= (INT1 | INT2 | INT3 | INT4);	  // Set as outputs
	P8->OUT &= ~(INT3 | INT4 | INT1 | INT2); // Start all as off
}

void FullStep(int phase, bool direction)
{
	if (direction)
	{
		switch (phase)
		{
		case 0:
			P8->OUT &= ~(ALL);		  // turn everything Turn off previous phase
			P8->OUT |= (INT1 | INT3); // only turn on needed pins
			ms_delay(30);
			break;
		case 1:
			P8->OUT &= ~(ALL);		  // turn everything Turn off previous phase
			P8->OUT |= (INT2 | INT3); // only turn on needed pins
			ms_delay(30);
			break;
		case 2:
			P8->OUT &= ~(ALL);		  // turn everything Turn off previous phase
			P8->OUT |= (INT2 | INT4); // only turn on needed pins
			ms_delay(30);
			break;
		case 3:
			P8->OUT &= ~(ALL);		  // turn everything Turn off previous phase
			P8->OUT |= (INT4 | INT1); // only turn on needed pins
			ms_delay(30);
			break;
		default:
			break;
		}
	}

	else
	{
		switch (phase)
		{
		case 0:
			P8->OUT &= ~(ALL);		  // turn everything off
			P8->OUT |= (INT1 | INT4); // only turn on needed pins
			ms_delay(30);
			break;
		case 1:
			P8->OUT &= ~(ALL);		  // turn everything off
			P8->OUT |= (INT2 | INT4); // only turn on needed pins
			ms_delay(30);
			break;
		case 2:
			P8->OUT &= ~(ALL);		  // turn everything off
			P8->OUT |= (INT2 | INT3); // only turn on needed pins
			ms_delay(30);
			break;
		case 3:
			P8->OUT &= ~(ALL);		  // turn everything off
			P8->OUT |= (INT3 | INT1); // only turn on needed pins
			ms_delay(30);
			break;
		default:
			break;
		}
	}
}


void WaveDrive(int phase, bool direction, int speed)
{
    speed *= 10;
	if (direction)
	{
		switch (phase)
		{
		case 0:
			P8->OUT &= ~(ALL);		  // turn everything Turn off previous phase
			P8->OUT |= (INT1); // only turn on needed pins
			ms_delay(speed);
			break;
		case 1:
			P8->OUT &= ~(ALL);		  // turn everything Turn off previous phase
			P8->OUT |= (INT2); // only turn on needed pins
			ms_delay(speed);
			break;
		case 2:
			P8->OUT &= ~(ALL);		  // turn everything Turn off previous phase
			P8->OUT |= (INT3); // only turn on needed pins
			ms_delay(speed);
			break;
		case 3:
			P8->OUT &= ~(ALL);		  // turn everything Turn off previous phase
			P8->OUT |= (INT4); // only turn on needed pins
			ms_delay(speed);
			break;
		default:
			break;
		}
	}

	else
	{
		switch (phase)
		{
		case 0:
			P8->OUT &= ~(ALL);		  // turn everything off
			P8->OUT |= (INT4); // only turn on needed pins
			ms_delay(speed);
			break;
		case 1:
			P8->OUT &= ~(ALL);		  // turn everything off
			P8->OUT |= (INT3); // only turn on needed pins
			ms_delay(speed);
			break;
		case 2:
			P8->OUT &= ~(ALL);		  // turn everything off
			P8->OUT |= (INT2); // only turn on needed pins
			ms_delay(speed);
			break;
		case 3:
			P8->OUT &= ~(ALL);		  // turn everything off
			P8->OUT |= (INT1); // only turn on needed pins
			ms_delay(speed);
			break;
		default:
			break;
		}
	}
}


// // Obsolete w/ unipolar use for bipolar
// void HalfStep(int phase, bool direction)
// {
// 	if (direction)
// 	{
// 		switch (phase)
// 		{
// 		case 0:
// 			P8->OUT &= ~(INT4); // turn everything off
// 			P8->OUT |= (INT1);	// only turn on needed pins
// 			ms_delay(30);
// 			break;
// 		case 1:
// 			P8->OUT |= (INT1 | INT3); // only turn on needed pins
// 			ms_delay(30);
// 			break;
// 		case 2:
// 			P8->OUT &= ~(INT1); // turn everything off
// 			P8->OUT |= (INT3);	// only turn on needed pins
// 			ms_delay(30);
// 			break;
// 		case 3:
// 			P8->OUT |= (INT3 | INT2); // only turn on needed pins
// 			ms_delay(30);
// 			break;
// 		case 4:
// 			P8->OUT &= ~(INT2); // turn everything Turn off previous phase
// 			P8->OUT |= (INT2);	// only turn on needed pins
// 			ms_delay(30);
// 			break;
// 		case 5:
// 			P8->OUT |= (INT2 | INT4); // only turn on needed pins
// 			ms_delay(30);
// 			break;
// 		case 6:
// 			P8->OUT &= ~(INT2); // turn everything off
// 			P8->OUT |= (INT4);	// only turn on needed pins
// 			ms_delay(30);
// 			break;
// 		case 7:
// 			P8->OUT |= (INT4 | INT1); // only turn on needed pins
// 			ms_delay(30);
// 			break;
// 		default:
// 			break;
// 		}
// 	}

// 	else
// 	{
// 		switch (phase)
// 		{
// 		case 0:
// 			P8->OUT |= (INT1 | INT4); // only turn on needed pins
// 			ms_delay(30);
// 			P8->OUT &= ~(INT1); // turn everything Turn off previous phase
// 			break;
// 		case 1:
// 			P8->OUT |= (INT2 | INT4); // only turn on needed pins
// 			ms_delay(30);
// 			P8->OUT &= ~(INT4); // turn everything off
// 			break;
// 		case 2:
// 			P8->OUT |= (INT2 | INT3); // only turn on needed pins
// 			ms_delay(30);
// 			P8->OUT &= ~(INT2); // turn everything off
// 			break;
// 		case 3:
// 			P8->OUT |= (INT3 | INT1); // only turn on needed pins
// 			ms_delay(30);
// 			P8->OUT &= ~(INT3); // turn everything off
// 			break;
// 		default:
// 			break;
// 		}
// 	}
// }

void CyclesToStep(int cycles, bool direction)
{
	int i, j;
	for (i = 0; i < cycles; i++)
	{
		for (j = 0; j < 4; j++)
		{
			FullStep(j, direction);
		}
	}
}

void DegreesToStep(int degrees, bool direction)
{ // 170 steps 180 degress so 9/170 = 18.9 ~ 20 steps / 20 deg
	int i;
	CyclesToStep(10* degrees, direction);


}
