FILES:
timeout.py - contains functions for timeout if functions take too long
serial_utils.py - contains functions for using the serial port
test.py - an example program that uses the serial port

SETUP:
> sudo apt-get install python-serial
> sudo raspi-config
	-> Interfacing Options -> Serial -> disable serial login, enable serial port
	-> Interfacing Options -> SSH -> enable

RUN TEST:
connect the RX and TX pins on the Pi together (in the outer row, TX is pin 4 and RX is pin 5 from the corner side)
> python test.py
program should print that the serial port is working