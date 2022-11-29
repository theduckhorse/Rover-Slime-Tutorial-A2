from m5stack import *
from m5ui import *
from uiflow import *
import urequests as request
import ujson
import wifiCfg
import time

setScreenColor(0x111111)

ssid = ''
password = ''
requestURL='http://ptsv2.com/t/aviendre/post'

lcd.clear()
M5Led.on()
wifiCfg.doConnect(ssid, password)
lcd.print('Connected to wifi', 0, 0, 0xffffff)

wait(2)
lcd.clear()
lcd.print('Waiting for UART',0,0,0xffffff)

uart1 = machine.UART(1,tx=32,rx=33)
uart1.init(9600,bits=8,parity=None,stop=1)
while True:
  if uart1.any():
    lcd.clear()
    lcd.print('UART RECEIVED',0,0,0xffffff)
    wait(3)
    
    message = (uart1.read()).decode()
    lcd.clear()
    lcd.print(message,0,0,0xffffff)
    wait(3)
    
    post = ujson.dumps({'hello':message})
    lcd.clear()
    request.post(requestURL,headers={'Content-type':'application/json'},data=post)
    lcd.print('POST sent',0,0,0xffffff)
    
    wait(3)
    lcd.clear()
    lcd.print('Waiting for UART',0,0,0xffffff)

try:
  post = ujson.dumps({'hello':'hello'})
  requestURL='http://ptsv2.com/t/aviendre/post'
  request.post(requestURL,headers={'Content-type':'application/json'},data=post)
  data = request.get(requestURL).text
  lcd.clear()
  lcd.print(data,0,0,0xffffff)
  
except:
  print('Error')