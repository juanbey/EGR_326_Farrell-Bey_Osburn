#include "msp.h"
#include "I2C.h"


void I2C_init(void){
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;     //Enable Reset State (0x0001) P3.5/6

    EUSCI_B0->CTLW0 |= (EUSCI_B_CTLW0_SWRST     //Enable Reset State
                    | EUSCI_B_CTLW0_SSEL__SMCLK // SM clk
                    | EUSCI_B_CTLW0_SYNC        // synch mode
                    | EUSCI_B_CTLW0_MODE_3      //I2C mode
                    | EUSCI_B_CTLW0_MST);       // master mode

    EUSCI_B0->BRW = 30; //Prescalar

    P1->SEL0 |= (SCL | SDA);                    // setup pins
    P1->SEL1 &= ~(SCL | SDA);

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    //Enable Reset State (0x0001) P3.5/6
}


int I2C_write(int slaveAddr, unsigned char memAddr, unsigned char data){
    SysTick_Init();

    EUSCI_B0->I2CSA = slaveAddr;            // setup slave address
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;    // Enable transmitter
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // generate START and send slave address

    while(!(EUSCI_B0->IFG & 2));          // Wait until start is done

    EUSCI_B0->TXBUF = memAddr;              // Send memory address to slave

    while(!(EUSCI_B0->IFG & 2));            // Wait until data is ready to trasmit

    EUSCI_B0->TXBUF = data;                 // Send data to slave
    ms_delay(20);

    while(!(EUSCI_B0->IFG & 2));            // Wait until last transmit is done

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP; // Send STOP

    while(EUSCI_B0->CTLW0 & 4);         // Wait until last transmit is done
    return 0;

}


void I2C_read(int slaveAddr, unsigned char memAddr, unsigned char* data){

    EUSCI_B0->I2CSA = slaveAddr;                // setup slave address
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;        // Enable transmitter
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;     // generate START and send slave address

    while(EUSCI_B0->CTLW0 & 2);         // Wait till start is done

    EUSCI_B0->TXBUF = memAddr;              //Send memory address to slave

    while(!(EUSCI_B0->IFG & 2));            //Wait till 'it' is ready to trasmit

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Enable reciver
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // Generate RESTART and send slave address

    while(EUSCI_B0->CTLW0 & 2);         // Wait till 'restart' is ready to trasmit

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP; // Send STOP

    while(!(EUSCI_B0->IFG & 1));            // Wait till last recieve is done

    *data = EUSCI_B0->RXBUF;

    while(EUSCI_B0->CTLW0 & 4);         // Wait until STOP is sent
}

void I2C_print(unsigned char *data){
    printf("%x : %x : %x\tday %x\tdate: %x/%x/%x\n",    // print off date/time from RTC
                   data[2] & 0x3F,      // hr
                   data[1],             // min
                   data[0],             // sec
                   data[3],             // day
                   data[5],             // month
                   data[4],             // date
                   data[6]);           // year

}

//int I2C_burstwrite(int slaveAddr, unsigned char* memAddr, unsigned char* data){
//    int size = sizeof(data);
//    int i = 0;
//
//
//    for(i = 0;i < (size - 1);++i){
//            I2C_write(RTC_SLAVE, memAddr[i], data[i]);         // set seconds
//    }
//    return 0;
//}
//
//int I2C_burstread(int slaveAddr, unsigned char memAddr, unsigned char* data);

