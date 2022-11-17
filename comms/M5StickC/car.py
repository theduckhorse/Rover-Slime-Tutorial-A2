from m5stack import *
from m5ui import *
from uiflow import *
from m5mqtt import M5mqtt
import wifiCfg

def lcd_print(data):
    lcd.clear(lcd.BLACK)
    lcd.text(lcd.CENTER, lcd.CENTER, data)


def callback(topic_data):
  uart.write(topic_data)

lcd_print("hello world")
wifiCfg.screenShow()
# Replace "SSID" and "PASSWORD" with your WiFi SSID and Password
wifiCfg.doConnect("SSID", "PASSWORD")

m5mqtt = M5mqtt(
    "mosquitto",
    "test.mosquitto.org",
    port=1883,
    user=None,
    password=None,
    keepalive=300,
    ssl=False,
    ssl_params=None
)

# Start connection
m5mqtt.subscribe("csc2003comms", callback)
m5mqtt.start()
lcd_print("MQTT Connected")

# Create a serial port instance
uart1 = machine.UART(1, tx=32, rx=33)

# Initialize the serial port
uart1.init(9600, bits=8, parity=None, stop=1)

# Read/write case
while True:
    if uart1.any():
        lcd.clear(lcd.BLACK)
        message = uart1.read()
        lcd_print(message)
        m5mqtt.publish("csc2003comms", message)