from m5stack import *
from m5ui import *
from uiflow import *

def lcd_print(data):
    lcd.clear(lcd.BLACK)
    lcd.text(lcd.CENTER, lcd.CENTER, data)

lcd_print("Ready")

# Create a serial port instance
uart1 = machine.UART(1, tx=32, rx=33)

# Initialize the serial port
uart1.init(460800, bits=8, parity=None, stop=1)

# Read/write case
while True:
    if uart1.any():
        message = uart1.read()
        lcd_print("Received")
        uart1.write("c")