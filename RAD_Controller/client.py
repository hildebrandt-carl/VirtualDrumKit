import socket
from serial_utils import *

ipAddress = '160.119.248.28'
tcpPort = 4242

serialPort = '/dev/ttyS0'

print("Starting RAD_Team's controller")


print("Attempting TCP connection : %s:%d" % (ipAddress, tcpPort))
sock = socket.socket()

try:
    sock.connect((ipAddress, tcpPort))
except:
    print("Connection failure")
    exit(0)

print("Connection success")


print("Attempting serial connection : %s" % (serialPort))
connect('/dev/ttyS0')

print("Sending webserver that this is a controller.")
try:
    sock.send("controller")
except:
    print("Could not send webserver a message")


print("Waiting for  web server responce")
#TODO: Make this have a timeout period?
try:
    return_string = sock.recv(256)
except (OSError, serial.SerialException):
    print("No responce recieved")

if(return_string == "ack"):
    print("Controller accepted")
else:
    print("Controller rejected")

print("Waiting for commands from web server")
x = 0
while(1):
    webserverMSG = sock.recv(256)
    print("Recieved : %s", webserverMSG)
    print("Forawrding message on serial")
    try:
        send(webserverMSG)
    except:
        print("Unable to send message")

print("Closing the web server connection")
try:
    sock.close()
except:
    print("Could not close webserver connection")

print("Closing the serial connection")
try:
    disconnect()
except:
    print("Could not close the serial conneciton")