from m5stack import *
from m5ui import *
from uiflow import *
import socket
import wifiCfg

def lcd_print(data):
    lcd.clear(lcd.BLACK)
    lcd.text(lcd.CENTER, lcd.CENTER, data)

# test LCD print
lcd_print("hello world")

# connect to wifi
wifiCfg.screenShow()
wifiCfg.doConnect("username", "password")

# set host ip address and port
HOST = "192.168.68.117"
PORT = 5000

# set up udpsocket
udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# connect to socket
udpsocket.connect((HOST, PORT))

#udpsocket.send(DATA)

# send a message to start the loop
udpsocket.sendto("INIT", (HOST, PORT))

# declare counter
count = 0

while True:
    # receive packet
    data = udpsocket.recv(4096)
    # increment counter and display on m5stick
    count += 1
    lcd_print(str(count))
    # send packet back to calculate RTT
    udpsocket.send(data)