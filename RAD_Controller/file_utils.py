from serial_utils import *
import time

# usage: sendFile( fileName )
# example: sendFile( 'patterns/SimpleBeat.csv' )
def sendFile(*args):

	# reset virtual clocks to 5 seconds before time 0, when the pattern will start
	sendMessage(16,0,500) #(pow(2,32)-64))
	time.sleep(2)

	# open and read the file data
	f =  open(args[0])
	content = f.readlines()
	content = [x.strip() for x in content]

	# parse and send schedule messages
	for line in content:
		list = line.split(',')
		if list[1]:
			drums = list[1].split('+')
                        d = 0
			for drum in drums:
				if drum == 'BASS':     # (1<<7) = 128
					d = d + 128
				elif drum == 'FLTOM':  # (1<<6) = 64
					d = d + 64
				elif drum == 'LOTOM':  # (1<<5) = 32
					d = d + 32
				elif drum == 'HITOM':  # (1<<4) = 16
					d = d + 16
				elif drum == 'SNARE':  # (1<<3) = 8
					d = d + 8
				elif drum == 'HIHAT':  # (1<<2) = 4
					d = d + 4
				elif drum == 'CYMBAL': # (1<<1) = 2
					d = d + 2
			sendMessage(8, d, int(list[0])) # 8=SCHDL message

def handleWebMessage(*args):
	msg = "".join(set(args[0]))
	d = 0
	for char in msg:
		# patterns
		if char == 'w':
			sendFile( 'patterns/SimpleBeat.csv' )
		elif char == 'x':
			sendFile( 'patterns/ComplexAttack.csv' )
		elif char == 'y':
			sendFile( 'patterns/EyeOfTheTiger.csv' )
		elif char == 'z':
			# set virtual clocks to a high number to skip everything in the FIFO queues
			sendMessage(16,0,20000)

		# realtime
		elif char == 'a':     # BASS
			d = d + 128 # (1<<7) = 128
		elif char == 'b':   # FLTOM
			d = d + 64  # (1<<6) = 64
		elif char == 'c':   # LOTOM
			d = d + 32  # (1<<5) = 32
		elif char == 'd':   # HITOM
			d = d + 16  # (1<<4) = 16
		elif char == 'e':   # SNARE
			d = d + 8   # (1<<3) = 8
		elif char == 'f':   # HIHAT
			d = d + 4   # (1<<2) = 4
		elif char == 'g':   # CYMBAL
			d = d + 2   # (1<<1) = 2

	# send play now message
	if d != 0:
		sendMessage(0,d,0)

