/**************************************************************************************
 * Title: Farrell-Bey_KeJuan_EGR_326_901_Lab2
 * Filename: main.c
 * Author: KeJuan Farrell-Bey & Jonathan Osburn
 * Course: EGR 326-901
 * Date: 9/2/2022
 * Assignment: Lab # 2:
 * Instructor: Dr. Brian Krug
 *
 * This program is toggle an external LED continuosly if button 1 is pressed and held the RGB
 *           LED will cycle and blink every second.
 * Revision: Initial draft
 *****************************  Pins   **************************************************/

#include "msp.h"
#include <keypad.h>
#include <stdio.h>
#include <stdbool.h>
#include "timers.h"
#include "ST7735.h"

#define SLAVE_ADDRESS 0x48

char TXData;
//char TXData[10] = "RGBBGRBGGR";
//int i = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    keypad_irq_init();
    Clock_Init48MHz();
    Output_Init();
    ST7735_FillScreen(ST7735_BLACK); // set screen to black
    ST7735_SetTextColor(ST7735_WHITE);
    Output_Clear();

    // Master init
    P6->SEL0 |= BIT4 | BIT5; // P1.6 and P1.7 as UCB0SDA and UCB0SCL //TODO:change to 6.4 SDA and 6.5 SCL

    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Hold EUSCI_B1 module in reset state
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_MODE_3 | EUSCI_B_CTLW0_MST | EUSCI_B_CTLW0_SYNC;
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2; // Select SMCLK as EUSCI_B1 clock
    EUSCI_B1->BRW = 0x001E;                    // Set BITCLK = BRCLK / (UCBRx+1) = 3 MHz / 30 = 100 kHz
    EUSCI_B1->I2CSA = SLAVE_ADDRESS;
    EUSCI_B1->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;  // Clear SWRST to resume operation
    NVIC->ISER[1] = 0x00000008;               // Port P1 interrupt is enabled in NVIC
    NVIC->ISER[0] = 1 << ((EUSCIB1_IRQn)&31); // EUSCI_B1 interrupt is enabled in NVIC
    __enable_irq();                           // All interrupts are enabled

    // main funct
    while (EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTP)
        ;

    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TR | EUSCI_B_CTLW0_TXSTT;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk; // Sleep on exit
    __sleep();                           // enter LPM0
}

// void EUSCIB1_IRQHandler(void)
// {
//  uint32_t status = EUSCI_B1->IFG;      // Get EUSCI_B1 interrupt flag
//  EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG0; // Clear EUSCI_B1 TX interrupt flag

//  if (status & EUSCI_B_IFG_TXIFG0)
//  { // Check if transmit interrupt occurs
//      if (TXData == 10 + '0')
//      {
//          EUSCI_B1->TXBUF = 'H';
//      }
//      else
//      {
//          EUSCI_B1->TXBUF = TXData; // Load current TXData value to transmit buffer
//      }

//      EUSCI_B1->IE &= ~EUSCI_B_IE_TXIE0;
//  } // Disable EUSCI_B1 TX interrupt
// }
void EUSCIB1_IRQHandler(void)
{
    uint32_t status = EUSCI_B1->IFG;
    EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG0;

    if (status & EUSCI_B_IFG_TXIFG0)
    {

        EUSCI_B1->TXBUF = TXData;   //[i++ % 10];
        EUSCI_B1->IE &= ~EUSCI_B_IE_TXIE0;
    }
}

void PORT4_IRQHandler(void)
{
    int num = 25;
    num = Read_Keypad();
    P4->IFG = 0;

    // if (num != 25)
    // {
    //  EUSCI_B1->IE |= EUSCI_B_IE_TXIE0;
    // }

    if (num == 12)
    {
        ST7735_DrawCharS(10 * 6, 7 * 10, 83, ST7735_WHITE, ST7735_BLACK, 4);
        EUSCI_B1->IE |= EUSCI_B_IE_TXIE0; // Enable EUSCI_A0 TX interrupt
    }
    else if (num == 10)
    {
        ST7735_DrawCharS(10 * 6, 7 * 10, 72, ST7735_WHITE, ST7735_BLACK, 4);
        TXData = num;// + '0';
        EUSCI_B1->IE |= EUSCI_B_IE_TXIE0; // Enable EUSCI_A0 TX interrupt
    }
    else if (num == 11)
    {
        num = 0;
        Output_Clear(); // black out screen
        ST7735_DrawCharS(10 * 6, 7 * 10, (num + 48), ST7735_WHITE, ST7735_BLACK, 4);
        TXData = num;// + '0';
    }
    else
    {
        Output_Clear(); // black out screen
        ST7735_DrawCharS(10 * 6, 7 * 10, (num + 48), ST7735_WHITE, ST7735_BLACK, 4);
        TXData = num; // + '0';
        //EUSCI_B1->IE |= EUSCI_B_IE_TXIE0;
    }
}
