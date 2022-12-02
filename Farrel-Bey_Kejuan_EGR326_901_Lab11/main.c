/********************************************************
 * Name: Yuki Nishibashi and Jaime Malone
 * Date: Nov. 18th 2022
 * Course: EGR326-903
 * Instructor: Dr. Nabeeh Kandalaft
 * Lab: Lab11 - Storing data on an external EEPROM via I2C
 * File: Lab11
 * Description: This program is intended to incorporate the EEPROM and RTC.
 *              the goal is to read the RTC then store the date data into the EEPROM
 *              up to 5 times, at which point the first object saved is then overwritten.
 *              The user is able to interface with this system using two pushbuttons, both of
 *              which operate using interrupts.
 *
 *              Note: The EEPROM is very sensitive when reading and writing, therefore it REQUIRES
 *              a 10 ms delay between every read or write call.
 *
 ********************************************************/


/***********************Include**************/
#include "msp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ST7735.h"

/***********************Defines**************/

#define SLAVE_ADDRESS_RTC 0x68 //address of the RTC
#define SLAVE_ADDRESS_EEPROM 0x57 //address of EEPROM

//struct to hold date and time data
typedef struct Date {

    unsigned char seconds;
    unsigned char minutes;
    unsigned char hours;
    unsigned char day;
    unsigned char date;
    unsigned char month;
    unsigned char year;

} Date;

/************************Prototypes************/
void I2C_init(void);
int I2C_Write(int slaveAddr, unsigned char memAddr, unsigned char data);
int I2C_Read(int slaveAddr, unsigned char memAddr, unsigned char* data);
void pin_init(void);
void SysTick_delay(uint16_t delay_ms);
void SysTick_init(void);
void Print_info(int bank); //function to print data onto LCD


/*********************Global Variables*********/
uint8_t print_flag; //flag set when button is pressed, wishing to print mem bank
uint8_t save_flag; //flag set when button is pressed, wishing to store time

//variables used to break up data from RTC, allowing it to be printed onto the RTC
unsigned char seconds[2];
unsigned char minutes[2];
unsigned char hours[2];
unsigned char day[2];
unsigned char date[2];
unsigned char month[2];
unsigned char year[2];

unsigned char print_track = 0x00; //used to track where we are in the memory (for printing)
unsigned char store_track = 0x00; //used to track where we are in the memory (for storing)


/*********************MAIN***********************/

void main(void)
{

    I2C_init(); //initialize LCD
    SysTick_init(); //initialize Systick
    pin_init(); //initialize buttons

    ST7735_InitR(INITR_REDTAB); //initialize LCD
    ST7735_FillScreen(0x66F6); //FILL LCD screen

    Date RTC_data; //struct for rtc data returned
    Date RTC_address; //struct to hold addresses of each element of RTC
    Date EEPROM_data; //struct for EEPROM data read
    uint8_t bank_num = 0; //bank number for printing

    //set the RTC element addresses
    RTC_address.seconds = 0x00;
    RTC_address.minutes = 0x01;
    RTC_address.hours = 0x02;
    RTC_address.day = 0x03;
    RTC_address.date = 0x04;
    RTC_address.month = 0x05;
    RTC_address.year = 0x06;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    //enable interrupts
    NVIC_EnableIRQ(PORT3_IRQn);
    __enable_irq();

    while(1){

        if(print_flag){ //if button to print was pressed

            I2C_Read(SLAVE_ADDRESS_EEPROM, print_track, &EEPROM_data.seconds); //read from eeprom for seconds
            print_track++; //increment the address
            SysTick_delay(10); //delay NEEDED
            I2C_Read(SLAVE_ADDRESS_EEPROM, print_track, &EEPROM_data.minutes); //read from eeprom for minutes
            print_track++; //increment the address
            SysTick_delay(10);
            I2C_Read(SLAVE_ADDRESS_EEPROM, print_track, &EEPROM_data.hours); //read from eeprom for hours
            print_track++; //increment the address
            SysTick_delay(10);
            I2C_Read(SLAVE_ADDRESS_EEPROM, print_track, &EEPROM_data.day); //read from eeprom for day
            print_track++; //increment the address
            SysTick_delay(10);
            I2C_Read(SLAVE_ADDRESS_EEPROM, print_track, &EEPROM_data.date); //read from eeprom for date
            print_track++; //increment the address
            SysTick_delay(10);
            I2C_Read(SLAVE_ADDRESS_EEPROM, print_track, &EEPROM_data.month); //read from eeprom for month
            print_track++; //increment the address
            SysTick_delay(10);
            I2C_Read(SLAVE_ADDRESS_EEPROM, print_track, &EEPROM_data.year); //read from eeprom for year
            SysTick_delay(10);

            //breaking up the seconds
            seconds[1] = ((int) (EEPROM_data.seconds >> 4)) + 48;
            seconds[0] = ((int) (EEPROM_data.seconds & 0x0F)) + 48;

            //breaking up the minutes
            minutes[1] = ((int) (EEPROM_data.minutes >> 4)) + 48;
            minutes[0] = ((int) (EEPROM_data.minutes & 0x0F)) + 48;

            //breaking up the hours
            hours[1] = ((int) (EEPROM_data.hours >> 4)) + 48;
            hours[0] = ((int) (EEPROM_data.hours & 0x0F)) + 48;

            //breaking up the day
            day[1] = ((int) (EEPROM_data.day >> 3)) + 48;
            day[0] = ((int) EEPROM_data.day) + 48;

            //breaking up the date
            date[1] = ((int) (EEPROM_data.date >> 4)) + 48;
            date[0] = ((int) (EEPROM_data.date & 0x0F)) + 48;

            //breaking up the month
            month[1] = ((int) (EEPROM_data.month >> 4)) + 48;
            month[0] = ((int) (EEPROM_data.month & 0x0F)) + 48;

            //breaking up the year
            year[1] = ((int) (EEPROM_data.year >> 4)) + 48;
            year[0] = ((int) (EEPROM_data.year & 0x0F)) + 48;

            //call print function to print onto LCD screen
            Print_info(bank_num);

            bank_num++; //increment bank number
            print_track += 10; //increment address by 10 to start next data reading at 0x16

            //reset bank number once 5 things have been stored
            if(bank_num == 5)
                bank_num = 0;

            //reset the address in the EEPROM once 5 things have been stored
            if(print_track == 80)
                print_track = 0;

            print_flag = 0;

        }

        else if(save_flag){

            I2C_Read(SLAVE_ADDRESS_RTC, RTC_address.seconds, &RTC_data.seconds); //read from RTC for seconds
            I2C_Read(SLAVE_ADDRESS_RTC, RTC_address.minutes, &RTC_data.minutes); //read from RTC for minutes
            I2C_Read(SLAVE_ADDRESS_RTC, RTC_address.hours, &RTC_data.hours); //read from RTC for hours
            I2C_Read(SLAVE_ADDRESS_RTC, RTC_address.day, &RTC_data.day); //read from RTC for day
            I2C_Read(SLAVE_ADDRESS_RTC, RTC_address.date, &RTC_data.date); //read from RTC for date
            I2C_Read(SLAVE_ADDRESS_RTC, RTC_address.month, &RTC_data.month); //read from RTC for month
            I2C_Read(SLAVE_ADDRESS_RTC, RTC_address.year, &RTC_data.year); //read from RTC for year

            SysTick_delay(100);

            I2C_Write(SLAVE_ADDRESS_EEPROM, store_track, RTC_data.seconds); //write to EEPROM with RTC data for seconds
            store_track++; //increment the address
            SysTick_delay(10);
            I2C_Write(SLAVE_ADDRESS_EEPROM, store_track, RTC_data.minutes); //write to EEPROM with RTC data for minutes
            store_track++; //increment the address
            SysTick_delay(10);
            I2C_Write(SLAVE_ADDRESS_EEPROM, store_track, RTC_data.hours); //write to EEPROM with RTC data for hours
            store_track++; //increment the address
            SysTick_delay(10);
            I2C_Write(SLAVE_ADDRESS_EEPROM, store_track, RTC_data.day); //write to EEPROM with RTC data for day
            store_track++; //increment the address
            SysTick_delay(10);
            I2C_Write(SLAVE_ADDRESS_EEPROM, store_track, RTC_data.date); //write to EEPROM with RTC data for date
            store_track++; //increment the address
            SysTick_delay(10);
            I2C_Write(SLAVE_ADDRESS_EEPROM, store_track, RTC_data.month); //write to EEPROM with RTC data for month
            store_track++; //increment the address
            SysTick_delay(10);
            I2C_Write(SLAVE_ADDRESS_EEPROM, store_track, RTC_data.year); //write to EEPROM with RTC data for year

            store_track += 10; //increment address by 10 to start next data reading at 0x16, 0x32, etc

            //reset the address in the EEPROM once 5 things have been stored
            if(store_track == 80)
                store_track = 0;

            save_flag = 0;
        }
    }
}

void pin_init(void){

    P3 -> SEL0 &=~ 0xC0; // Config simple I/O for P3.6 and P3.7
    P3 -> SEL1 &=~ 0xC0;
    P3 -> DIR &=~ 0xC0;  //P3.6 and P3.7 are inputs
    P3 -> REN |= 0xC0;   //Pull up resistor
    P3 -> OUT |= 0xC0;   //Inputs are off (reversed due to the pull up)

    P3 -> IES |= 0xC0;   //Interrupt triggered on falling edge
    P3 -> IE  |= 0xC0;   //Interrupt enabled
    P3 -> IFG &=~0xC0;   //Clear Interrupt flag
}

void Print_info(int bank){



    //print bank number
    ST7735_DrawCharS(20, 23, 'B', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(32, 23, 'A', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(44, 23, 'N', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(56, 23, 'K', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(68, 23, ':', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(80, 23, (bank + 48), 0x0400, 0x66F6, 1);

    //print the day of the week
    ST7735_DrawCharS(20, 43, 'D', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(32, 43, 'A', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(44, 43, 'Y', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(56, 43, ':', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(68, 43, day[1], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(80, 43, day[0], 0x0400, 0x66F6, 1);

    //print the month, date, year
    ST7735_DrawCharS(10, 63, month[1], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(20, 63, month[0], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(32, 63, '/', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(44, 63, date[1], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(56, 63, date[0], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(68, 63, '/', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(80, 63, year[1], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(92, 63, year[0], 0x0400, 0x66F6, 1);

    //print the time
    ST7735_DrawCharS(20, 83, hours[1], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(32, 83, hours[0], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(44, 83, ':', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(56, 83, minutes[1], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(68, 83, minutes[0], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(80, 83, ':', 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(92, 83, seconds[1], 0x0400, 0x66F6, 1);
    ST7735_DrawCharS(104, 83, seconds[0], 0x0400, 0x66F6, 1);

}

void PORT3_IRQHandler(void)
{
    if((P3 -> IFG & BIT6)){

        save_flag = 1;

    }

    else if((P3 -> IFG & BIT7)){

        print_flag = 1;

    }

    P3-> IFG &=~ BIT6; //clear IFG for P1.6
    P3-> IFG &=~ BIT7; //clear IFG for P1.7
}

void I2C_init(void){

    EUSCI_B0->CTLW0 |= 1;

    EUSCI_B0->CTLW0 = 0x0F81;
    EUSCI_B0->BRW = 30;
    P1->SEL0 |= 0xC0;
    P1->SEL1 &=~ 0xC0;
    EUSCI_B0->CTLW0 &=~ 1;

}

int I2C_Write(int slaveAddr, unsigned char memAddr, unsigned char data){

    I2C_init();

    EUSCI_B0->I2CSA = slaveAddr;
    EUSCI_B0->CTLW0 |= 0x0010;
    EUSCI_B0->CTLW0 |= 0x0002;
    while(!(EUSCI_B0->IFG & 2));
    EUSCI_B0->TXBUF = memAddr;
    while(!(EUSCI_B0->IFG & 2));
    EUSCI_B0->TXBUF = data;
    while(!(EUSCI_B0->IFG & 2));
    EUSCI_B0->CTLW0 |= 0x0004;
    while(EUSCI_B0->CTLW0 & 4);
    return 0;
}


int I2C_Read(int slaveAddr, unsigned char memAddr, unsigned char* data){

    I2C_init();

    EUSCI_B0->I2CSA = slaveAddr;
    EUSCI_B0 -> CTLW0 |= 0x0010;
    EUSCI_B0 -> CTLW0 |= 0x0002;

    while((EUSCI_B0->CTLW0 & 2));
    EUSCI_B0->TXBUF = memAddr;
    while(!(EUSCI_B0->IFG & 2));
    EUSCI_B0->CTLW0 &= ~0x0010;
    EUSCI_B0->CTLW0 |= 0x0002;
    while(EUSCI_B0->CTLW0 & 2);
    EUSCI_B0->CTLW0 |= 0x0004;

    while(!(EUSCI_B0->IFG & 1));
    *data = EUSCI_B0->RXBUF;
    while(EUSCI_B0->CTLW0 & 4);
    return 0;

}

void SysTick_init(void)
{
    SysTick -> CTRL = 0;
    SysTick -> LOAD = 0x00FFFFFF;
    SysTick -> VAL = 0;
    SysTick -> CTRL = 5;

}

void SysTick_delay(uint16_t delay_ms)
{
    SysTick -> LOAD = ((delay_ms*3000) - 1);
    SysTick -> VAL = 0;
    while((SysTick -> CTRL & 0x00010000) == 0);
}