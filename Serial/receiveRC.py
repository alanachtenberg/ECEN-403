# Python module to send values over serial port
# /dev/ttyACM0 (Arduino Due on linux)

import serial
import time

ser = serial.Serial('/dev/ttyACM0', '115200') # create serial object
ser.flushOutput()

def main():
	while(1):
    

		a = ser.readline()
		print(a)


if __name__ == '__main__':
    main()
