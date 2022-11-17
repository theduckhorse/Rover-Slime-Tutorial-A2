from m5stack import *
from m5ui import *
from uiflow import *

def lcd_print(data):
    lcd.clear(lcd.BLACK)
    lcd.text(lcd.CENTER, lcd.CENTER, data)

lcd_print("Ready")

count = 0

# Create a serial port instance
uart1 = machine.UART(1, tx=32, rx=33)

# Initialize the serial port
uart1.init(460800, bits=8, parity=None, stop=1)

# Read/write case
while True:
    if uart1.any():
        lcd.clear(lcd.BLACK)
        message = uart1.read()
        count += len(message)
        lcd_print(str(count))
        if count == 1000:
            uart1.write("c")
            count = 0