/*
 * wheel_encoder_uart_debug.h
 *
 *  Created on: 25 Nov 2022
 *      Author: Yee Kit
 */

#ifndef WHEEL_ENCODER_UART_DEBUG_H_
#define WHEEL_ENCODER_UART_DEBUG_H_


/* Standard Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

/* Application Defines  */
#define TIMER_PERIOD 1024       // timer period to get 1 sec timer
#define TIMEVAL 60              // used to calculate RPM
#define TOTALNOTCHES 20         // total notches on wheel
#define CMtoM 100               // convert cm to m
#define WHEELCIRCUMFERENCE 20.4 // wheel circumference in cm
#define WHEELDIAMETER 0.65      // wheel diameter in cm
#define NOTCHLENGTH 1.02        // 1 notch length in cm

/* Global Variables */
extern volatile uint32_t leftCounter;
extern volatile uint32_t rightCounter;
extern volatile uint32_t leftRPM;
extern volatile uint32_t rightRPM;
extern volatile float curSpeed;
extern volatile float totalDist;
extern volatile char str[80];

/* Declare Functions */
void uPrintf(unsigned char *TxArray);
void initWheelEncoderConfig();


#endif /* WHEEL_ENCODER_UART_DEBUG_H_ */
