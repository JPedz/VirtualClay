import socket 
IPV4 = '10.0.0.15'
port = 5000
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
clientSocket.connect((IPV4,port))

clientSocket.send("1")
