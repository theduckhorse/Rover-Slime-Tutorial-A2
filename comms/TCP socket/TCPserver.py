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

try:
    while 1:
        newSocket, address = tcpsocket.accept()
        print("Connected")
        # loop serving the new client
        while 1:
            receivedData = newSocket.recv(1024)
            # stop timer after receiving
            toc = time.perf_counter()
            # find the rtt by deducting start timer and end timer
            print(f"{toc - tic:0.4f}")
            if not receivedData: break
            # sleep
            time.sleep(1)
            # Echo back the same data you just received
            newSocket.send(receivedData)
            #start the timer
            tic = time.perf_counter()
        newSocket.close(  )
        print("Disconnected from")
finally:
    tcpsocket.close()