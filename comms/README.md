# CSC2003 Team A2 Communications Module
## Table of contents
* [UART Performance](#uart-performance)
* [Wi-Fi M5StickC PLUS](#wi-fi-m5stickc-plus)
* [Wi-Fi ESP8266](#wi-fi-esp8266)
* [IOT protocols](#IOT-protocols)


## UART Performance
|Technology          |Latency          |Throughput |
|--------------------|-----------------|-----------|
|Wi-Fi M5stickC Plus |0.1346 ms/message|8.0973 KB/s|
|Wi-Fi ESP8266       |201.320 ms       |2.6213 MB/s|

We are able to calculate the throughput using the formula: 

`Throughput = Window Size (bits) / Round-Trip-Time (s)`

Standard TCP Window size is 64KB. 64KB converted to bits is 52488.

## Wi-Fi M5StickC PLUS
### Test Parameters:
Latency Test: 1000 characters sent sequentially

Throughput Test: 1KB message sent

### Result Images:

<ins>Latency Test</ins>

![Latency Screenshot](./assets/ttermpro_m5_latency.png)

<ins>Throughput Test</ins>

![Throughput Screenshot](./assets/ttermpro_m5_throughput.png)

## Wi-Fi ESP8266
## Test Parameters:
To check the round-trip-time, we used the ping feature within laptops. It will retrieve the minimum, average, maximum and standard deviation.

![Latency Screenshot](./assets/espLatency.png)

Latency Test: 64 bytes sent from Laptop to MSP432

## IOT protocols
Comparison betweeen UDP socket and MQTT
![Latency](./assets/wifiprotocol.png)
