#!/usr/bin/env python
import ctypes
import mmap
import os
import struct
import Queue
import serial
import time
import logging


#ser = serial.Serial('/dev/ttymxc3', 115200) # create serial object, blocking reads
#ser = serial.Serial('/dev/ttymxc3', 115200, timeout = 0) # create serial object, non blocking reads
#ser.flushOutput()
  
class UdooHighLevelGateway:
    def __init__(self):
        self.ser0   = 0.0 # float
        self.ser1   = 0.0 # float
        self.ser2   = 0.0 # float
        self.ser3   = 0.0 # float
        self.ser4   = 0.0 # string
        #self.ser5   = 0.0 # string
        self.offset = 0
 
    def initialize(self, buf):
        self.ser0 = ctypes.c_float.from_buffer(buf)
        self.offset = struct.calcsize(self.ser0._type_)
        self.ser1 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ser1._type_)
        self.ser2 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ser2._type_)
        self.ser3 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ser3._type_)
        self.ser4 = ctypes.c_char.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ser4._type_)
        #self.ser5 = ctypes.c_char.from_buffer(buf, self.offset)
        #self.offset += struct.calcsize(self.ser5._type_)
  

def fill_dict(serialValues, serialValueDict):
    while not serialValues.empty():
        data = serialValues.get()
        logging.debug("Header %f", data)
        if data == 5:
            serialValueDict['hdr'] = data
            serialValueDict['val1'] = serialValues.get()
            serialValueDict['val2'] = serialValues.get()
            serialValueDict['val3'] = serialValues.get()
            serialValueDict['ftr'] = serialValues.get()
            logging.debug("Value Dict")
            logging.debug(serialValueDict)
            break
        else:
            serialValues.put(data)
    return serialValueDict


def main():
    # Create new empty file to back memory map on disk
    fd = os.open('mmaptest', os.O_CREAT | os.O_TRUNC | os.O_RDWR)
  
    # Zero out the file to insure it's the right size
    assert os.write(fd, '\x00' * mmap.PAGESIZE) == mmap.PAGESIZE
  
    # Create the mmap instance with the following params:
    # fd: File descriptor which backs the mapping or -1 for anonymous mapping
    # length: Must in multiples of PAGESIZE (usually 4 KB)
    # flags: MAP_SHARED means other processes can share this mmap
    # prot: PROT_WRITE means this process can write to this mmap
    buf = mmap.mmap(fd, mmap.PAGESIZE)
  
    UdooGate = UdooHighLevelGateway()
    UdooGate.initialize(buf)

    serialValues = Queue.Queue()
    serialValueDict = {}
  
    while 1:
        #sData0 = ser.readline()
        #sData1 = ser.readline()
        #sData2 = ser.readline()
        #sData3 = ser.readline()
	    #sData4 = ser.readline()
		
        sData0 = 11
        serialValues.put(sData0)
        sData1 = 14
        serialValues.put(sData1)
        sData2 = 's'
        serialValues.put(sData2)
        sData3 = 5
        serialValues.put(sData3)
        sData4 = 8
        serialValues.put(sData4)
		
        #print(sData0, sData1, sData2, sData3, sData4)

        serialValueDict = fill_dict(serialValues, serialValueDict)
        

        try:
            UdooGate.ser0.value = float(serialValueDict['hdr'])
            UdooGate.ser1.value = float(serialValueDict['val1'])
            UdooGate.ser2.value = float(serialValueDict['val2'])
            UdooGate.ser3.value = float(serialValueDict['val3'])
            UdooGate.ser4.value = serialValueDict['ftr']

        except ValueError:
            pass


  
if __name__ == '__main__':
    main()
