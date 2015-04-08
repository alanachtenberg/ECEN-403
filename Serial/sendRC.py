# Python code to send Attitude and Throttle Values over serial port
# /dev/ttyACM0 (ArduPilot)

import serial

#ser = serial.Serial('/dev/ttyACM0', '57600') # create serial object

def main():
  while(1):
    
    #thr = 1500 # might need to convert to string
    #roll = 1500
    #pitch = 1500
    #yaw = 1500
  
    thr = raw_input('Enter Throttle Value: ')
    #roll = raw_input('Enter Roll Value: ')
    pitch = raw_input('Enter Pitch Value: ')
    #yaw = raw_input('Enter Yaw Value: ')

    autoRC = [thr, pitch]
  
    ser.write(autoRC)
    print('Values Sent')
    ser.flush


main()
