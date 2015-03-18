import socket 
import getIP

IPV4 = getIP.get_lan_ip()
port = 5000
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serverSocket.bind((IPV4,port))
serverSocket.listen(2)
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

while 1:

	connection, address = serverSocket.accept()
	data = connection.recv(1);
	print data 