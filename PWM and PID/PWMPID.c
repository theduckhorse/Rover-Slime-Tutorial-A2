/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

// Conversion curve from tick to PWM (%)
#define TCONVD(tick) ((4000/tick)-10);


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
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN1);//configure wheel directional pins as output
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);


    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);//set wheel direction pins to low in case

     GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);//set pins to pwm mode
     GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);// pin 5.6 and pin 2.5
     GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
     GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
     GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);
     GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
     GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
     GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);


     //Timer_A_configureUpMode(TIMER_A1_BASE, &up2Config);
     /* Enabling interrupt*/
     Interrupt_enableInterrupt(INT_PORT1);
     Interrupt_enableInterrupt(INT_T32_INT1);
     Interrupt_enableInterrupt(INT_T32_INT2);

     Interrupt_enableSleepOnIsrExit();
     Interrupt_enableMaster();
    

       Interrupt_enableInterrupt(INT_PORT2);
       PErrorInit();//init timers for pid
       lockout = 0;//set lockout
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
        if(speed == 5)//based on the speed mode
        {
            pwmConfig.dutyCycle = 3000;//50% duty cycle
            pwmConfig2.dutyCycle = 3000;
            setPoint = 6700/100.0;//expected values for 50%
        }
        else if(speed == 7){
            pwmConfig.dutyCycle = 4500;//75% duty cycle
            pwmConfig2.dutyCycle = 4500;
            setPoint = 4700/100.0;//expected values for 75%
        }
        else if(speed == 8)
        {
            pwmConfig.dutyCycle = 4800;//80% duty cycle
            pwmConfig2.dutyCycle = 4800;
            setPoint = 4460/100.0;//expected values for 80%
        }
        else{
            pwmConfig.dutyCycle = 4500;
            pwmConfig2.dutyCycle = 4500;//default speed is 90%, 75% duty cycle
        }

    distbased = 1;// turn on dist based movement mode
    enableWEncoderInterrupt();//enable wheel encoder interrupt
    leftstatus = 1;//reset statuses
    rightstatus = 1;
    distinnotch = cmToNotchConv(dist);// get the distance in notches from cm
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);//use configs to generate pwms
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);// start moving forward
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    }

}
void driveMotor(int pwm, int pwm2)
{
    pwmConfig.dutyCycle = pwm;// get the duty cycle
    pwmConfig2.dutyCycle = pwm2;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);//generate pwm
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);//move forward
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
}
void stopMotor(){
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);//stop the wheels
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    pwmConfig.dutyCycle = 0;//0% duty cycle
    pwmConfig2.dutyCycle = 0;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);// generate the pwm with 0% duty cycle
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
}
void notchTurnRight(void)
{
    turning = 1;
    pwmConfig.dutyCycle = 3000;//50% duty cycle
    pwmConfig2.dutyCycle = 3000;
    notchcounter = 8;//8 notches, 90 degrees
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    Interrupt_enableInterrupt(INT_PORT2);
    enableWEncoderInterrupt();
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);//turn right
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

}
void notchTurnHalfRight(void)
{
    turning = 1;// enable turning mode
    pwmConfig.dutyCycle = 3000;//50% duty cycle
    pwmConfig2.dutyCycle = 3000;
    notchcounter = 4;//4 notches, 45 degrees
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    Interrupt_enableInterrupt(INT_PORT2);
    enableWEncoderInterrupt();//8 notches, 90 degrees
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);//turn right
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

}
void notchTurnLeft(void)
{
    turning = 1;// enable turning mode
    pwmConfig.dutyCycle = 3000;//50% duty cycle
    pwmConfig2.dutyCycle = 3000;
    notchcounter = 8;
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    Interrupt_enableInterrupt(INT_PORT2);
    enableWEncoderInterrupt();
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);//turn left
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
}
void notchTurnHalfLeft(void)
{
    turning = 1;// enable turning mode
    pwmConfig.dutyCycle = 3000;//50% duty cycle
    pwmConfig2.dutyCycle = 3000;
    notchcounter = 4;//4 notches, 45 degrees
    Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);
    Interrupt_enableInterrupt(INT_PORT2);
    enableWEncoderInterrupt();
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);//turn left
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
void PErrorInit(void)// prepare the timers for the timing between notches
{

    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);// set to 128KHZ
    MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1,TIMER32_32BIT, TIMER32_PERIODIC_MODE);// initialize the timers
    Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1,TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_0_BASE, 128000);//set the count to 128000 to time 1 second
    Timer32_setCount(TIMER32_1_BASE, 128000);
    Timer32_enableInterrupt(TIMER32_0_BASE);//enable timer interrupts
    Timer32_enableInterrupt(TIMER32_1_BASE);
}
void T32_0_IRQHandler(void)
{

    pidsecL++;//increment when a second has elapsed between two notches
   /* Clear interrupt flag */
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
}
void T32_1_IRQHandler(void)
{
    pidsecR++;//increment when a second has elapsed between two notches
   /* Clear interrupt flag */
    Timer32_clearInterruptFlag(TIMER32_1_BASE);
}
void PErrorStartL(void)
{

    Timer32_setCount(TIMER32_0_BASE, 128000);//reset the timer
    Timer32_startTimer(TIMER32_0_BASE, true);// start the timer

}
static int PErrorEndL(void)
{

    int pulsetime2=0;

        /* Number of times the interrupt occurred (1 interrupt = 128000 ticks)    */
    pulsetime2 = pidsecL * 128000;

        /* Number of ticks (between 1 to 128000) before the interrupt could occur */
    Timer32_haltTimer(TIMER32_0_BASE);
    pulsetime2 = (128000-Timer32_getValue(TIMER32_0_BASE));

    Timer32_setCount(TIMER32_0_BASE, 128000);
    pidsecL = 0;
    return pulsetime2;
}
void PErrorStartR(void)
{
    Timer32_setCount(TIMER32_1_BASE, 128000);//reset timer
    Timer32_startTimer(TIMER32_1_BASE, true);//start timer
}
static int PErrorEndR(void)
{

    int pulsetime=0;

        /* Number of times the interrupt occurred (1 interrupt = 128000 ticks)    */
    pulsetime = pidsecR * 128000;
        /* Number of ticks (between 1 to 128000) before the interrupt could occur */
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
    moveForward(8,300);
}



void PORT2_IRQHandler(void)
{
    uint32_t status;
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P2);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, status);

    //Counter for triggering the pid
    counter++;

    // Trigger only after 40 notch and when not turning
    if (counter >= 40 && turning == 0) {
            // PID

            // PID Left Motor
            // Calculate Error
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

            //Calculate PID Output for left
            outputLeft = (kp * errorLeft) + (ki * integralLeft) + (kd * derivativeLeft);

   
            //Save Left Error to Last Error
            lasterrorLeft = errorLeft;

            //Using conversion curve (tick -> pwm) left
            percentage = TCONVD(( wLeft/1000.0 + outputLeft));

            // Cap the change to 100 < and > 49 percent left
            if (percentage > 100.0) {
                percentage = 100.0;
            }else if (percentage < 49.0) {
                percentage = 49.0;
            }
            
            // Convert to PWM from percentage on the left
            pwm =  floor(((percentage / 100.0) * 6000.0));

            // Send the updated PWM for the left
            pwmConfig.dutyCycle = pwm;
            Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);


            //Calculate PID Output of the right
            outputRight = (kp * errorRight) + (ki * integralRight) + (kd * derivativeRight);

            //Save Error to Last Error on the right
            lasterrorRight = errorRight;
        
            //Using conversion curve (tick -> pwm) on the right
            percentage2 = TCONVD(( wRight/1000.0 + outputRight));

            // Cap the change to 100 < and > 49 percent on the right
            if (percentage2 > 100.0) {
                percentage2 = 100.0;
            }else if (percentage2 < 49.0) {
                percentage2 = 49.0;
            }

            // Convert to PWM frmo percentage on the right
            pwm2 =  floor((percentage2 / 100.0) * 6000.0);
            
            // Send the updated PWM for the right
            pwmConfig2.dutyCycle = pwm2;
            Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig2);

            //reset counter till next PID
            counter = 0;

    }


    // Get interrupt status
    if (status & GPIO_PIN6)
        {
            leftcounter++;

            if(leftstatus == 1 && lockout == 0)// if timer not started and its the left wheel
            {
                lockout =1;// move to the next step for the wheel
                PErrorStartL();//start the timer
                leftstatus = 0;//declare that timer started
            }
            else if(leftstatus == 0 && lockout == 1)// if timer started and its the left wheel
            {

                wLeft = PErrorEndL();//return the time between two notches for the left wheel
                leftstatus = 1;//declare that timer is not started
                lockout = 2;//move on to the next wheel
            }
        }

        // increment right wheel counter
        if (status & GPIO_PIN7)
        {
            rightcounter++;

            if(rightstatus == 1 && lockout == 2)//if timer not started for the right wheel
            {
                lockout = 3;//move to the next step for the wheel
                 PErrorStartR();// start right wheel timer
                 rightstatus = 0;//declare timer started
            }
            else if(rightstatus == 0 && lockout ==3)// if timer started for the right wheel
            {
            wRight = PErrorEndR();//get the time between two notches for the right wheel
            rightstatus = 1;//declare right wheel timer stopped
            lockout = 0;// switch back to the left wheel
            }
        }
        if(leftcounter > notchcounter && rightcounter > notchcounter && turning == 1)//if the wheels have turned the required notches and its in turning mode
        {
                turning = 0;//stop turning mode
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);//stop the wheels
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
                disableWEncoderInterrupt();//disable wheel encoder interrupts
                leftcounter = 0;//reset side counters and counter for PID
                rightcounter = 0;
                counter = 0;
        }
        if(distbased == 1 && leftcounter > distinnotch && rightcounter > distinnotch)//if in distance based movement mode and the wheels have turned the required notches
        {
            distbased = 0;//turn off dist based movement mode
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);//stop the wheels
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
            distinnotch = 0;// reset all the variables
            disableWEncoderInterrupt();// disable interrupt
            leftcounter = 0;
            rightcounter = 0;
            lockout = 0;
            integralLeft = 0;
            derivativeLeft = 0;
            integralRight = 0;
            derivativeRight = 0;
            counter = 0;
        }

}

void disableWEncoderInterrupt(void){
    GPIO_disableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_disableInterrupt(GPIO_PORT_P2, GPIO_PIN7);
}
void enableWEncoderInterrupt(void){
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7);
}


