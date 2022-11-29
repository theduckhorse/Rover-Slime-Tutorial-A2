from m5stack import *
from m5ui import *
from uiflow import *
import socket
import wifiCfg

def lcd_print(data):
    lcd.clear(lcd.BLACK)
    lcd.text(lcd.CENTER, lcd.CENTER, data)

lcd_print("hello world")
wifiCfg.screenShow()
wifiCfg.doConnect("username", "password")

HOST = "192.168.68.117"
PORT = 5000

udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

udpsocket.connect((HOST, PORT))

#udpsocket.send(DATA)

udpsocket.sendto("INIT", (HOST, PORT))

count = 0

while True:
    data = udpsocket.recv(4096)
    count += 1
    lcd_print(str(count))
    udpsocket.send(data)