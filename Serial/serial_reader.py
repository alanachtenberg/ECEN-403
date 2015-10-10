import mmap
import os
import struct
import time
import json
import Queue
from BtComm import BtComm
from UdooSharedMem import UdooSharedMem
  
def unloadSerialData(buf, mem):
    return struct.unpack(mem["Type"], buf[mem["Offset"]:mem["Offset"]+mem["Size"]])
 
def fill_queue(buf, queue):
    ser0, = unloadSerialData(buf, UdooSharedMem['ser0'])
    ser1, = unloadSerialData(buf, UdooSharedMem['ser1'])
    ser2, = unloadSerialData(buf, UdooSharedMem['ser2'])
    ser3, = unloadSerialData(buf, UdooSharedMem['ser3'])
    queue.put(ser0)
    queue.put(ser1)
    queue.put(ser2)
    queue.put(ser3)    
    return queue

def fill_dict(q, rDict):
    while not q.empty():
        data = q.get()
        print(data)
        if data == 8:
            rDict['hdr'] = data
            rDict['val1'] = q.get()
            rDict['val2'] = q.get()
            rDict['ftr'] = q.get()
            break
        else:
            q.put(data)
    return rDict
	
def rx_print(d):
	print(d)
    
def main():
    # Open the file for reading
    fd = os.open('mmaptest', os.O_RDWR)
  
    # Memory map the file
    buf = mmap.mmap(fd, mmap.PAGESIZE)

    serial_queue = Queue.Queue()
    rDict = {}
    Dict = {}
	
	#btcomm = BtComm()
	#btcomm.start(rx_print)
	
  
    while 1:
        serial_queue = fill_queue(buf, serial_queue)
        Dict = fill_dict(serial_queue, rDict)

        print("q should be correct, let's check...")
        print(Dict)
        
        json_data = json.dumps(Dict)
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
