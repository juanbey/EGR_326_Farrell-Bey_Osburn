#include "msp.h"
#include <stdio.h>
#include <string.h>
#include "keypad.h"
#include "timers.h"

volatile int32_t userVal, digits = 0; // values of decade encoder  and user entered value

void keypad_init(void)
{
    P4->SEL0 &= ~(ROWS | COLS);
    P4->SEL1 &= ~(ROWS | COLS); // configure P4.0, P4.1, P4.2, P4.3, P4.4, P4.5, P4.6  as GPIO
    P4->DIR &= ~(ROWS | COLS);  // make P4.0, P4.1, P4.2, P4.3, P4.4, P4.5, P4.6 input
    P4->REN |= (ROWS);          // enable pull resistors on rows
    P4->OUT |= (ROWS);          // Rows are pull-up
}

void keypad_irq_init(void)
{
    P4->SEL0 &= ~(ROWS | COLS);
    P4->SEL1 &= ~(ROWS | COLS); // configure P4.0, P4.1, P4.2, P4.3, P4.4, P4.5, P4.6  as GPIO
    P4->DIR &= ~(ROWS | COLS);  // make P4.0, P4.1, P4.2, P4.3, P4.4, P4.5, P4.6 input
    P4->REN |= (ROWS);          // enable pull resistors on rows
    P4->OUT |= (ROWS);          // Rows are pull-up

    P4->IES |= (ROWS); // Set pin interrupt trigger from high to low

    P4->IFG = 0;

    P4->IE |= (ROWS); // Set interrupts

    NVIC_EnableIRQ(PORT4_IRQn); // Enable interrupts on port 1
    __enable_interrupts();
}


uint8_t Read_Keypad(void)
{
    uint8_t col, row, num;

    for (col = 0; col < 3; col++)
    {
        P4->DIR = 0x00;           // Set Columns to inputs
        P4->DIR |= BIT(4 + col);  // Set column 3 to output
        P4->OUT &= ~BIT(4 + col); // Set column 3 to LOW

        Delay1ms(10);        // Delay the while loop
        row = P4->IN & 0x0F; // read all rows

        while (!(P4IN & BIT0) | !(P4IN & BIT1) | !(P4IN & BIT2) | !(P4IN & BIT3))
            ;

        if (row != 0x0F)
        {
            break; // if one of the input is low, some key is reset.
        }
    }

    P4->DIR = 0x00; // Set Columns to inputs

    if (col == 3)
    {
        return 0;
    }

    if (row == 0x0E)
    {
        num = col + 1; // key in row 0
    }

    if (row == 0x0D)
    {
        num = 3 + col + 1; // key in row 1
    }

    if (row == 0x0B)
    {
        num = 6 + col + 1; // key in row 2
    }

    if (row == 0x07)
    {
        num = 9 + col + 1; // key in row 3
    }

    if (num == 11)
    {
        num = 0;
    }

    return num;
}

void GetPin(int input, int pin[])
{
    int temp; // Temporary vars for shifting

    if ((input >= 1 && input <= 3) || input == 10)
    {
        temp = pin[0]; // Shift each digit in the pin over and fill in the next user input

        pin[1] = temp;
        pin[0] = input;
        digits++;
        if (input == 10)
        {
            printf("Entered: *\n"); // If there are no invalid values, print eneterd value
        }
        else
        {
            printf("Entered: %d\n", input); // If there are no invalid values, print eneterd value
        }
    }

    else
    {
        printf("Invalid Value Entered Please Try Again!\n");
        digits = 0;
        pin[0] = 25;
        pin[1] = 25;
    }

    if (digits == 2)
    {
        if (pin[0] == 10)
        {
            printf("Error: Enter * first then select options 1, 2, or 3\n"); // If there are no invalid values, print
            pin[0] = 25;
            pin[1] = 25;
            digits = 0;
        }
        // userVal = (pin[1] * 10) + pin[0];
    }
}

// void setDateTime(int input, int pin[4], int *output, int *type)
//{
//     int temp, temp1, temp2, i;                                               // Temporary vars for shifting
//     unsigned char setDate[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // sec   min    Hr    Day  Date   Mon   Yr
//     enum type
//     {
//         sec,
//         min,
//         hr,
//         dy,
//         dt,
//         mon,
//         yr
//     };
//
//     if (input != 12 || input != 10)
//     { //    if ((input >= 1  && input <= 3) || input == 10) {
//         if (input == 11)
//         {
//             input = 0;
//         }
//
//         temp = pin[0]; // Shift each digit in the pin over and fill in the next user input
//         temp1 = pin[1];
//         temp2 = pin[2];
//
//         pin[3] = temp2;
//         pin[2] = temp1;
//         pin[1] = temp;
//         pin[0] = input;
//     }
//
//     else
//     {
//         printf("Invalid Value Entered Please Try Again!\n");
//         for (i = 0; i < 4; i++)
//         { // Reset all values
//             pin[i] = 0;
//         }
//         digits = 0;
//     }
//
//     switch (type)
//     {
//     case sec:
//         if (digits == 2)
//         {
//             if (pin[1] > 5 || pin[0] > 9)
//             {
//                 printf("Error: Enter Valid Time for Seconds\n"); // If there are no invalid values, print
//             }
//
//             else
//             {
//                 setDate[sec] = (pin[1] * 10) + pin[0];
//                 *output = setDate[sec];
//             }
//
//             for (i = 0; i < 4; i++)
//             { // Reset all values
//                 pin[i] = 0;
//             }
//             digits = 0;
//         }
//         break;
//     case min:
//         if (digits == 2)
//         {
//             if (pin[1] > 5 || pin[0] > 9)
//             {
//                 printf("Error: Enter Valid Time for Minutes\n"); // If there are no invalid values, print
//             }
//
//             else
//             {
//                 setDate[min] = (pin[1] * 10) + pin[0];
//                 *output = setDate[min];
//             }
//
//             for (i = 0; i < 4; i++)
//             { // Reset all values
//                 pin[i] = 0;
//             }
//             digits = 0;
//         }
//         break;
//     case hr:
//         if (digits == 2)
//         {
//             if ((pin[1] >= 2 && pin[0] > 4) || (pin[1] > 2) || (pin[0] > 9))
//             {
//                 printf("Error: Enter Valid Time for the Hour\n"); // If there are no invalid values, print
//             }
//
//             else
//             {
//                 setDate[hr] = (pin[1] * 10) + pin[0];
//                 *output = setDate[hr];
//             }
//
//             for (i = 0; i < 4; i++)
//             { // Reset all values
//                 pin[i] = 0;
//             }
//             digits = 0;
//         }
//         break;
//     case dy:
//         if (digits == 1)
//         {
//             if (pin[0] > 7)
//             {
//                 printf("Error: Enter Valid Day of the week\n"); // If there are no invalid values, print
//             }
//
//             else
//             {
//                 setDate[dy] = pin[0];
//                 *output = setDate[dy];
//             }
//
//             for (i = 0; i < 4; i++)
//             { // Reset all values
//                 pin[i] = 0;
//             }
//             digits = 0;
//         }
//         break;
//     case dt:
//         if (digits == 2)
//         {
//             if (pin[1] > 3 || (pin[1] == 3 && pin[0] > 1) || (setDate[mon] == 02 && (pin[1] > 2 && pin[0] > 9)))
//             {
//                 printf("Error: Enter Valid Day of the Month\n"); // If there are no invalid values, print
//             }
//
//             else
//             {
//                 setDate[dt] = (pin[1] * 10) + pin[0];
//                 *output = setDate[dt];
//             }
//
//             for (i = 0; i < 4; i++)
//             { // Reset all values
//                 pin[i] = 0;
//             }
//             digits = 0;
//         }
//         break;
//     case mon:
//         if (digits == 2)
//         {
//             if ((pin[1] > 1 && pin[0] > 2) || pin[1] > 1)
//             {
//                 printf("Error: Enter Valid Month\n"); // If there are no invalid values, print
//             }
//
//             else
//             {
//                 setDate[mon] = (pin[1] * 10) + pin[0];
//                 *output = setDate[mon];
//             }
//
//             for (i = 0; i < 4; i++)
//             { // Reset all values
//                 pin[i] = 0;
//             }
//             digits = 0;
//         }
//         break;
//     case yr:
//         if (digits == 3)
//         {
//             if (pin[2] > 1 || pin[1] > 9 || pin[0] > 9)
//             {
//                 printf("Error: Enter Valid Month\n"); // If there are no invalid values, print
//             }
//
//             else
//             {
//                 setDate[yr] = (pin[2] * 100) + (pin[1] * 10) + pin[0];
//                 *output = setDate[yr];
//             }
//
//             for (i = 0; i < 4; i++)
//             { // Reset all values
//                 pin[i] = 0;
//             }
//             digits = 0;
//
//             break;
//
//         default:
//
//             break;
//         }
//         //!!
//     }
// }
