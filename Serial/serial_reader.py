import mmap
import os
import struct
import time
from UdooSharedMem import UdooSharedMem
  
def unloadSerialData(buf, mem):
    return struct.unpack(mem["Type"], buf[mem["Offset"]:mem["Offset"]+mem["Size"]])
 
def main():
    # Open the file for reading
    fd = os.open('mmaptest', os.O_RDWR)
  
    # Memory map the file
    buf = mmap.mmap(fd, mmap.PAGESIZE)
  
    i = None
    ##s = None
  
    while 1:
        #new_i, = struct.unpack('f', buf[:4])
        #new_s, = struct.unpack('f', buf[4:8])
  
        ser0, = unloadSerialData(buf, UdooSharedMem['ser0'])
	ser1, = unloadSerialData(buf, UdooSharedMem['ser1'])
	ser2, = unloadSerialData(buf, UdooSharedMem['ser2'])
	ser3, = unloadSerialData(buf, UdooSharedMem['ser3'])
	ser4, = unloadSerialData(buf, UdooSharedMem['ser4'])
 
        if i != ser0:
            print 'i: %s => %f' % (i, ser0)
	    print 'ser1: %s' % (ser1)
	    print 'ser2: %s' % (ser2)
	    print 'ser3: %s' % (ser3)
	    print 'ser4: %s' % (ser4)
            print 'Press Ctrl-C to exit'
            i = ser0
  
        time.sleep(1) 
  ## Delete fd when done
  
if __name__ == '__main__':
    main()
