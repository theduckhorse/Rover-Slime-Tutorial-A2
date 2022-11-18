import socket
import time
import os
udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print(socket.AF_INET)

udpsocket.bind(('0.0.0.0', 5000))
bufferSize  = 4096
msgFromServer = str(os.urandom(256))
bytesToSend = str.encode(msgFromServer)

#udpsocket.sendto("test", (IP, 5000))
tic = 0
while True:
    bytesAddressPair = udpsocket.recvfrom(bufferSize)
    toc = time.perf_counter()
    print(f"{toc - tic:0.4f}")
    message = bytesAddressPair[0]
    address = bytesAddressPair[1]

    # clientMsg = "Message from Client:{}".format(message)
    # clientIP  = "Client IP Address:{}".format(address)
    
    # print(clientMsg)
    # print(clientIP)

    # Sending a reply to client
    udpsocket.sendto(bytesToSend, address)
    tic = time.perf_counter()