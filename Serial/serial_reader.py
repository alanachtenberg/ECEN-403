import logging

import mmap
import os
import struct
import time
import json
import Queue
#from BtComm import BtComm
from UdooSharedMem import UdooSharedMem
  
def unloadSerialData(buf, mem):
    return struct.unpack(mem["Type"], buf[mem["Offset"]:mem["Offset"]+mem["Size"]])
 
def fill_dict(buf, rDict):
    rDict['hdr'] = unloadSerialData(buf, UdooSharedMem['ser0'])
    rDict['val1'] = unloadSerialData(buf, UdooSharedMem['ser1'])
    rDict['val2'] = unloadSerialData(buf, UdooSharedMem['ser2'])
    rDict['val3'] = unloadSerialData(buf, UdooSharedMem['ser3'])
    rDict['ftr'] = unloadSerialData(buf, UdooSharedMem['ser4'])
    return rDict

def rx_print(d):
	print(d)
    
def main():
    logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.INFO)
    # Open the file for reading
    fd = os.open('mmaptest', os.O_RDWR)
  
    # Memory map the file
    buf = mmap.mmap(fd, mmap.PAGESIZE)

    rDict = {}

    #btcomm = BtComm()
    #btcomm.start(rx_print)
	
  
    while 1:
        rDict = fill_dict(buf, rDict)


        print("rDict should be correct, let's check...")
        print(rDict)
        
        json_data = json.dumps(rDict)
        #print json_data
        print('send BtComm!')
        ## send BT comm here
        #btcomm.send(json_data)

        
        #ser0, = unloadSerialData(buf, UdooSharedMem['ser0'])
        #ser1, = unloadSerialData(buf, UdooSharedMem['ser1'])
        #ser2, = unloadSerialData(buf, UdooSharedMem['ser2'])
        #ser3, = unloadSerialData(buf, UdooSharedMem['ser3'])
        #ser4, = unloadSerialData(buf, UdooSharedMem['ser4'])
 
        #print 'ser0: %s' % (ser0)
        #print 'ser1: %s' % (ser1)
        #print 'ser2: %s' % (ser2)
        #print 'ser3: %s' % (ser3)
        #print 'ser4: %s' % (ser4)
        print 'Press Ctrl-C to exit'

  ## Delete fd when done?
  
if __name__ == '__main__':
    main()
