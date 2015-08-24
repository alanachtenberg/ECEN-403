# Python module to send values over serial port
# /dev/ttymxc3 (Arduino Due)

import serial
import time

ser = serial.Serial('/dev/ttymxc3', '115200') # create serial object
ser.flushOutput()

def main():
	while(1):
    
		#print("Welcome to Serial Comm. Please select mode( 0 for write, 1 for read)")
		#msg = raw_input('Enter Mode: ')
    		#if (msg == "0"):
		#	ser.write("sup")
		#	ser.flush
		#	print('Message Sent')
		#	time.sleep(1)
		#	ser.write("homes")
		#	print('LED should now be off')
		#if (msg == "1"):
		#	a = ser.read(10)
   		#	print(a)
		#time.sleep(1)
		
    		#a = []

		
		a = ser.readline()
		print(a)


main()
