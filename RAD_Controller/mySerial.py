from serial_utils import *
import struct

connect()

sendMessage(16,0,(pow(2,32)-320))
#sendMessage(0,8,6)
#sendMessage(0,16,60)
