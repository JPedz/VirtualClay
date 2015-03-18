import socket 
IPV4 = '172.23.249.126'
port = 5000
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
clientSocket.connect((IPV4,port))

clientSocket.send("1")
