
/* DriverLib Includes */
#include <driverlib.h>

/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "barcode_reader.h"
#include "wheel_encoder.h"

void uPrintf(unsigned char * TxArray);
void initUart();

unsigned char barcodeMessage[50];
unsigned char encoderMessage[50];

//![Simple UART Config]
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate. These
 * values were calculated using the online calculator that TI provides at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
        19,                                             // BRDIV = 19
        8,                                              // UCxBRF = 8
        85,                                             // UCxBRS = 85
        EUSCI_A_UART_NO_PARITY,                         // NO Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};
//![Simple UART Config]


void initUart()
{
    /* Selecting P3.2 and P3.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts (Rx) */
    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA2);
}

int main(void)
{
    /* Halting WDT  */
    WDT_A_holdTimer();

    SysTick_enableModule();
    SysTick_setPeriod(12000000);
    SysTick_enableInterrupt();


    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    initUart();
    initBarcodeConfig();
    initWheelEncoderConfig();
    uPrintf("Comms Test");

    Interrupt_enableMaster();

    while(1)
    {
        PCM_gotoLPM3InterruptSafe();
    }
}

void uPrintf(unsigned char * TxArray)
{
    unsigned short i = 0;
    while(*(TxArray+i))
    {
        UART_transmitData(EUSCI_A2_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                             // Increment pointer to point to the next character
    }
}

void SysTick_Handler(void)
{
    MAP_Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    // Do actions here
    sprintf(encoderMessage, "Speed: %.3f m/s, Distance: %.5f m", curSpeed, totalDist);
    if (barcodeLength > 0)
    {
        sprintf(barcodeMessage, "Decoded Barcode: %s", decodedOutput);
        uPrintf(barcodeMessage);
    }
    uPrintf(encoderMessage);
}

/* EUSCI A2 UART ISR */
void EUSCIA2_IRQHandler(void)
{
    unsigned char a = 0;

    a = UART_receiveData(EUSCI_A2_BASE);
}
