#include <driverlib.h>
#include <comms/ESP8266.h>
#include <comms/UART_Driver.h>

//Change this to the access point of your choice
#define SSID "ssid"
#define PASSWORD "password"

void changeState();
void listAP();
void connectToWifi();
void listIP();
void initWifi();

/* Calculation of config file is through https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 * System clock is set to 24,000,000 Hz and baud rate is 1,152,200 bps */
eUSCI_UART_Config UART0Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     13,
     0,
     37,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

eUSCI_UART_Config UART2Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     13,
     0,
     37,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

void main()
{
    WDT_A_holdTimer();

    initWifi();
    Interrupt_enableMaster();
    while(1){
        PCM_gotoLPM3InterruptSafe();
    }
}

/*Sets the ESP8266 Wifi mode to be a station*/
void changeState(){
    bool changed = ESP8266_ChangeMode1();
    if (!changed)
    {
        UART_Printf(EUSCI_A0_BASE, "Can't change station\n\r");
        changeState();
    }
    UART_Printf(EUSCI_A0_BASE, "Station Online\n\r");
}

/*List all available access points from the ESP8266 Station*/
void listAP(){
    bool listAvail = ESP8266_AvailableAPs();
    if(!listAvail){
        UART_Printf(EUSCI_A0_BASE, "List unavailable\n\r");
        listAP();
    }
    UART_Printf(EUSCI_A0_BASE, "List of AP:\n\r");
    UART_Printf(EUSCI_A0_BASE, ESP8266_GetBuffer());
}

/*Connects the ESP8266 module to the Wi-fi access points*/
void connectToWifi(){
    bool connectedAP = ESP8266_ConnectToAP(SSID, PASSWORD);
    if (!connectedAP)
    {
        UART_Printf(EUSCI_A0_BASE, "Cannot connect to AP\n\r");
        connectToWifi();
    }
    UART_Printf(EUSCI_A0_BASE, "Connected to WiFi\n\r");
}

void listIP(){
bool stationInfo = ESP8266_QueryIP();
    if(!stationInfo){
        UART_Printf(EUSCI_A0_BASE, "Station info unavailable\n\r");
        listAP();
    }
    UART_Printf(EUSCI_A0_BASE, "Station Info:\n\r");
    UART_Printf(EUSCI_A0_BASE, ESP8266_GetBuffer());
}

/*Ensures that the ports are all initialised. Highest baud rate  through the ESP8266 is */
void initWifi(){
    /*Ensure MSP432 is Running at 24 MHz*/
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
    PCM_setCoreVoltageLevel(PCM_VCORE1);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);

    /*Initialize required hardware peripherals for the ESP8266*/
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    UART_initModule(EUSCI_A0_BASE, &UART0Config);
    UART_enableModule(EUSCI_A0_BASE);
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    UART_initModule(EUSCI_A2_BASE, &UART2Config);
    UART_enableModule(EUSCI_A2_BASE);
    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA2);

    /*Reset GPIO of the ESP8266*/
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN1);

    ESP8266_HardReset();
    UART_Printf(EUSCI_A0_BASE, "ESP8266 Start\r\n");

    changeState();
    listAP();
    connectToWifi();
    listIP();
}
