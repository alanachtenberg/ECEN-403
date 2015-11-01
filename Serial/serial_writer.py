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
        self.hdr   = 0 # char
        self.val1   = 0.0 # float
        self.val2   = 0.0 # float
        self.val3   = 0.0 # float
        self.ftr   = 0 # char
        self.hdr2   = 0 # char
        self.k_val1   = 0.0 # float
        self.ftr2   = 0 # char
        self.kine = 0
        self.ecg = 0
        self.offset = 0
 
    def initialize(self, buf):
        self.hdr = ctypes.c_char.from_buffer(buf)
        self.offset = struct.calcsize(self.hdr._type_)
        self.val1 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.val1._type_)
        self.val2 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.val2._type_)
        self.val3 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.val3._type_)
        self.ftr = ctypes.c_char.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ftr._type_)
        self.hdr2 = ctypes.c_char.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.hdr2._type_)
        self.k_val1 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.k_val1._type_)
        self.ftr2 = ctypes.c_char.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ftr2._type_)
        self.kine = ctypes.c_char.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.kine._type_)
        self.ecg = ctypes.c_char.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.ecg._type_)
  

def fill_dict(serialValues, UdooGate, serialValueDict):
    while not serialValues.empty():
        data = serialValues.get()
        logging.debug("Header %f", data)
        if data == 'e':
            serialValueDict['hdr'] = data
            serialValueDict['val1'] = serialValues.get()
            serialValueDict['val2'] = serialValues.get()
            serialValueDict['val3'] = serialValues.get()
            serialValueDict['ftr'] = serialValues.get()
            #print(serialValueDict)            
            logging.debug("Value Dict")
            logging.debug(serialValueDict)
            try:
                UdooGate.hdr.value = serialValueDict['hdr']
                UdooGate.val1.value = float(serialValueDict['val1'])
                UdooGate.val2.value = float(serialValueDict['val2'])
                UdooGate.val3.value = float(serialValueDict['val3'])
                UdooGate.ftr.value = serialValueDict['ftr']
                UdooGate.kine.value = '1'

            except ValueError:
                pass

            break

        elif data == 'k':        
            serialValueDict['hdr2'] = data
            serialValueDict['k_val1'] = serialValues.get()
            serialValueDict['ftr2'] = serialValues.get()
            logging.debug("Value Dict")
            logging.debug(serialValueDict)
            print(serialValueDict)
            try:
                UdooGate.hdr2.value = serialValueDict['hdr2']
                UdooGate.k_val1.value = float(serialValueDict['k_val1'])
                UdooGate.ftr2.value = serialValueDict['ftr2']
                UdooGate.ecg.value = '1'

            except ValueError:
                pass

            break

        else:
            serialValues.put(data)



def main():
    logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.INFO)    
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
		
        #sData0 = 11
        #serialValues.put(sData0)
        #sData1 = 14
        #serialValues.put(sData1)
        #sData2 = 'f'
        #serialValues.put(sData2)
        #sData3 = 'E'
        #serialValues.put(sData3)
        #sData4 = 8
        #serialValues.put(sData4)


        sData0 = 9
        serialValues.put(sData0)
        sData1 = 'f'
        serialValues.put(sData1)
        sData2 = 'k'
        serialValues.put(sData2)
		
        #print(sData0, sData1, sData2, sData3, sData4)

        fill_dict(serialValues, UdooGate, serialValueDict)
        
        
  
if __name__ == '__main__':
    main()
