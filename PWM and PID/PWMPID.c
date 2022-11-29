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
double kp = 0.9;
double ki = 0.001;
double kd = 0.6;

int counter=0;

#define wheelcirc 20.4;
volatile char str[80];

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
     
       Interrupt_enableInterrupt(INT_PORT2);
       PErrorInit();
       //leftstatus = 1;
       //rightstatus = 1;
       //moveForward(9,300);
       lockout = 0;
     //notchTurnHalfLeft();
     /* Sleeping when not in use */
     while (1)
     {
         PCM_gotoLPM0();
     }
}

void moveForward(int speed, int dist)
{
    if(speed < 10 && speed > 4)
    {
        if(speed == 5)
        {
            pwmConfig.dutyCycle = 3000;
            pwmConfig2.dutyCycle = 3000;
            setPoint = 6700/100.0;//50%
        }
        else if(speed == 7){
            pwmConfig.dutyCycle = 4500;
            pwmConfig2.dutyCycle = 4500;
            setPoint = 4700/100.0;//75%
        }
        else if(speed == 8)
        {
            pwmConfig.dutyCycle = 4800;
            pwmConfig2.dutyCycle = 4800;
            setPoint = 4460/100.0;//80%
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

void notchTurnRight(void)
{
    turning = 1;
    pwmConfig.dutyCycle = 3000;
    pwmConfig2.dutyCycle = 3000;
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
    pwmConfig.dutyCycle = 3000;
    pwmConfig2.dutyCycle = 3000;
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
    pwmConfig.dutyCycle = 3000;
    pwmConfig2.dutyCycle = 3000;
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
    pwmConfig.dutyCycle = 3000;
    pwmConfig2.dutyCycle = 3000;
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

int cmToNotchConv(int dist)// method to convert from cm to notches
{
    double wheelrot = dist/21.36;//convert cm into number of wheel rotations
    int notch = ceil(wheelrot * 20);//convert from wheels into notches
    return notch;
}
#define TICKPERIOD 1000
void PErrorInit(void)
{

    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1,TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1,TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE, 128000);
    Timer32_setCount(TIMER32_1_BASE, 128000);
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Timer32_enableInterrupt(TIMER32_1_BASE);
}

void T32_0_IRQHandler(void)
{

    pidsecL++;
   
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
}
void T32_1_IRQHandler(void)
{

    pidsecR++;

    /* Clear interrupt flag */
    Timer32_clearInterruptFlag(TIMER32_1_BASE);
}
void PErrorStartL(void)
{
    Timer32_haltTimer(TIMER32_0_BASE);
    Timer32_setCount(TIMER32_0_BASE, 128000);
    Timer32_startTimer(TIMER32_0_BASE, true);

}
static int PErrorEndL(void)
{

    int pulsetime2=0;

        /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime2 = pidsecL * 128000;

        /* Number of ticks (between 1 to 999) before the interrupt could occur */
    Timer32_haltTimer(TIMER32_0_BASE);
    pulsetime2 = (128000-Timer32_getValue(TIMER32_0_BASE));
    //sprintf(str, "left timer: %d\n\r", Timer32_getValue(TIMER32_0_BASE));
                        printf("left timer: %d\n\r", pulsetime2);
        //Timer32_setCount(TIMER32_0_BASE, 128000);
    //pidsecL = 0;
    return pulsetime2;
}
void PErrorStartR(void)
{
    Timer32_setCount(TIMER32_1_BASE, 128000);
    Timer32_startTimer(TIMER32_1_BASE, true);
}
static int PErrorEndR(void)
{

    int pulsetime=0;

        /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    pulsetime = pidsecR * 128000;
        /* Number of ticks (between 1 to 999) before the interrupt could occur */
    pulsetime += (128000-Timer32_getValue(TIMER32_1_BASE));
    //sprintf(str, "right timer: %d\n\r", Timer32_getValue(TIMER32_1_BASE));
                    printf("right timer: %d\n\r", pulsetime);
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
    moveForward(5,10);
}
#define RPM(tick) ((1/((tick/1000000.0)*20))*60)
#define TCONVD(tick) ((4000/tick)-10);
void PORT2_IRQHandler(void)
{
    uint32_t status;
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P2);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, status);

    //Counter for triggering the pid 
    counter++;

    // Trigger only after 40 notch and when not turning
    if (counter >= 40 && turning == 0) {

        // Set the target PWM/NPM (Notches Per Minute) to hit (WIP need to determine the correct value)
            
            // PID Left Motor
            // Calculate error 
            errorLeft = setPoint - wLeft/1000.0;
            //Calculate Integral
            integralLeft = integralLeft + errorLeft;
            //Calculate Derivative
            derivativeLeft = errorLeft - lasterrorLeft;

            //PID Right Motor
            //Calculate Error (Proportional)
            errorRight = setPoint - wRight/1000.0;
            //Calculate Integral
            integralRight = integralRight + errorRight;
            //Calculate Derivative
            derivativeRight = errorRight - lasterrorRight;


            //Calculate PID Output
            outputLeft = (kp * errorLeft) + (ki * integralLeft) + (kd * derivativeLeft);

   
            //Save Left Error to Last Error
            lasterrorLeft = errorLeft;

            //If output is positive, increase pwm. otherwise decrease pwm (WIP need to check if correct way)
            percentage = TCONVD(( wLeft/1000.0 + outputLeft));

            if (percentage > 100.0) {
                percentage = 100.0;
            }else if (percentage < 49.0) {
                percentage = 49.0;
            }

            pwm =  floor(((percentage / 100.0) * 6000.0));

            pwmConfig.dutyCycle = pwm;
            Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);


            //Calculate PID Output
            outputRight = (kp * errorRight) + (ki * integralRight) + (kd * derivativeRight);
            //Limit Output (WIP, need to change)
            //Save Error to Last Error
            lasterrorRight = errorRight;
            //If output is positive, increase pwm. otherwise decrease pwm
            //Using conversion curve (tick -> pwm) 
            percentage2 = TCONVD(( wRight/1000.0 + outputRight));

            if (percentage2 > 100.0) {
                percentage2 = 100.0;
            }else if (percentage2 < 49.0) {
                percentage2 = 49.0;
            }


            pwm2 =  floor((percentage2 / 100.0) * 6000.0);
            pwmConfig2.dutyCycle = pwm2;

            counter = 0;
            Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    }


    // Get interrupt status
    if (status & GPIO_PIN6)
        {
            leftcounter++;

            if(leftstatus == 1 && lockout == 0)
            {
                lockout =1;
                PErrorStartL();
                leftstatus = 0;
            }
            else if(leftstatus == 0 && lockout == 1)
            {
                
                wLeft = PErrorEndL();
                leftstatus = 1;
                lockout = 2;
            }
        }

        // increment right wheel counter
        if (status & GPIO_PIN7)
        {
            rightcounter++;

            if(rightstatus == 1 && lockout == 2)
            {
                lockout = 3;
                 PErrorStartR();
                 rightstatus = 0;
            }
            else if(rightstatus == 0 && lockout ==3)
            {
            wRight = PErrorEndR();
            rightstatus = 1;
            lockout = 0;
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
                counter = 0;
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
            lockout = 0;
            integralLeft = 0;
            derivativeLeft = 0;
            integralRight = 0;
            derivativeRight = 0;
            counter = 0;
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


