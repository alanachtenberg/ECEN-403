#!/usr/bin/env python
import ctypes
import mmap
import os
import struct

import serial
import time

ser = serial.Serial('/dev/ttyACM0', '115200') # create serial object
ser.flushOutput()
  
class QuadHighLevelGateway:
    def __init__(self):
        self.roll   = 0.0 # float
        self.ser1   = 0.0 # float
        self.ser2   = 0.0 # float
        self.ser3   = 0.0 # float
        self.ser4   = 0.0 # float
	self.ser5   = 0.0 # serial port data
        self.offset = 0
 
    def initialize(self, buf):
        self.roll = ctypes.c_float.from_buffer(buf)
        self.offset = struct.calcsize(self.roll._type_)
        self.ser1 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ser1._type_)
        self.ser2 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ser2._type_)
        self.ser3 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ser3._type_)
        self.ser4 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ser4._type_)
	self.ser5 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ser5._type_)
  
def main():
    # Create new empty file to back memory map on disk
    fd = os.open('mmaptest', os.O_CREAT | os.O_TRUNC | os.O_RDWR)
  
    # Zero out the file to insure it's the right size
    assert os.write(fd, '\x00' * mmap.PAGESIZE) == mmap.PAGESIZE
  
    # Create the mmap instace with the following params:
    # fd: File descriptor which backs the mapping or -1 for anonymous mapping
    # length: Must in multiples of PAGESIZE (usually 4 KB)
    # flags: MAP_SHARED means other processes can share this mmap
    # prot: PROT_WRITE means this process can write to this mmap
    buf = mmap.mmap(fd, mmap.PAGESIZE)
  
    sData1 = ser.readline()
    sData2 = ser.readline()
    sData3 = ser.readline()
    sData4 = ser.readline()
    quadGate = QuadHighLevelGateway()
    quadGate.initialize(buf)
  
    print 'First 20 bytes of memory mapping: %r' % buf[:20]
    raw_input('Now run b.py and press ENTER')
  
    print
    print 'Changing i'
    quadGate.roll.value = 0.1
  
    print 'Changing s'
    quadGate.roll.value = 5.2
  
    new_i = raw_input('Enter a new value for i: ')
    quadGate.roll.value = float(new_i)
    quadGate.ser1.value = float(sData1)
    quadGate.ser2.value = float(sData2)
    quadGate.ser3.value = float(sData3)
    quadGate.ser4.value = float(sData4)

  
  
if __name__ == '__main__':
    main()
