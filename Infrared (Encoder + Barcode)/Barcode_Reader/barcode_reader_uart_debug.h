/*
 * barcode_reader_uart_debug.h
 *
 *  Created on: 25 Nov 2022
 *      Author: Yee Kit
 */

#ifndef BARCODE_READER_UART_DEBUG_H_
#define BARCODE_READER_UART_DEBUG_H_

/* Standard Includes */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* Application Defines */
#define THRESHOLD 1.8     // normalized ADC result threshold
#define COUNTTHRESHOLD 10 // counter threshold of 50 to filter out random fast movement
#define LTHRESHOLD 500   // length counter threshold to signify end of reading
#define BARCOUNT 5        // count of how many bars in 1 char

/* Declare Functions */
void uPrintf(unsigned char *TxArray);
unsigned char decodeCode39();
void initBarcodeConfig();

/* Statics */
static volatile uint16_t curADCResult;
static volatile float normalizedADCRes;

/* Global/App Variables */
extern int counter;
extern int spacesIndex;
extern int barsIndex;
extern int spaceAmount;
extern int spaceHighest;
extern int spaceLowest;
extern int spaceAvg;
extern int barAmount;
extern int barHighest;
extern int barLowest;
extern int barAvg;
extern int i;
extern int spaceOffset;
extern int barOffset;
extern bool isBar;
extern bool outIsBar;
extern unsigned char *decodedOutput[20];
extern char binaryStr[9];
extern int bars[50];
extern int spaces[50];
extern int barsLength;
extern int spacesLength;
extern char str[80];


#endif /* BARCODE_READER_UART_DEBUG_H_ */
