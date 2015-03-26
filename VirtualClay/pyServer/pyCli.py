import socket 
<<<<<<< HEAD
IPV4 = '172.23.249.126'
=======
IPV4 = '192.168.11.102'
>>>>>>> ff3a395c4d5d5efbe039fd7f9f2c92569223db66
port = 5000
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
while 1:
	clientSocket.connect((IPV4,port))
	var = raw_input("enter a value")
	if(var == "q"):
		break
	clientSocket.send(str(var))
	clientSocket.close()
