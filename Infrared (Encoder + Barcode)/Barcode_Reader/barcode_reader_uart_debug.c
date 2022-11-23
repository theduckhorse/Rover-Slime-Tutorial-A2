/**
 * barcode_reader.c
 *
 * Authors: Goh Yee Kit
 *          Zhang XiangHui
 *          Cham Xun Thong
 *
 * Interrupts for infrared sensor and decoding functions for Code 39 Barcodes
 * Pin 5.5 > ADC input for infrared sensor
 *
 *                 MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P5.5  |<--- Infrared Sensor
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *             ------------------
 *
 */

/* DriverLib Includes */
#include "driverlib.h"

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
int bars[200];
int spaces[200];
int barsLength = sizeof(bars) / sizeof(bars[0]);
int spacesLength = sizeof(spaces) / sizeof(spaces[0]);

// UART Config for 115200 baud rate at 48mhz
const eUSCI_UART_Config uartConfig =
    {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source
        26,                                            // BRDIV = 26
        0,                                             // UCxBRF = 0
        111,                                           // UCxBRS = 111
        EUSCI_A_UART_ODD_PARITY,                       // ODD Parity
        EUSCI_A_UART_LSB_FIRST,                        // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
        EUSCI_A_UART_MODE,                             // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // OverSampling
};

// Compares binary string to match character mapping table of Code39 Barcode
// Refer to mappings from binary to character based on bars and spaces:
// https://www.cristallight.com/ibarcoder/help/barcodes/c39.html
unsigned char decodeCode39()
{
    if (strcmp(binaryStr, "100010100") == 0)
    {
        return '1';
    }
    else if (strcmp(binaryStr, "010010100") == 0)
    {
        return '2';
    }
    else if (strcmp(binaryStr, "110000100") == 0)
    {
        return '3';
    }
    else if (strcmp(binaryStr, "001010100") == 0)
    {
        return '4';
    }
    else if (strcmp(binaryStr, "101000100") == 0)
    {
        return '5';
    }
    else if (strcmp(binaryStr, "011000100") == 0)
    {
        return '6';
    }
    else if (strcmp(binaryStr, "000110100") == 0)
    {
        return '7';
    }
    else if (strcmp(binaryStr, "100100100") == 0)
    {
        return '8';
    }
    else if (strcmp(binaryStr, "010100100") == 0)
    {
        return '9';
    }
    else if (strcmp(binaryStr, "001100100") == 0)
    {
        return '0';
    }
    else if (strcmp(binaryStr, "100010010") == 0)
    {
        return 'A';
    }
    else if (strcmp(binaryStr, "010010010") == 0)
    {
        return 'B';
    }
    else if (strcmp(binaryStr, "110000010") == 0)
    {
        return 'C';
    }
    else if (strcmp(binaryStr, "001010010") == 0)
    {
        return 'D';
    }
    else if (strcmp(binaryStr, "101000010") == 0)
    {
        return 'E';
    }
    else if (strcmp(binaryStr, "011000010") == 0)
    {
        return 'F';
    }
    else if (strcmp(binaryStr, "000110010") == 0)
    {
        return 'G';
    }
    else if (strcmp(binaryStr, "100100010") == 0)
    {
        return 'H';
    }
    else if (strcmp(binaryStr, "010100010") == 0)
    {
        return 'I';
    }
    else if (strcmp(binaryStr, "001100010") == 0)
    {
        return 'J';
    }
    else if (strcmp(binaryStr, "100010001") == 0)
    {
        return 'K';
    }
    else if (strcmp(binaryStr, "010010001") == 0)
    {
        return 'L';
    }
    else if (strcmp(binaryStr, "110000001") == 0)
    {
        return 'M';
    }
    else if (strcmp(binaryStr, "001010001") == 0)
    {
        return 'N';
    }
    else if (strcmp(binaryStr, "101000001") == 0)
    {
        return 'O';
    }
    else if (strcmp(binaryStr, "011000001") == 0)
    {
        return 'P';
    }
    else if (strcmp(binaryStr, "000110001") == 0)
    {
        return 'Q';
    }
    else if (strcmp(binaryStr, "100100001") == 0)
    {
        return 'R';
    }
    else if (strcmp(binaryStr, "010100001") == 0)
    {
        return 'S';
    }
    else if (strcmp(binaryStr, "001100001") == 0)
    {
        return 'T';
    }
    else if (strcmp(binaryStr, "100011000") == 0)
    {
        return 'U';
    }
    else if (strcmp(binaryStr, "010011000") == 0)
    {
        return 'V';
    }
    else if (strcmp(binaryStr, "110001000") == 0)
    {
        return 'W';
    }
    else if (strcmp(binaryStr, "001011000") == 0)
    {
        return 'X';
    }
    else if (strcmp(binaryStr, "101001000") == 0)
    {
        return 'Y';
    }
    else if (strcmp(binaryStr, "011001000") == 0)
    {
        return 'Z';
    }
    else if (strcmp(binaryStr, "000111000") == 0)
    {
        return '-';
    }
    else if (strcmp(binaryStr, "100101000") == 0)
    {
        return '.';
    }
    else if (strcmp(binaryStr, "010101000") == 0)
    {
        return ' ';
    }
    else if (strcmp(binaryStr, "001101000") == 0)
    {
        return '*';
    }
    else if (strcmp(binaryStr, "000001110") == 0)
    {
        return '$';
    }
    else if (strcmp(binaryStr, "000001101") == 0)
    {
        return '/';
    }
    else if (strcmp(binaryStr, "000001011") == 0)
    {
        return '+';
    }
    else if (strcmp(binaryStr, "000000111") == 0)
    {
        return '%';
    }
    else
    {
        return '?';
    }
}

int main(void)
{
    initBarcodeConfig();
    uPrintf("Script Start\n\r");

    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
}

// Config for GPIO and other setup
void initBarcodeConfig()
{
    /* Halting the Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    /* Initializing Variables */
    curADCResult = 0;

    /* Setting Flash wait state */
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Setting DCO to 48MHz */
    MAP_PCM_setPowerState(PCM_AM_LDO_VCORE1);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    /* Initializing ADC (MCLK/1/4) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4, 0);

    /* Configuring GPIOs (5.5 A0) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory */
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                        ADC_INPUT_A0, false);

    /* Configuring Sample Timer */
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    /* Enabling/Toggling Conversion */
    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();

    /* Enabling interrupts */
    MAP_ADC14_enableInterrupt(ADC_INT0);
    MAP_Interrupt_enableInterrupt(INT_ADC14);
    MAP_Interrupt_enableMaster();
}

void uPrintf(unsigned char *TxArray)
{
    unsigned short i = 0;
    while (*(TxArray + i))
    {
        UART_transmitData(EUSCI_A0_BASE, *(TxArray + i)); // Write the character at the location specified by pointer
        i++;                                              // Increment pointer to point to the next character
    }
}

/* ADC Interrupt Handler. This handler is called whenever there is a conversion that is finished for ADC_MEM0. */
void ADC14_IRQHandler(void)
{
    uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    if (ADC_INT0 & status)
    {
        curADCResult = MAP_ADC14_getResult(ADC_MEM0);
        normalizedADCRes = (curADCResult * 3.3) / 16384; // normalized ADC result to 3.3V
        if (normalizedADCRes > THRESHOLD)
        { // space
            if (counter > 0 && isBar == false)
            {
                if (counter > COUNTTHRESHOLD)
                { // if counter if over threshold, add to spaces array the count of the space
                    spaces[spacesIndex] = counter;
                    spacesIndex++; // increase index
                    counter = 0;   // resets counter
                }
                else
                {
                    counter = 0;
                }
            }
            isBar = true;
            counter++;
        }
        else
        {
            if (counter > 0 && isBar == true)
            { // bar
                if (counter > COUNTTHRESHOLD)
                { // if counter if over threshold, add to bars array the count of the bar
                    bars[barsIndex] = counter;
                    barsIndex++; // increase index
                    counter = 0; // resets counter after adding to array
                }
                else
                {
                    counter = 0;
                }
            }
            isBar = false;
            counter++;
        }
        if (counter > LTHRESHOLD)
        { // if counter > the length threshold decode current data
            if (bars[0] != 0 || spaces[0] != 0)
            {
                spaceHighest = 0;
                spaceLowest = 999999;
                spaceAvg = 0;
                spaceAmount = 0;
                barAmount = 0;
                barHighest = 0;
                barLowest = 999999;
                barAvg = 0;

                if (outIsBar == false)
                {
                    spaceOffset = 1;
                    barOffset = 0;
                }
                else
                {
                    spaceOffset = 1;
                    barOffset = 1;
                }
                for (i = 0 + barOffset; i < barsLength; i++)
                {
                    if (bars[i] == 0)
                    {
                        barAmount = i - barOffset;
                        break;
                    }
                    if (bars[i] > barHighest)
                    {
                        barHighest = bars[i];
                    }
                    if (bars[i] < barLowest)
                    {
                        barLowest = bars[i];
                    }
                }
                for (i = 0 + spaceOffset; i < spacesLength; i++)
                {
                    if (spaces[i] == 0)
                    {
                        spaceAmount = i - spaceOffset;
                        break;
                    }
                    if (spaces[i] > spaceHighest)
                    {
                        spaceHighest = spaces[i];
                    }
                    if (spaces[i] < spaceLowest)
                    {
                        spaceLowest = spaces[i];
                    }
                }
                memset(decodedOutput, 0, sizeof decodedOutput);
                int barcodeLength = barAmount / 5; // total number of char in barcode
                int decodeCounter = 0;              // decode counter
                unsigned char tmp[1] = {};           // tmp char array to hold decoded character
                barAvg = (barHighest + barLowest) / 2 - barLowest / 3; // Get the avg of the total counts of black bars
                spaceAvg = (spaceHighest + spaceLowest) / 2 - spaceLowest / 3; // Get the avg of the total counts of white spaces
                bars[0 + barOffset] = barLowest; // offsets the 2nd index to lowest
                bars[1 + barOffset] = barLowest; // offsets the 3rd index to lowest
                spaces[0 + spaceOffset] = spaceHighest; // offsets the 2nd index to lowest
                spaces[1 + spaceOffset] = spaceLowest; // offsets the 2nd index to lowest
                for (i = 0; i < barcodeLength; i++)
                {
                    memset(binaryStr, 0, sizeof binaryStr); // resets binarystring array after every character decoded
                    if (bars[decodeCounter + barOffset] >= barAvg)
                    { // if >= barAvg the width of black line is wide
                        binaryStr[0] = '1';
                    }
                    else
                    { // else the width of black line is thin
                        binaryStr[0] = '0';
                    }
                    if (bars[decodeCounter + 1 + barOffset] >= barAvg)
                    {
                        binaryStr[1] = '1';
                    }
                    else
                    {
                        binaryStr[1] = '0';
                    }
                    if (bars[decodeCounter + 2 + barOffset] >= barAvg)
                    {
                        binaryStr[2] = '1';
                    }
                    else
                    {
                        binaryStr[2] = '0';
                    }
                    if (bars[decodeCounter + 3 + barOffset] >= barAvg)
                    {
                        binaryStr[3] = '1';
                    }
                    else
                    {
                        binaryStr[3] = '0';
                    }
                    if (bars[decodeCounter + 4 + barOffset] >= barAvg)
                    {
                        binaryStr[4] = '1';
                    }
                    else
                    {
                        binaryStr[4] = '0';
                    }
                    if (spaces[decodeCounter + spaceOffset] >= spaceAvg)
                    {
                        binaryStr[5] = '1';
                    }
                    else
                    {
                        binaryStr[5] = '0';
                    }
                    if (spaces[decodeCounter + 1 + spaceOffset] >= spaceAvg)
                    { // offsets for blank space after each char
                        binaryStr[6] = '1';
                    }
                    else
                    {
                        binaryStr[6] = '0';
                    }
                    if (spaces[decodeCounter + 2 + spaceOffset] >= spaceAvg)
                    {
                        binaryStr[7] = '1';
                    }
                    else
                    {
                        binaryStr[7] = '0';
                    }
                    if (spaces[decodeCounter + 3 + spaceOffset] >= spaceAvg)
                    {
                        binaryStr[8] = '1';
                    }
                    else
                    {
                        binaryStr[8] = '0';
                    }
                    decodeCounter += 5;          // adds five to decodeCounter for next character
                    tmp[0] = decodeCode39();     // decodes character
                    strcat(decodedOutput, tmp); // concat decoded character to output
                }
                if (barcodeLength > 0)
                {
                    uPrintf("\n\rOutput: ");
                    uPrintf(decodedOutput);
                }
                memset(bars, 0, sizeof bars);
                memset(spaces, 0, sizeof spaces);
                counter = 0;
                spacesIndex = 0;
                barsIndex = 0;
            }
            else
            {
                // resets counters and index if first index is 0
                counter = 0;
                spacesIndex = 0;
                barsIndex = 0;
                outIsBar = isBar;
            }
        }
        MAP_ADC14_toggleConversionTrigger();
    }
}
