//#include "driverlib.h"
//
//#define MIN_DISTANCE 15.0f // Distance of 15cm
//
//
//bool start_pulse=0;
//float dist;
//
//const Timer_A_ContinuousModeConfig continuousModeConfig =
//   {
//            TIMER_A_CLOCKSOURCE_SMCLK,           // SMCLK Clock Source
//            TIMER_A_CLOCKSOURCE_DIVIDER_3,       // SMCLK/1 = 3MHz
//            TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer ISR
//            TIMER_A_DO_CLEAR                   // Skip Clear Counter
//   };
//
//   const Timer_A_CompareModeConfig CCR1Config =
//   {
//           TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR2
//           TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
//           TIMER_A_OUTPUTMODE_SET_RESET,
//           0x000A                                // 10 Duty Cycle
//   };
//
//// -------------------------------------------------------------------------------------------------------------------
//   void Initalise_HCSR04(void)
//   {
//
//             MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION); // To set the timer to trigger echo burst every 10 µs
//
//
//
//             MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // To light up LED when lesser than minimum distance
//
//             /* Configuring Timer_A0 for continuous Mode */
//             MAP_Timer_A_initCompare(TIMER_A1_BASE, &CCR1Config);
//             MAP_Timer_A_configureContinuousMode(TIMER_A1_BASE, &continuousModeConfig); // 60 miliseconds measurement cycle, to easily divide
//             MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
//             MAP_Timer_A_disableInterrupt(TIMER_A1_BASE);
//
//             MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN0);
//             MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN0);
//             MAP_GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN0);
//             MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN0, GPIO_LOW_TO_HIGH_TRANSITION);
//
//   }
//
//
//int main(void)
//{
//       uint32_t tval1, tval2;
//       uint32_t duration;
//
//
//       MAP_WDT_A_holdTimer();
//
//       Initalise_HCSR04();
//
//       MAP_Interrupt_enableInterrupt(INT_PORT3);
//       MAP_Interrupt_enableMaster();
//
//       printf("Starting now \n");
//       while(1){
//           asm("    nop");
//           asm("    nop");
//           asm("    nop");
//           if(start_pulse==1)
//           {
//               tval1 = MAP_Timer_A_getCounterValue(TIMER_A1_BASE);
//
//               while (MAP_GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN0) == 1);
//
//               tval2 = MAP_Timer_A_getCounterValue(TIMER_A1_BASE);
//
//               start_pulse = 0;
//
//               if(tval1 > tval2)
//                   tval2 = tval2+0xffff;
//
//               duration = tval2-tval1;
//               dist= duration/58.0f;
//               printf("%d cm\n", (int)(dist));
//
//               if (dist < MIN_DISTANCE){
//                   MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
//               }
//               else
//               {
//                   MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
//               }
//
//           }
//
//       }
//
//}
//
//
//void PORT3_IRQHandler(void)
//{
//    uint32_t status;
//
//    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
//    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
//
//    if(status & GPIO_PIN0){
//        start_pulse=1;
//    }
//}
//
//
//


#include "driverlib.h"

#define MIN_DISTANCE 15.0f // Minimum distance of the detected ultrasonic sensor


bool start_pulse=0;
float dist;

const Timer_A_ContinuousModeConfig continuousModeConfig =
   {
            TIMER_A_CLOCKSOURCE_SMCLK,           // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_3,       // SMCLK/1 = 3MHz
            TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer ISR
            TIMER_A_DO_CLEAR                   // Skup Clear Counter
   };

   const Timer_A_CompareModeConfig CCR1Config =
   {
           TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR2
           TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
           TIMER_A_OUTPUTMODE_SET_RESET,
           0x000A                                // 10 Duty Cycle
   };

   const Timer_A_CompareModeConfig CCR2Config =
   {
           TIMER_A_CAPTURECOMPARE_REGISTER_2,          // Use CCR2
           TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
           TIMER_A_OUTPUTMODE_SET_RESET,
           0x000A                                // 10 Duty Cycle
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



      /* For left ultrasonic sensor */

             /* Configuring P2.5 as Output */
             MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
             MAP_GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN0);


             /* Configuring Timer_A0 for continuous Mode */
             MAP_Timer_A_initCompare(TIMER_A1_BASE, &CCR2Config);
             MAP_Timer_A_configureContinuousMode(TIMER_A1_BASE, &continuousModeConfig);
             MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
             MAP_Timer_A_disableInterrupt(TIMER_A1_BASE);

             MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN0);
             MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN0);
             MAP_GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN0);
             MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN0, GPIO_LOW_TO_HIGH_TRANSITION);



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
       while(1){

           //selectSensor(GPIO_PORT_P3, GPIO_PIN0, tval1,tval2,duration); // Front ultrasonic sensor
           selectSensor(GPIO_PORT_P3, GPIO_PIN0, tval1,tval2,duration); // Left ultrasonic sensor
       }

}

void selectSensor(uint32_t GPIO_PORT, uint32_t GPIO_PIN, uint32_t tval1, uint32_t tval2, uint32_t duration)
{

       asm("    nop");
       asm("    nop");
       asm("    nop");
       if(start_pulse==1) // when it is being interrupted
       {
           tval1 = MAP_Timer_A_getCounterValue(TIMER_A1_BASE); //The value when it hits the rising edge

           while (MAP_GPIO_getInputPinValue(GPIO_PORT, GPIO_PIN) == 1); // While waiting for the falling edge

           tval2 = MAP_Timer_A_getCounterValue(TIMER_A1_BASE); // Get the value of the falling edge

           start_pulse = 0; // Set back the start pulse back to 0

           if(tval1 > tval2)
               tval2 = tval2+0xffff; // Makes it count to the limit of the 0XFFFH to restart back to 0

           duration = tval2-tval1; // get the duration of the pulse
           dist= duration/58.0f; // To convert the echo

           if (dist < MIN_DISTANCE){
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
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, status);

    if(status & GPIO_PIN7){
        start_pulse=1;
    }
}





