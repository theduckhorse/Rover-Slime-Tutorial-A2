from m5stack import *
from m5ui import *
from uiflow import *
import socket
import wifiCfg

# set host ip address and port and size of buffer
HOST = "192.168.68.117"
PORT = 5000
BUFFER = 4096

# function print message on LCD screen
def lcd_print(data):
    lcd.clear(lcd.BLACK)
    lcd.text(lcd.CENTER, lcd.CENTER, data)

# print hello world on lcd
lcd_print("hello world")

# connect to wifi
wifiCfg.screenShow()
wifiCfg.doConnect("username", "password")

# set up scoket with UDP (socket datagram)
udpsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# connect to socket
udpsocket.connect((HOST, PORT))

# send packet without specified receiver
#udpsocket.send(DATA)

# send packet 
udpsocket.sendto("INIT", (HOST, PORT))

# counter to displayed on LCD
count = 0

while True:
    # receive buffer server
    data = udpsocket.recv(BUFFER)

    # increment counter
    count += 1

    # display updated counter on screen
    lcd_print(str(count))

    # send a new packet to client
    udpsocket.send(data)