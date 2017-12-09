from serial_utils import *
import timeout

connect()
send('1234567890the serial port is working! :D')
print receive()
