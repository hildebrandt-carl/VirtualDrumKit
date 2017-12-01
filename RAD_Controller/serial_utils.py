import subprocess
import serial
import sys
import glob
import time
from timeout import timeout
from timeout import TimeoutError

def getConnected():
	global connected
	return connected

@timeout(10)
def connect():
	global connected
	global portName
	global port
	connected = False
	portName = '/dev/ttyS0'
	try:
		port = serial.Serial(portName)
		port.timeout = 1
		port.write_timeout = 1
		connected = True
		return
	except (OSError, serial.SerialException):
		pass
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
