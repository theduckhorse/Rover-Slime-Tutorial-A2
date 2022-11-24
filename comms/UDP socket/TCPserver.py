import socket
import time

# set host ip address and port and size of buffer
HOST = "0.0.0.0"
PORT = 5000
BUFFER = 4096

# variables for timer to benchmark
tic = 0
toc = 0

# set up scoket with TCP (socket stream)
tcpsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# assigns an IP address and a port number to a socket instance
tcpsocket.bind((HOST, PORT))

# send encoded string to client
bytesToSend = str.encode("Test")

# Listen for incoming connections
tcpsocket.listen(1)

while True:
    # Wait for a connection
    print('waiting for a connection')
    connection, client_address = tcpsocket.accept()

    while True:
        # receive from client
        bytesAddressPair = tcpsocket.recvfrom(BUFFER)
        # stop timer after receiving
        toc = time.perf_counter()
        # find the rtt by deducting start timer and end timer
        print(f"{toc - tic:0.4f}")

        # decode the payload
        message = bytesAddressPair[0]
        address = bytesAddressPair[1]

        # sleep
        time.sleep()

        # Sending a reply to client
        tcpsocket.sendto(bytesToSend, address)
        #start the timer
        tic = time.perf_counter()