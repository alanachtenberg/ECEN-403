#!/usr/bin/env python
import ctypes
import mmap
import os
import struct
import Queue
import serial
import time
import logging

ser = serial.Serial('/dev/ttyACM0', 115200) # create serial object, blocking reads
#ser = serial.Serial('/dev/ttymxc3', 115200) # create serial object, blocking reads
#ser = serial.Serial('/dev/ttymxc3', 115200, timeout = 0) # create serial object, non blocking reads
ser.flushOutput()
  
class UdooHighLevelGateway:
    def __init__(self):
        self.EcgHdr   = 0 # char
        self.BPM      = 0.0 # float
        self.MbFlag   = 0.0 # int
        self.LvpFlag  = 0.0 # int
        self.LvpValue = 0 # int
        self.EcgFtr   = 0 # char
        self.KineHdr     = 0 # char
        self.k_val1   = 0.0 # float
        self.KineFtr     = 0 # char
        self.KineFlag     = 0
        self.EcgFlag      = 0
        self.offset   = 0
 
    def initialize(self, buf):
        self.EcgHdr = ctypes.c_char.from_buffer(buf)
        self.offset = struct.calcsize(self.EcgHdr._type_)
        self.BPM = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.BPM._type_)
        self.MbFlag = ctypes.c_int.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.MbFlag._type_)
        self.LvpFlag = ctypes.c_byte.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.LvpFlag._type_)
        self.LvpValue = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.LvpValue._type_)
        self.EcgFtr = ctypes.c_char.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.EcgFtr._type_)
        self.KineHdr = ctypes.c_char.from_buffer(buf)
        self.offset = struct.calcsize(self.KineHdr._type_)
        self.k_val1 = ctypes.c_float.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.k_val1._type_)
        self.KineFtr = ctypes.c_char.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.KineFtr._type_)
        self.KineFlag = ctypes.c_byte.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.KineFlag._type_)
        self.EcgFlag = ctypes.c_byte.from_buffer(buf, self.offset)
        self.offset += struct.calcsize(self.EcgFlag._type_)
  

def fill_dict(serialValues, UdooGate, serialValueDict):
    while not serialValues.empty():
        data = serialValues.get()
        #print(data)
        #logging.debug("Header %f", data)
        if 'E' in data:
            serialValueDict['EcgHdr'] = 'e'
            serialValueDict['BPM'] = serialValues.get()
            serialValueDict['MbFlag'] = serialValues.get()
            serialValueDict['LvpFlag'] = serialValues.get()
            #LvpFlag = serialValues.get()
            #print('LvpFlag: ')
            #print(LvpFlag)
            serialValueDict['LvpValue'] = serialValues.get()
            serialValueDict['EcgFtr'] = 'f' 
            none = serialValues.get()
            #logging.debug("Value Dict")
            #logging.debug(serialValueDict)
            print(serialValueDict)
            
            try:
                UdooGate.EcgHdr.value = serialValueDict['EcgHdr']
                UdooGate.BPM.value = float(serialValueDict['BPM'])
                UdooGate.MbFlag.value = int(serialValueDict['MbFlag'])
                UdooGate.LvpFlag.value = int(serialValueDict['LvpFlag'])
                UdooGate.LvpValue.value = float(serialValueDict['LvpValue'])
                UdooGate.EcgFtr.value = serialValueDict['EcgFtr']
                UdooGate.EcgFlag.value = 1

            except ValueError:
                print('some error occured')
                pass

            break

        elif 'k' in data:        
            serialValueDict['KineHdr'] = 'k'
            serialValueDict['k_val1'] = serialValues.get()
            serialValueDict['KineFtr'] = 'f' 
            none = serialValues.get()
            #logging.debug("Value Dict")
            #logging.debug(serialValueDict)
            print(serialValueDict)

            try:
                UdooGate.KineHdr.value = serialValueDict['KineHdr']
                UdooGate.k_val1.value = float(serialValueDict['k_val1'])
                UdooGate.KineFtr.value = serialValueDict['KineFtr']
                UdooGate.KineFlag.value = 1

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
        sData0 = ser.readline()
        sData1 = ser.readline()
        sData2 = ser.readline()
        sData3 = ser.readline()
        sData4 = ser.readline()
        sData5 = ser.readline()

        serialValues.put(sData0)
        serialValues.put(sData1)
        serialValues.put(sData2)
        serialValues.put(sData3)
        serialValues.put(sData4)
        serialValues.put(sData5)

        fill_dict(serialValues, UdooGate, serialValueDict)
        
        
  
if __name__ == '__main__':
    main()
