import socket

ipAddress = '160.119.248.176'
portNumber = 4242

print("Starting RAD_Team's controller")
print("Attempting to connect to: %s:%d" % (ipAddress, portNumber))

sock = socket.socket()

try:
    sock.connect((ipAddress, portNumber))
except:
    print("Connection failure")
    exit(0)

print("Connection success")
print("Telling the server that its the controller connecting")
sock.send("c")

print("Waiting for a responce")
return_string = sock.recv(256)

if(return_string == "ack"):
    print("Controller accepted")
else:
    print("Controller rejected")
    exit(0)


print("Waiting for commands from server")
x = 0
while(1):
    print sock.recv(256)
    sock.send("ack%d" % x)
    x = x + 1

print("Closing connection")
sock.close()