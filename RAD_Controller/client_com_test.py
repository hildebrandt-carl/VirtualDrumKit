import socket


ipAddress = '160.119.248.176'

ipAddress = '127.0.0.1'
portNumber = 4242

print("Starting RAD_Team's web connection")
print("Attempting to connect to: %s:%d" % (ipAddress, portNumber))

sock = socket.socket()

try:
    sock.connect((ipAddress, portNumber))
except:
    print("Connection failure")
    exit(0)

print("Connection success")
print("Telling the server that its the web connecting")
sock.send("web")

print("Waiting for a responce")
return_string = sock.recv(256)

if(return_string == "ack"):
    print("Controller accepted")
else:
    print("Controller rejected")
    exit(0)


print("Sending two messages to the server")
sock.send("First Message")
sock.send("Seccond Message")

print("Closing connection")
sock.close()