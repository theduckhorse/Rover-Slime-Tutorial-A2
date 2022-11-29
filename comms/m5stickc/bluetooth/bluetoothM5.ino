#include <M5StickCPlus.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error BLUETOOTH IS NOT ENABLED!
#endif

BluetoothSerial SerialBT;

void setup() {

    M5.begin();  // Initialize M5StickC Plus

    M5.Lcd.setTextSize(3);  // Set font size
    M5.Lcd.setRotation(3);  // Rotate the screen

    // LCD display
    M5.Lcd.print("READY");

    Serial.begin(9600, SERIAL_8N1, 33, 32);
    SerialBT.begin("ESP32Test");

    }

/* After the program in setup() runs, it runs the program in loop()
The loop() function is an infinite loop in which the program runs repeatedly
*/
void loop() {
  while(Serial.available() > 0){
    String receivedData = Serial.readString();
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0,0);
    M5.Lcd.print(receivedData);
    SerialBT.print(receivedData);
  }
}