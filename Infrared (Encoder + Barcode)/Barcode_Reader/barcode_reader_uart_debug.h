/*
 * barcode_reader.h
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
#define COUNTTHRESHOLD 50 // counter threshold of 50 to filter out random fast movement
#define LTHRESHOLD 3000   // length counter threshold to signify end of reading
#define BARCOUNT 5        // count of how many bars in 1 char

/* Declare Functions */
void uPrintf(unsigned char *TxArray);
unsigned char decodeCode39();
void initBarcodeConfig();

/* Statics */
static volatile uint16_t curADCResult;
static volatile float normalizedADCRes;

/* Global/App Variables */
int counter = 0;
int spacesIndex = 0;
int barsIndex = 0;
int spaceAmount = 0;
int spaceHighest = 0;
int spaceLowest = 999999;
int spaceAvg = 0;
int barAmount = 0;
int barHighest = 0;
int barLowest = 999999;
int barAvg = 0;
int i = 0;
int spaceOffset = 0;
int barOffset = 0;
bool isBar = true;
bool outIsBar = true;
unsigned char *decodedOutput[20];
char binaryStr[9];
int bars[50];
int spaces[50];
int barsLength = sizeof(bars) / sizeof(bars[0]);
int spacesLength = sizeof(spaces) / sizeof(spaces[0]);
char str[80];


#endif /* BARCODE_READER_UART_DEBUG_H_ */
