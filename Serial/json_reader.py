import json
import mmap
import os
import time

def main():

	# Open the file for reading
	#fd = os.fdopen('sharedmem', os.O_RDWR)

	# Memory map the file
	#buf = mmap.mmap(fd, mmap.PAGESIZE)

	dataFile1 = open('sharedmem1')

	while(1):
		
		#dataFile2 = open('sharedmem2')
		rDict1 = json.load(dataFile1)
		print(rDict1)
		#rDict2 = json.load(dataFile2)
		#print(rDict2)

		#dataFile1.close()
		#dataFile2.close()
		#time.sleep(20)


if __name__ == '__main__':
    main()
