/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_21_00_05 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
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
/*******************************************************************************
 * MSP432 GPIO - Toggle Output High/Low
 *
 * Description: In this very simple example, the LED on P1.0 is configured as
 * an output using DriverLib's GPIO APIs. An infinite loop is then started
 * which will continuously toggle the GPIO and effectively blink the LED.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.0  |---> P1.0 LED
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 * Author: Timothy Logan
 ******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
int mode;
int turning;
int notchcounter;
int leftcounter;
int rightcounter;

int errorLeft;
double integralLeft;
double derivativeLeft;
double lasterrorLeft;
double outputLeft;

int errorRight;
double integralRight;
double derivativeRight;
double lasterrorRight;
double outputRight;

int setPoint;
double kp = 0.5;
double ki = 0.0001;
double kd = 0.1;

int debug;
int distbased;
//int distancetogo;
int distinnotch;
#define wheelcirc 20.4;
volatile char str[80];
//void turnRight();
const eUSCI_UART_Config uartConfig =//settings have been configured to a baudrate of 115200 using the above calculator
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
        1,                                             // BRDIV = 1
        10,                                              // UCxBRF = 10
        0,                                              // UCxBRS = 0
        EUSCI_A_UART_ODD_PARITY,                        // ODD Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};
Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_10,
        6000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        6000

};
Timer_A_PWMConfig pwmConfig2 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_10,
        6000,
        TIMER_A_CAPTURECOMPARE_REGISTER_2,
        TIMER_A_OUTPUTMODE_RESET_SET,
        6000

};
#define pwmrate 600

#define turningPeriod 95000
#define TIMER_PERIOD    0x2DC6
#define TIMER_PERIOD2 5120

/* Timer_A UpMode Configuration Parameter */
Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_64,          // SMCLK/1 = 3MHz
        50000,                           // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};
Timer_A_UpModeConfig up2Config =
{
         TIMER_A_CLOCKSOURCE_ACLK,           // ACLK Clock Source
         TIMER_A_CLOCKSOURCE_DIVIDER_32,     // ACLK/32 = 1024Hz
         TIMER_PERIOD2,                       // 1024 tick period for 1 sec
         TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Timer interrupt
         TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, // Enable CCR0 interrupt
         TIMER_A_DO_CLEAR                    // Clear value
};

int main(void)
{

    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    /* Configuring P1.0 as output */
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
     /* Configuring GPIO5.6 as the PWM output
      * And GPIO 1.4 as Input button and interrupt is enabled
      * There is a jumper cable to 1.0 for the LED */
     GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
     GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
     GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
     GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
     GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
     GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
     GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
     GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
     mode = 0;
     /* Configuring Timer_A */
    // pwmConfig2.dutyCycle = 1800;
    // pwmConfig.dutyCycle = 1800;
    // Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
   //  Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);

     Timer_A_configureUpMode(TIMER_A1_BASE, &up2Config);
     /* Enabling interrupts, starting the watchdog timer */
     Interrupt_enableInterrupt(INT_PORT1);
     Interrupt_enableInterrupt(INT_TA1_0);
    // Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
     Interrupt_enableSleepOnIsrExit();
     Interrupt_enableMaster();
    // enableWEncoderInterrupt();
     //GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
      //   GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
       //  GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
       //  GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////testing
     GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

       /* Configuring UART Module */
       UART_initModule(EUSCI_A0_BASE, &uartConfig);
       debug = 1;
       /* Enable UART module */
       UART_enableModule(EUSCI_A0_BASE);

       /* Enabling interrupts (Rx) */
       UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
       Interrupt_enableInterrupt(INT_EUSCIA0);
       uPrintf("Going into LPM3\n\r");
       Interrupt_enableInterrupt(INT_PORT2);

       moveForward(8,300);

     //notchTurnHalfLeft();
     /* Sleeping when not in use */
     while (1)
     {
         PCM_gotoLPM0();
     }
}

void moveForward(int speed, int dist)
{
    if(speed < 10 && speed > 6)
    {
        if(speed == 8)
        {
            pwmConfig.dutyCycle = 3000;
            pwmConfig2.dutyCycle = 3000;//50%
        }
        else if(speed == 9){
            pwmConfig.dutyCycle = 4500;
            pwmConfig2.dutyCycle = 4500;//75%
        }
        else if(speed == 7)
        {
            pwmConfig.dutyCycle = 2400;
            pwmConfig2.dutyCycle = 2400;//40%
        }
        else{
            pwmConfig.dutyCycle = 4500;
            pwmConfig2.dutyCycle = 4500;//default speed is 90%, 75% duty cycle
        }

    distbased = 1;
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7);

    distinnotch = cmToNotchConv(dist);
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    }

}

/*void turnLeft(void)
{
    upConfig.timerPeriod = turningPeriod;
    pwmConfig.dutyCycle = 1350;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
}
void turnLeftHalf(void)
{
    upConfig.timerPeriod = turningPeriod;
    upConfig.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_32;
    pwmConfig.dutyCycle = 1350;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
}
void turnRight(void)
{
    upConfig.timerPeriod = turningPeriod;
    pwmConfig.dutyCycle = 1350;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
}
void turnRightHalf(void)
{
    upConfig.timerPeriod = turningPeriod;
    upConfig.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_32;
    pwmConfig.dutyCycle = 1350;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
}*/
void notchTurnRight(void)
{
    pwmConfig.dutyCycle = 5400;
    notchcounter = 8;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Interrupt_enableInterrupt(INT_PORT2);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

}
void notchTurnHalfRight(void)
{
    pwmConfig.dutyCycle = 5400;
    notchcounter = 4;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Interrupt_enableInterrupt(INT_PORT2);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

}
void notchTurnLeft(void)
{
    pwmConfig.dutyCycle = 5400;
    notchcounter = 8;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Interrupt_enableInterrupt(INT_PORT2);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
}
void notchTurnHalfLeft(void)
{
    pwmConfig.dutyCycle = 5400;
    notchcounter = 4;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    Interrupt_enableInterrupt(INT_PORT2);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
}

int cmToNotchConv(int dist)
{
    double wheelrot = dist/20.4;
    int notch = ceil(wheelrot * 20);
    return notch;
}
/*
void PORT1_IRQHandler(void)//unused
{
    // Get  the current status of the GPIO
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    // increment
    if (status & GPIO_PIN4)
    {
        GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        if(pwmConfig.dutyCycle == 1500)
            pwmConfig.dutyCycle = 150;
        else
            pwmConfig.dutyCycle += 150;

        Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
        Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    }
    if (status & GPIO_PIN1)
        {
            if(mode == 0)
            {
                turnLeft();
                //GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
                //GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
                //GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
                //GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
                mode = 1;
            }
            else if (mode == 1)
            {
                turnRight();
                mode = 0;
            }
            else if (mode == 2)
            {
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
                GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
                GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
                mode = 3;
            }
            else if (mode == 3)
            {
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

            mode = 0;
            }
        }

}
*/
void PORT2_IRQHandler(void)
{
    uint32_t status;

    // Get interrupt status
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P2);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, status);
    // increment left wheel counter
    if (status & GPIO_PIN6)
    {
        leftcounter++;

    }

    // increment right wheel counter
    if (status & GPIO_PIN7)
    {
        rightcounter++;
    }
    if(leftcounter > notchcounter && rightcounter > notchcounter && turning == 1)
    {
            turning = 0;
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
            disableWEncoderInterrupt();

    }
    if(distbased == 1 && leftcounter > distinnotch && rightcounter > distinnotch)
    {
        distbased = 0;
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
        sprintf(str, "Left Wheel travelled: %d\n\r", leftcounter);
        uPrintf(str);
        sprintf(str, "Right Wheel travelled: %d\n\r", rightcounter);
        uPrintf(str);
        distinnotch = 0;
        disableWEncoderInterrupt();
    }

    // clear interrupt flag

}
void disableWEncoderInterrupt(void){
    GPIO_disableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_disableInterrupt(GPIO_PORT_P2, GPIO_PIN7);
}
void enableWEncoderInterrupt(void){
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7);
}
void TA1_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_STOP_MODE);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);

    // PID
    // left wheel average 100%
    // 3175.2
    // right wheel average 100%
    // 3331.2

    // Set the target PWM/NPM (Notches Per Minute) to hit (WIP need to determine the correct value)
    setPoint = pwmConfig.dutyCycle * 0.75; //throttle by 25% ???
    setPoint = pwmConfig.dutyCycle

    //PID Left Motor
    //Calculate Error (Proportional)
    errorLeft = setPoint - (leftcounter*12);
    //Calculate Integral
    integralLeft = integralLeft + errorLeft;
    //Calculate Derivative
    derivativeLeft = errorLeft - lasterrorRight;
    //Calculate PID Output
    outputLeft = (kp * errorLeft) + (ki * integralLeft) + (kd * derivativeLeft);
    //Limit Output (WIP, need to change)
    if (outputLeft > 6000)
        outputLeft = 6000;
    if (outputLeft < 0)
        outputLeft = 0;
    //Save Error to Last Error
    lasterrorLeft = errorLeft;
    //If output is positive, increase pwm. otherwise decrease pwm (WIP need to check if correct way)
    // if (outputLeft > 0)
    //     pwmConfig.dutyCycle += outputLeft;
    // else if (outputLeft < 0)
    //     pwmConfig.dutyCycle -= outputLeft;
        pwmConfig.dutyCycle = outputLeft;

    //PID Right Motor
    //Calculate Error (Proportional)
    errorRight = setPoint - (rightcounter*12);
    //Calculate Integral
    integralRight = integralRight + errorRight;
    //Calculate Derivative
    derivativeRight = errorRight - lasterrorRight;
    //Calculate PID Output
    outputRight = (kp * errorRight) + (ki * integralRight) + (kd * derivativeRight);
    //Limit Output (WIP, need to change)
    if (outputRight > 6000)
        outputRight = 6000;
    if (outputRight < 0)
        outputRight = 0;
    //Save Error to Last Error
    lasterrorRight = errorRight;
    //If output is positive, increase pwm. otherwise decrease pwm (WIP need to check if correct way)
    // if (outputRight > 0)
    //     pwmConfig.dutyCycle += outputRight;
    // else if (outputRight < 0)
    //     pwmConfig.dutyCycle -= outputRight;
    pwmConfig.dutyCycle = outputRight;

    if(debug == 1){
        debug = 0;
        leftcounter = leftcounter * 12;
        rightcounter = rightcounter * 12;
        sprintf(str, "Left Wheel NPM: %d\n\r", leftcounter);
              uPrintf(str);
              sprintf(str, "Right Wheel NPM: %d\n\r", rightcounter);
              uPrintf(str);
    }
}

void uPrintf(unsigned char * TxArray)
{
    unsigned short i = 0;
    while(*(TxArray+i))
    {
        UART_transmitData(EUSCI_A0_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                             // Increment pointer to point to the next character
    }
}
