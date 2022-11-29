import socket
import time

# set host ip address and port and size of buffer
HOST = "0.0.0.0"
PORT = 5000
BUFFER = 4096

# variables for timer to benchmark
tic = 0
toc = 0

# set up scoket with UDP (socket datagram)
udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# assigns an IP address and a port number to a socket instance
udpsocket.bind((HOST, PORT))

# send encoded string to client
bytesToSend = str.encode("Test")

#udpsocket.sendto("test", (IP, 5000))

while True:
    # receive from client
    bytesAddressPair = udpsocket.recvfrom(BUFFER)
    # stop timer after receiving
    toc = time.perf_counter()
    # find the rtt by deducting start timer and end timer
    print(f"{toc - tic:0.4f}")
    message = bytesAddressPair[0]
    address = bytesAddressPair[1]

    # print client message
    # clientMsg = "Message from Client:{}".format(message)
    # clientIP  = "Client IP Address:{}".format(address)
    # print(clientMsg)
    # print(clientIP)

    # sleep
    time.sleep(1)

    # Sending a reply to client
    udpsocket.sendto(bytesToSend, address)
    #start the timer
    tic = time.perf_counter()