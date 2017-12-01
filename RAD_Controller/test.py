from serial_utils import *
import timeout


connect()
send('the serial port is working! :D')
print receive()
