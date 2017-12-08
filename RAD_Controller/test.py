from serial_utils import *
import timeout

connect('/dev/ttyS0')
send('the serial port is working! :D')
print receive()
