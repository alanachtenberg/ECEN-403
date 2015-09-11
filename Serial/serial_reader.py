import mmap
import os
import struct
import time
from QuadSharedMem import QuadSharedMem
  
def unloadQuadData(buf, mem):
    return struct.unpack(mem["Type"], buf[mem["Offset"]:mem["Offset"]+mem["Size"]])
 
def main():
    # Open the file for reading
    fd = os.open('mmaptest', os.O_RDWR)
  
    # Memory map the file
    buf = mmap.mmap(fd, mmap.PAGESIZE)
  
    i = None
    s = None
  
    while 1:
        #new_i, = struct.unpack('f', buf[:4])
        #new_s, = struct.unpack('f', buf[4:8])
  
        roll, = unloadQuadData(buf, QuadSharedMem['Roll'])
	ser1, = unloadQuadData(buf, QuadSharedMem['ser1'])
	ser2, = unloadQuadData(buf, QuadSharedMem['ser2'])
	ser3, = unloadQuadData(buf, QuadSharedMem['ser3'])
	ser4, = unloadQuadData(buf, QuadSharedMem['ser4'])
 
        if i != roll:
            print 'i: %s => %f' % (i, roll)
	    print 'ser1: %s' % (ser1)
	    print 'ser2: %s' % (ser2)
	    print 'ser3: %s' % (ser3)
	    print 'ser4: %s' % (ser4)
            print 'Press Ctrl-C to exit'
            i = roll
  
        time.sleep(1)
  
  
if __name__ == '__main__':
    main()
