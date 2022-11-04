#include "msp.h"
#include <stdio.h>
#include <string.h>
#include "keypad.h"
#include "I2C.h"

#define MAX 7
#define RTC_SLAVE 0b1101000

char *getDay(int);
char *getMonth(int);
unsigned char seconds, minutes, hours, day, date, month, year = 0;
uint8_t key, temp;
int pin[4] = {0, 0, 0, 0}; // array to hold number entry
enum dateTimetype
{
    sec,
    min,
    hr,
    dy,
    dt,
    mon,
    yr
};
unsigned char setDate[MAX] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // sec   min    Hr    Day  Date   Mon   Yr
unsigned char day[MAX - 4];
unsigned char time[MAX - 4];
unsigned char temperature[MAX - 5];

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

    int i = 0;

    I2C_init();
    SysTick_Init();
    keypad_init();

    //     for(i = 0; i < MAX; i++){
    //         I2C_write(RTC_SLAVE, i, &date[i]);         // Write date/time to RTC
    //     }
    printf("Enter * then 1 for Date, 2 for Time, or 3 for Temperature (C)\n");
    while (1)
    {
        do
        {
            key = Read_Keypad();
            if (key != 0)
            {
                setDateTime(key, pin, setDate[i], i); // get the number
                key = 0;
            }

            /* code */
        } while (i < yr);

        key = Read_Keypad();
        if (key != 0)
        {
            GetPin(key, pin); // get the number pressed
            key = 0;
        }

        if (pin[1] == 10)
        {
            I2C_print(setDateTime);
        }
    }
}

char *getDay(int day)
{
    switch (day)
    {
    case 1:
        return "Monday";
        break;
    case 2:
        return "Tuesday";
        break;
    case 3:
        return "Wednesday";
        break;
    case 4:
        return "Thursday";
        break;
    case 5:
        return "Friday";
        break;
    case 6:
        return "Saturday";
        break;
    case 7:
        return "Sunday";
        break;
    default:;
        break;
    }
}

char *getMonth(int month)
{
    switch (month)
    {
    case 0x01:
        return "January";
        break;
    case 0x02:
        return "February";
        break;
    case 0x03:
        return "March";
        break;
    case 0x04:
        return "April";
        break;
    case 0x05:
        return "May";
        break;
    case 0x06:
        return "June";
        break;
    case 0x07:
        return "July";
        break;
    case 0x08:
        return "August";
        break;
    case 0x09:
        return "September";
        break;
    case 0x10:
        return "October";
        break;
    case 0x11:
        return "November";
        break;
    case 0x12:
        return "December";
        break;
    default:;
        break;
    }
}
