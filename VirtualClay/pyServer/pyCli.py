import socket 
IPV4 = '10.0.0.15'
port = 5000
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
while 1:
	clientSocket.connect((IPV4,port))
	var = raw_input("enter a value")
	if(var == "q"):
		break
	clientSocket.send(str(var))
	clientSocket.close()
