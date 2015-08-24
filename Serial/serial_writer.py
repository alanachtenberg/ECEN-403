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
        self.pitch  = 0.0 # float
        self.yaw    = 0.0 # float
        self.Kp     = 0.0 # float
        self.Kd     = 0.0 # float
        self.offset = 0
 
    def initialize(self, buf):
        self.roll = ctypes.c_float.from_buffer(buf)
        self.offset = struct.calcsize(self.roll._type_)
        self.pitch = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.pitch._type_)
        self.yaw = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.yaw._type_)
        self.Kp = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.Kp._type_)
        self.Kd = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.Kd._type_)
  
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
  
    a = ser.readline()
    print(a)
    
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
    sData = ser.readline()
    quadGate.roll.value = float(sData)
  
  
if __name__ == '__main__':
    main()
