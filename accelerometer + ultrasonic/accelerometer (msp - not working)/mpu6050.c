/* DriverLib Defines */
#include "driverlib.h"

/* Standard Defines */
#include <stdint.h>

#include <stdbool.h>
#include <string.h>

/* Slave Address for I2C Slave */
#define SLAVE_ADDRESS 0x68
#define NUM_OF_REC_BYTES 10

/* Variables */
const uint8_t TXData[2] = {0x04, 0x00};
static uint8_t RXData[NUM_OF_REC_BYTES];
static volatile uint32_t xferIndex;
static volatile bool stopSent;

/* I2C Master Configuration Parameter */
const eUSCI_I2C_MasterConfig i2cConfig =
    {
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,     // SMCLK Clock Source
        3000000,                           // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS, // Desired I2C Clock of 100khz
        0,                                 // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP           // No Autostop
};

int main(void)
{
    volatile uint32_t ii;

    /* Disabling the Watchdog  */
    MAP_WDT_A_holdTimer();

    /* Select Port 1 for I2C - Set Pin 6, 7 to input Primary Module Function,
     *   (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
     */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN5 + GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Initializing I2C Master to SMCLK at 100khz with no autostop */
    MAP_I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);

    /* Specify slave address */
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, SLAVE_ADDRESS);

    /* Set Master in transmit mode */
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    /* Enable I2C Module to start operations */
    MAP_I2C_enableModule(EUSCI_B1_BASE);

    /* Enable and clear the interrupt flag */
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE,
                               EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    // Enable master Receive interrupt
    MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableInterrupt(INT_EUSCIB1);

    while (1)
    {
        /* Making sure the last transaction has been completely sent out */
        while (MAP_I2C_masterIsStopSent(EUSCI_B1_BASE) == EUSCI_B_I2C_SENDING_STOP)
            ;

        /* Send start and the first byte of the transmit buffer. We have to send
         * two bytes to clean out whatever is in the buffer from a previous
         * send  */
        MAP_I2C_masterSendMultiByteStart(EUSCI_B1_BASE, TXData[0]);
        MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, TXData[0]);

        /* Enabling transfer interrupt after stop has been sent */
        MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

        /* While the stop condition hasn't been sent out... */
        while (!stopSent)
        {
            MAP_PCM_gotoLPM0InterruptSafe();
        }

        stopSent = false;
    }
}

/*******************************************************************************
 * eUSCIB1 ISR. The repeated start and transmit/receive operations happen
 * within this ISR.
 *******************************************************************************/
void EUSCIB1_IRQHandler(void)
{
    uint_fast16_t status;

    status = MAP_I2C_getEnabledInterruptStatus(EUSCI_B1_BASE);
    MAP_I2C_clearInterruptFlag(EUSCI_B1_BASE, status);

    /* If we reached the transmit interrupt, it means we are at index 1 of
     * the transmit buffer. When doing a repeated start, before we reach the
     * last byte we will need to change the mode to receive mode, set the start
     * condition send bit, and then load the final byte into the TXBUF.
     */
    if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0)
    {
        MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, TXData[1]);
        MAP_I2C_disableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
        MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        xferIndex = 0;
        MAP_I2C_masterReceiveStart(EUSCI_B1_BASE);
        MAP_I2C_enableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
    }

    /* Receives bytes into the receive buffer. If we have received all bytes,
     * send a STOP condition */
    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0)
    {
        if (xferIndex == NUM_OF_REC_BYTES - 2)
        {
            MAP_I2C_masterReceiveMultiByteStop(EUSCI_B1_BASE);
            RXData[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
        }
        else if (xferIndex == NUM_OF_REC_BYTES - 1)
        {
            RXData[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
            MAP_I2C_disableInterrupt(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
            MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
            xferIndex = 0;
            stopSent = true;
            MAP_Interrupt_disableSleepOnIsrExit();
        }
        else
        {
            RXData[xferIndex++] = MAP_I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
        }
    }
}