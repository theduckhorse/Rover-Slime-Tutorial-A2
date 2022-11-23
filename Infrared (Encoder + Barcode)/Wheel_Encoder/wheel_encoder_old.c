/**
 * wheel_encoder.c
 *
 * Authors: Goh Yee Kit
 *          Zhang XiangHui
 *          Cham Xun Thong
 *
 * Interrupts for wheel encoders
 * Pin 2.6 > Left Wheel Encoder Pin 2.7 > Right Wheel Encoder
 *
 * -GPIO PORT2 ISR-
 * ISR of left and right wheel encoder to increase counter
 *
 * -TA1_0 ISR-
 * Timer ISR to calculate the RPM of the left and right wheel every 1 second
 *
 *                 MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P2.7  |<--- Right Encoder
 *            |                  |
 *            |            P2.6  |<--Left Encoder
 *            |                  |
 *            |                  |
 *             ------------------
 *
 */

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

/* Application Defines  */
#define TIMER_PERIOD 1024 // timer period to get 1 sec timer
#define TIMEVAL 60        // used to calculate RPM
#define WHEELCIRCUMFERENCE 20.4
#define WHEELDIAMETER 0.65
#define NOTCHLENGTH 1.02
#define PI 3.142

/* Global Variables */
volatile uint32_t leftCounter;
volatile uint32_t rightCounter;
volatile uint32_t leftRPM;
volatile uint32_t rightRPM;
volatile float curDist;
volatile float totalDist;
volatile char str[80];

/* Declare Functions */
void uPrintf(unsigned char *TxArray);

/* Timer_A UpMode Configuration Parameter for 1 second timer */
const Timer_A_UpModeConfig upConfig =
    {
        TIMER_A_CLOCKSOURCE_ACLK,           // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_32,     // ACLK/32 = 1024Hz
        TIMER_PERIOD,                       // 1024 tick period for 1 sec
        TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                    // Clear value
};

// UART Config 115200 baud rate
const eUSCI_UART_Config uartConfig =
    {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
        1,                                             // BRDIV = 1
        10,                                            // UCxBRF = 10
        0,                                             // UCxBRS = 0
        EUSCI_A_UART_ODD_PARITY,                       // ODD Parity
        EUSCI_A_UART_LSB_FIRST,                        // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
        EUSCI_A_UART_MODE,                             // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
};

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // stop watchdog timer

    // init variables
    leftCounter = 0;
    rightCounter = 0;
    leftRPM = 0;
    rightRPM = 0;

    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    // Configure P2.6, P2.7 as input with pull-up resistor
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN6 | GPIO_PIN7);

    // Clear Interrupt flag for P2.6, P2.7
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN6 | GPIO_PIN7);
    // Enable Interrupt for P2.6, P2.7
    MAP_GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6 | GPIO_PIN7);
    // Enable Interrupt for port 2
    MAP_Interrupt_enableInterrupt(INT_PORT2);

    /* Configuring Timer_A1 for Up Mode */
    MAP_Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

    /* Enabling interrupts and starting the timer */
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableInterrupt(INT_TA1_0);
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    // Enable MASTER interrupts
    MAP_Interrupt_enableMaster();

    // Loop to go to LPM3
    while (1)
    {
        MAP_PCM_gotoLPM3();
    }
}

void uPrintf(unsigned char *TxArray)
{
    unsigned int i = 0;
    while (*(TxArray + i))
    {
        UART_transmitData(EUSCI_A0_BASE, *(TxArray + i)); // Write the character at the location specified by pointer
        i++;                                              // Increment pointer to point to the next character
    }
}

/* GPIO PORT2 ISR */
void PORT2_IRQHandler(void)
{
    uint32_t status;

    // Get interrupt status
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P2);

    // increment left wheel counter
    if (status & GPIO_PIN6)
    {
        leftCounter++;
    }

    // increment right wheel counter
    if (status & GPIO_PIN7)
    {
        rightCounter++;
    }

    // clear interrupt flag
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, status);
}

/* TA1_0 ISR */
void TA1_0_IRQHandler(void)
{
    // rpm = (freq x TIMEVAL (to get 1 minute depending on timer used))/20 (number of notches)
    curDist = ((leftCounter + rightCounter) / 2 * NOTCHLENGTH) / 100;
    totalDist += curDist;
    if (leftCounter > 0)
    {
        leftRPM = (leftCounter * TIMEVAL) / 20; // calculate left wheel rpm
        // leftspeed = ((leftRPM * WHEELDIAMETER * PI * 60) / 63360) / 2.237; // calculate left wheel speed in m/s from rpm
        leftCounter = 0;                        // reset left counter
    }
    else
    {
        leftRPM = 0; // if leftCounter = 0, leftRPM = 0, no calculation of rpm if car not moving
    }

    if (rightCounter > 0)
    {
        rightRPM = (rightCounter * TIMEVAL) / 20; // calculate right wheel rpm
        // rightspeed = ((rightRPM * WHEELDIAMETER * PI * 60) / 63360) / 2.237; // calculate left wheel speed in m/s from rpm
        rightCounter = 0;                         // reset right counter
    }
    else
    {
        rightRPM = 0; // if rightCounter = 0, rightRPM = 0, no calculation of rpm if car not moving
    }

    // avgspeed = ((((leftRPM + rightRPM) / 2) * WHEELDIAMETER * PI * 60) / 63360) / 2.237;
    sprintf(str, "Left Wheel RPM: %d\n\r", leftRPM);
    uPrintf(str);
    sprintf(str, "Right Wheel RPM: %d\n\r", rightRPM);
    uPrintf(str);
    sprintf(str, "Avg Speed: %f m/s\n\r", curDist);
    uPrintf(str);
    sprintf(str, "Total Distance: %f m\n\r", totalDist);
    uPrintf(str);
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
                                             TIMER_A_CAPTURECOMPARE_REGISTER_0);
}
