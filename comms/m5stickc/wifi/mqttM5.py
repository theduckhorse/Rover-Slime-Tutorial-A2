from m5stack import *
from m5ui import *
from uiflow import *
from m5mqtt import M5mqtt
import wifiCfg
import time

def lcd_print(data):
    lcd.clear(lcd.BLACK)
    lcd.text(lcd.CENTER, lcd.CENTER, data)


def callback(topic_data):
  pass

lcd_print("hello world")
wifiCfg.screenShow()
# Replace "SSID" and "PASSWORD" with your WiFi SSID and Password
wifiCfg.doConnect("username", "password")

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

message = "test"

# Read/write case
while True:
    time.sleep(1)
    m5mqtt.publish("csc2003comms", message)