/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *            |                 |
 *       RST -|     P3.3/UCA0TXD|----> M5StickC
 *            |                 |
 *            |     P3.2/UCA0RXD|<---- M5StickC
 *            |                 |
 *
 *******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

void uPrintfM5(unsigned char * TxArray);
void uPrintfConsole(char * TxArray);
void EUSCIA0_IRQHandler(void);
void EUSCIA2_IRQHandler(void);

volatile int j = 0;
char buffer [50];

volatile float currentTime = 0;

//![Simple UART Config]
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 460800 baud rate. These
 * values were calculated using the online calculator that TI provides at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_Config uartM5Config =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
        6,                                              // BRDIV = 6
        0,                                              // UCxBRF = 0
        170,                                            // UCxBRS = 170
        EUSCI_A_UART_NO_PARITY,                         // NO Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION, // No Oversampling
};
//![Simple UART Config]
// 9600 baud rate
const eUSCI_UART_Config uartConsoleConfig =
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
// 1 sec timer
const Timer_A_UpModeConfig upTimer =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_64,
        46875,
        TIMER_A_TAIE_INTERRUPT_DISABLE,
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,
        TIMER_A_DO_CLEAR
};

int main(void)
{
    /* Halting WDT  */
    WDT_A_holdTimer();

    /* Selecting P2.2 and P2.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A2_BASE, &uartM5Config);
    UART_initModule(EUSCI_A0_BASE, &uartConsoleConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);
    UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts (Rx) */
    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA2);
    Interrupt_enableInterrupt(INT_TA1_0);
    Interrupt_enableMaster();

    Timer_A_configureUpMode(TIMER_A1_BASE, &upTimer);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    uPrintfConsole("Start Transmission \n\r");
	
	// Send 1000 UART messages to M5StickCPlus
    for(j = 0; j < 1000; j++){
        uPrintfM5("a");
    }
    while (1)
    {
        PCM_gotoLPM3InterruptSafe();
    }
}

void delay(int count){
    volatile int k = 0;
    for(k = 0; k < count; k++){
       }
}

void uPrintfM5(unsigned char * TxArray)
{
    unsigned short i = 0;
    while(*(TxArray+i))
    {
        UART_transmitData(EUSCI_A2_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                             // Increment pointer to point to the next character
    }
}

void uPrintfConsole(char * TxArray)
{
    unsigned short i = 0;
    while(*(TxArray+i))
    {
        UART_transmitData(EUSCI_A0_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                             // Increment pointer to point to the next character
    }
}

/* EUSCI A2 UART ISR */
// Handles acknowledgement message from M5StickCPlus
void EUSCIA2_IRQHandler(void)
{
    char a = UART_receiveData(EUSCI_A2_BASE);
	// Get remaining time in register
    float time = (float)Timer_A_getCounterValue(TIMER_A1_BASE);
	// Stop timer
    Timer_A_stopTimer(TIMER_A1_BASE);
	// Calculate remaining time in register
    time = time / 46875;
	// Add remaining time into total time
    currentTime += time;
	// Convert seconds to milliseconds
    currentTime *= 1000;
	// Print time to buffer
    sprintf (buffer, "%2.4f", currentTime);
    uPrintfConsole(">> Acknowledgement received at ");
    uPrintfConsole(buffer);
    uPrintfConsole(" ms");
    uPrintfConsole("\n\r");
	// Calculate average latency per message
    float latency = currentTime / 1000;
    sprintf (buffer, "%2.4f", latency);
    uPrintfConsole("Latency per message: ");
    uPrintfConsole(buffer);
    uPrintfConsole(" ms");
    uPrintfConsole("\n\r");
}

// Every 1 second interrupt
void TA1_0_IRQHandler(void){
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    currentTime += 1;
    sprintf (buffer, "%04.1f", currentTime);
    uPrintfConsole(buffer);
    uPrintfConsole(" secs");
    uPrintfConsole("\n\r");
}
