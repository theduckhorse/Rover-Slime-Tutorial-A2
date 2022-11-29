/********************************************************************************************************************************/
/*  Ultrasonic codes for A2:                                                                                                    */
/*  By: CHEO CHEANG MING                                                                                                        */
/*      AHMAD FAIRUZI BIN KADIR SAHIB                                                                                           */
/*      POH KAI BOON                                                                                                            */
/*                                                                                                                              */
/*                                                                                                                              */
/*  Credits to Lab 4 Code Partial Reference on Ultrasonic Sensor                                                                */
/*  Kalman filter obtained from:                                                                                                */
/*  https://github.com/rizkymille/ultrasonic-hc-sr04-kalman-filter/blob/master/hc-sr04_kalman_filter/hc-sr04_kalman_filter.ino  */
/*                                                                                                                              */
/*                                                                                                                              */
/********************************************************************************************************************************/

#include "driverlib.h"

#define MIN_DISTANCE 15.0f // Minimum distance of the detected ultrasonic sensor

bool start_pulse = 0;
float distance = 0; // To store the converted distance

const Timer_A_ContinuousModeConfig continuousModeConfig =
    {
        TIMER_A_CLOCKSOURCE_SMCLK,      // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_3,  // SMCLK/1 = 3MHz
        TIMER_A_TAIE_INTERRUPT_DISABLE, // Disable Timer ISR
        TIMER_A_DO_CLEAR                // Skup Clear Counter
};

const Timer_A_CompareModeConfig CCR1Config =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_1,        // Use CCR2
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,
        0x000A // 10 Duty Cycle
};

// -------------------------------------------------------------------------------------------------------------------
void Initalise_HCSR04(void)
{

    /* Configuring P1.0 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // LED lights on when lesser than minimum distance

    /* For front ultrasonic sensor */

    /* Configuring P2.5 as Output */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN7);

    /* Configuring Timer_A0 for continuous Mode */
    MAP_Timer_A_initCompare(TIMER_A1_BASE, &CCR1Config);
    MAP_Timer_A_configureContinuousMode(TIMER_A1_BASE, &continuousModeConfig);
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
    MAP_Timer_A_disableInterrupt(TIMER_A1_BASE);

    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN7);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN7);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN7);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN7, GPIO_LOW_TO_HIGH_TRANSITION);
}

int main(void)
{
    uint32_t tval1, tval2;
    uint32_t duration;

    MAP_WDT_A_holdTimer();

    Initalise_HCSR04();

    MAP_Interrupt_enableInterrupt(INT_PORT3);
    MAP_Interrupt_enableMaster();

    printf("Starting now \n");
    while (1)
    {
        selectSensor(GPIO_PORT_P3, GPIO_PIN7, tval1, tval2, duration); // Left ultrasonic sensor
    }
}

void selectSensor(uint32_t GPIO_PORT, uint32_t GPIO_PIN, uint32_t tval1, uint32_t tval2, uint32_t duration)
{
    static const double R = 40;
    static const double H = 1.00;
    static double Q = 10;
    static double P = 0;
    static double U_hat = 0;
    static double K = 0;

    asm("    nop");
    asm("    nop");
    asm("    nop");
    if (start_pulse == 1) // when it is being interrupted
    {
        tval1 = MAP_Timer_A_getCounterValue(TIMER_A1_BASE); // The value when it hits the rising edge

        while (MAP_GPIO_getInputPinValue(GPIO_PORT, GPIO_PIN) == 1)
            ; // While waiting for the falling edge

        tval2 = MAP_Timer_A_getCounterValue(TIMER_A1_BASE); // Get the value of the falling edge

        start_pulse = 0; // Set back the start pulse back to 0

        if (tval1 > tval2)
            tval2 = tval2 + 0xffff; // Makes it count to the limit of the 0XFFFH to restart back to 0

        duration = tval2 - tval1;    // get the duration of the pulse
        distance = duration / 58.0f; // To convert the echo

        /* Code obtained from https://github.com/rizkymille/ultrasonic-hc-sr04-kalman-filter/blob/master/hc-sr04_kalman_filter/hc-sr04_kalman_filter.ino */
        K = P * H / (H * P * H + R);
        U_hat += +K * (distance - H * U_hat);
        P = (1 - K * H) * P + Q; // Kalman filter

        if (distance < MIN_DISTANCE)
        { // Toggle LED if lesser than minimum distance
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
        else
        {
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
    }
}

void PORT3_IRQHandler(void)
{
    uint32_t status; // local variable for interrupt status in the ISR

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3); // This API call gets the interrupt status of port 3, status of only enabled interrupts will be checked
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, status);         // Clear the interrupt flag for Port 3, if not ISR will be called again as CPU assumes interrupt is not handled yet

    if (status)
    {
        start_pulse = 1;
    }
}
