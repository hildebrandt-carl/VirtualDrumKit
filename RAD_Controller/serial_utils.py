import subprocess
import serial
import sys
import glob
import time
import struct
from timeout import timeout
from timeout import TimeoutError

# usage: sendMessage( control_byte, drums_byte, int )
# example: sendMessage( 0, 4, 42 )
def sendMessage(*args):
	bytes = struct.pack('I',args[2])
	sum = args[0] + args[1] + ord(bytes[0]) + ord(bytes[1]) + ord(bytes[2]) + ord(bytes[3])
	send(chr(0xFF))    #start byte
	send(chr(args[0])) #control
	send(chr(args[1])) #drums
	send(bytes[0])     #int
	send(bytes[1])
	send(bytes[2])
	send(bytes[3])
	send(chr(sum%256)) #checksum

def getConnected():
	global connected
	return connected

@timeout(10)
def connect(*args):
	global connected
	global portName
	global port
	connected = False
	portName = "/dev/ttyS0"
	try:
		port = serial.Serial(portName)
                port.baudrate = 115200
		port.timeout = 1
		port.write_timeout = 1
		connected = True
		return
	except (OSError, serial.SerialException):
		print("Connection failure")
	return

def disconnect(*args):
	global port
	global connected
	try:
		port.close()
	except Exception:
		pass
	connected = False

@timeout(5)
def send(*args):
	global port
	global connected
	if ~connected:
		connect()
	if connected:
		port.write(args[0])
	else:
		print 'ERROR: cannot connect'

@timeout(5)
def receive(*args):
	global port
	global connected
	if ~connected:
		connect()
	if connected:
		return port.readline()
	else:
		print 'ERROR: cannot connect'
