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
 
        if i != roll:
            print 'i: %s => %f' % (i, roll)
            print 'Press Ctrl-C to exit'
            i = roll
  
        time.sleep(1)
  
  
if __name__ == '__main__':
    main()
