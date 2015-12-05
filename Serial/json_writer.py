#json writer

import json
import mmap
import os
import serial
import time

#ser = serial.Serial('/dev/ttyACM0', 115200) # create serial object, blocking reads
#ser = serial.Serial('/dev/ttymxc3', 115200) # create serial object, blocking reads
#ser.flushOutput()


def main():
	# Create new empty file to back memory map on disk
    #fd = os.fdopen('sharedmem', os.O_CREAT | os.O_TRUNC | os.O_RDWR)

	# Zero out the file to insure it's the right size
    #assert os.write(fd, '\x00' * mmap.PAGESIZE) == mmap.PAGESIZE

	# Create the mmap instance with the following params:
    # fd: File descriptor which backs the mapping or -1 for anonymous mapping
    # length: Must in multiples of PAGESIZE (usually 4 KB)
    # flags: MAP_SHARED means other processes can share this mmap
    # prot: PROT_WRITE means this process can write to this mmap
    #buf = mmap.mmap(fd, mmap.PAGESIZE)

	#ecgObject = open('sharedmem1', 'w')
	#kineObject = open('sharedmem2', 'w')

	serialValueDict = {}
	serialValueDict2 = {}

	serialValueDict['Fuck'] = 'Bitches'
	serialValueDict['Get'] = 'Money'

	serialValueDict2['Get'] = 'Rich'
	serialValueDict2['Die'] = 'Trying'

	ecgObject = open('sharedmem1', 'w')

	#i = 0
	

	while(1):
		
		json.dump([serialValueDict, serialValueDict2], ecgObject)
		#ecgObject.close()
		#time.sleep(2000)
		#i = i + 1
		#if (i == 100):
			#serialValueDict['Fuck'] = 'Hoes'
			#serialValueDict['Get'] = 'Paid'
		#kineObject = open('sharedmem1', 'w')
		#json.dump(serialValueDict2, ecgObject)
		#ecgObject.close()
		#time.sleep(2000)





if __name__ == '__main__':
    main()

