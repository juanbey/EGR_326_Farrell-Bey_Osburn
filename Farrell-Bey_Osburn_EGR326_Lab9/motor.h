#ifndef MOTOR_H_
#define MOTOR_H_

#include "msp.h"
#include <stdio.h>
#include <stdbool.h>
#include "timers.h"

//
#define INT1 BIT4 // P8.4
#define INT2 BIT5 // P8.5
#define INT3 BIT6 // P8.6
#define INT4 BIT7 // P8.7
#define ALL (BIT4 | BIT5| BIT6| BIT7)

void FullStep(int phase, bool direction);
void Motor_Pin_Init();
void CyclesToStep(int cycles, bool direction);
void DegreesToStep(int cycles, bool direction);
//void HalfStep(int phase, bool direction);
void WaveShift(int phase, bool direction, int speed);
void WaveDrive(int speed);

#endif /* KEYPAD_H_ */
