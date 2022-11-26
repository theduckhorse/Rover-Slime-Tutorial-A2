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
int debug;
int distbased;
//int distancetogo;
int distinnotch;
int rightstatus;
int leftstatus;
int pidsecL;
int pidsecR;
int wRight;
int wLeft;
int lockout;
double errorLeft;
double integralLeft;
double derivativeLeft;
double lasterrorLeft;
double outputLeft;

int LR;
int pwm;
int pwm2;
double percentage;
double percentage2;
double errorRight;
double integralRight;
double derivativeRight;
double lasterrorRight;
double outputRight;

double setPoint;
double kp = 0.5;
double ki = 0.1;
double kd = 0.1;

int counter=0;

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
     Interrupt_enableInterrupt(INT_T32_INT1);
     Interrupt_enableInterrupt(INT_T32_INT2);
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
       PErrorInit();
       //leftstatus = 1;
       //rightstatus = 1;
       //moveForward(9,300);
       lockout = 0;
       LR = 0;
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
void driveMotor(int pwm, int pwm2)
{
    pwmConfig.dutyCycle = pwmConfig.dutyCycle + pwm;
    pwmConfig2.dutyCycle = pwmConfig2.dutyCycle + pwm2;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
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
    turning = 1;
    pwmConfig.dutyCycle = 5400;
    pwmConfig2.dutyCycle = 5400;
    notchcounter = 8;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
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
    turning = 1;
    pwmConfig.dutyCycle = 5400;
    pwmConfig2.dutyCycle = 5400;
    notchcounter = 4;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
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
    turning = 1;
    pwmConfig.dutyCycle = 5400;
    pwmConfig2.dutyCycle = 5400;
    notchcounter = 8;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
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
    turning = 1;
    pwmConfig.dutyCycle = 5400;
    pwmConfig2.dutyCycle = 5400;
    notchcounter = 4;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
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
#define TICKPERIOD 1000
void PErrorInit(void)
{
    const Timer_A_UpModeConfig pidUpConfig =
        {
                TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
                TIMER_A_CLOCKSOURCE_DIVIDER_3,          // SMCLK/3 = 1MHz
                TICKPERIOD,                             // 1000 tick period
                TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
                TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
                TIMER_A_DO_CLEAR                        // Clear value
        };
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1,TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1,TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE, 128000);
    Timer32_setCount(TIMER32_1_BASE, 128000);
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Timer32_enableInterrupt(TIMER32_1_BASE);
}/*
void PErrorStart(void)
{
    Timer_A_clearTimer(TIMER_A1_BASE);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

}
static uint32_t PErrorEnd(void)
{

    uint32_t pulsetime=0;

        /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)
    pulsetime = pidsec * TICKPERIOD;
        /* Number of ticks (between 1 to 999) before the interrupt could occur
    pulsetime += Timer_A_getCounterValue(TIMER_A1_BASE);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_STOP_MODE);
    Timer_A_clearTimer(TIMER_A1_BASE);
    pidsec = 0;

    return pulsetime;
}*/
void T32_0_IRQHandler(void)
{
    /* Increment global variable (count number of interrupt occurred) */
    pidsecL++;
   //uPrintf("1 sec\n\r");
    /* Clear interrupt flag */
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
}
void T32_1_IRQHandler(void)
{
    /* Increment global variable (count number of interrupt occurred) */
    pidsecR++;

    /* Clear interrupt flag */
    Timer32_clearInterruptFlag(TIMER32_1_BASE);
}
void PErrorStartL(void)
{
    Timer32_setCount(TIMER32_0_BASE, 128000);
    Timer32_startTimer(TIMER32_0_BASE, false);

}
static int PErrorEndL(void)
{

    int pulsetime=0;

        /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime = pidsecL * 128000;

        /* Number of ticks (between 1 to 999) before the interrupt could occur */
    pulsetime += (128000-Timer32_getValue(TIMER32_0_BASE));
        Timer32_haltTimer(TIMER32_0_BASE);
        Timer32_setCount(TIMER32_0_BASE, 128000);
    pidsecL = 0;
    return pulsetime;
}
void PErrorStartR(void)
{
    Timer32_setCount(TIMER32_1_BASE, 128000);
    Timer32_startTimer(TIMER32_1_BASE, false);
}
static int PErrorEndR(void)
{

    int pulsetime=0;

        /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime = pidsecR * 128000;
        /* Number of ticks (between 1 to 999) before the interrupt could occur */
    pulsetime += (128000-Timer32_getValue(TIMER32_1_BASE));
    Timer32_haltTimer(TIMER32_1_BASE);
    Timer32_setCount(TIMER32_1_BASE, 128000);
    pidsecR = 0;
    return pulsetime;
}

void PORT1_IRQHandler(void)//unused
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    leftstatus = 1;
    rightstatus = 1;
    //moveForward(9,300);
    notchTurnRight();
    /*
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
*/
}
#define RPM(tick) ((1/((tick/1000000.0)*20))*60)
#define TCONVD(tick) ((2700/tick)-10);
void PORT2_IRQHandler(void)
{
    uint32_t status;
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P2);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, status);
    counter++;
/*
    if (counter == 40) {

        // Set the target PWM/NPM (Notches Per Minute) to hit (WIP need to determine the correct value)
            //setPoint = pwmConfig.dutyCycle * 0.75; //throttle by 25% ???
            setPoint = 31100/1000.0; // 75%

            errorLeft = setPoint - wLeft/1000.0;
            integralLeft = integralLeft + errorLeft;
            derivativeLeft = errorLeft - lasterrorLeft;

            //PID Right Motor
            //Calculate Error (Proportional)
            errorRight = setPoint - wRight/1000.0;
            //Calculate Integral
            integralRight = integralRight + errorRight;
            //Calculate Derivative
            derivativeRight = errorRight - lasterrorRight;



            sprintf(str,"wRight: %d\n\r", wRight);
                        uPrintf(str);
            //PID Left Motor
            //Calculate Error (Proportional)
            sprintf(str,"wleft: %d\n\r", wLeft);
            uPrintf(str);
            //Calculate Integral
            //Calculate Derivative
            //Calculate PID Output
            outputLeft = (kp * errorLeft) + (ki * integralLeft) + (kd * derivativeLeft);
            sprintf(str,"left output: %lf\n", outputLeft);
            uPrintf(str);

            //Limit Output (WIP, need to change)
            // if (outputLeft > 10000)
            //     outputLeft = 10000;
            // if (outputLeft < 0.0)
            //     outputLeft = 0.0;
            //Save Error to Last Error
            lasterrorLeft = errorLeft;

            //If output is positive, increase pwm. otherwise decrease pwm (WIP need to check if correct way)
            // if (outputLeft > 0)
            //     pwmConfig.dutyCycle += outputLeft;
            // else if (outputLeft < 0)
            //     pwmConfig.dutyCycle -= outputLeft;
            percentage = TCONVD(( wLeft/1000.0 + outputLeft));
            sprintf(str,"percentage: %lf\n\r", percentage);
            uPrintf(str);
            if (percentage > 100.0) {
                percentage = 100.0;
            }
            sprintf(str,"outputleft: %lf\n\r", outputLeft);
            uPrintf(str);

            pwm =  floor(((percentage / 100.0) * 6000.0));
            sprintf(str,"opl: %lf\n\r", outputLeft);
            uPrintf(str);
            sprintf(str,"pwm: %d\n\r", pwm);
            uPrintf(str);
            //pwmConfig.dutyCycle = pwm;
            //Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);


            //Calculate PID Output
            outputRight = (kp * errorRight) + (ki * integralRight) + (kd * derivativeRight);
            //Limit Output (WIP, need to change)
            // if (outputRight > 115.0)
            //     outputRight = 115.0;
            // if (outputRight < 0.0)
            //     outputRight = 0.0;
            //Save Error to Last Error
            lasterrorRight = errorRight;
            //If output is positive, increase pwm. otherwise decrease pwm (WIP need to check if correct way)
            // if (outputRight > 0)
            //     pwmConfig.dutyCycle += outputRight;
            // else if (outputRight < 0)
            //     pwmConfig.dutyCycle -= outputRight;
            percentage2 = TCONVD(( wRight/1000.0 + outputRight));
            if (percentage > 100.0) {
                percentage = 100.0;
            }

            sprintf(str,"right percentage: %lf\n\r", percentage2);
            uPrintf(str);
            pwm2 =  floor((percentage2 / 100.0) * 6000.0);
           // pwmConfig2.dutyCycle = pwm2;
            sprintf(str," Right PWM: %d\n\r", pwm2);
            uPrintf(str);
            counter = 0;
            //Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    }

*/
    // Get interrupt status
    if (status & GPIO_PIN6)
        {
            leftcounter++;

            if(leftstatus == 1)
            {
                PErrorStartL();
                leftstatus = 0;
            }
            else if(leftstatus == 0)
            {
                //int check = PErrorEndL();
                /*if (check >= 25000)
                {
                    wLeft = check;
                }
                else
                {
                    wLeft = 25000;
                }*/
                wLeft = PErrorEndL();
                sprintf(str, "pulsewidthleft: %d\n\r", wLeft);
                uPrintf(str);
                leftstatus = 1;
            }
        }

        // increment right wheel counter
        if (status & GPIO_PIN7)
        {
            rightcounter++;

            if(rightstatus == 1)
            {
                 PErrorStartR();
                 rightstatus = 0;
            }
            else if(rightstatus == 0)
            {
                /*
            int check = PErrorEndR();
            if (check >= 25000)
            {
            wRight = check;
            }
            else
            {
            wRight = 25000;
            }


             */
                wRight = PErrorEndR();
            sprintf(str, "pulsewidthright: %d\n\r", wRight);

            uPrintf(str);
            rightstatus = 1;
            }
        }
        if(leftcounter > notchcounter && rightcounter > notchcounter && turning == 1)
        {
                turning = 0;
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
                disableWEncoderInterrupt();
                leftcounter = 0;
                rightcounter = 0;

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
            leftcounter = 0;
            rightcounter = 0;
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
/*void TA1_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_STOP_MODE);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    if(debug == 1){
        debug = 0;
        leftcounter = leftcounter * 12;
        rightcounter = rightcounter * 12;
        sprintf(str, "Left Wheel NPM: %d\n\r", leftcounter);
              printf(str);
              sprintf(str, "Right Wheel NPM: %d\n\r", rightcounter);
              printf(str);
    }
}*/
void uPrintf(unsigned char * TxArray)
{
    unsigned short i = 0;
    while(*(TxArray+i))
    {
        UART_transmitData(EUSCI_A0_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                             // Increment pointer to point to the next character
    }
}
