#include <M5StickCPlus.h>
#include <Wire.h>

// Change this unique address for each I2C slave node
#define NODE_ADDRESS 48

void setup() {
  M5.begin();
  M5.Lcd.print("Running");
  Serial.begin(115200);           // start serial for output
  Wire.begin(NODE_ADDRESS);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
}

void loop()
{
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}