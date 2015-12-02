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
    if (unloadSerialData(buf, UdooSharedMem['KineFlag'])[0] == '1'):    
        rDict['KineHdr'] = unloadSerialData(buf, UdooSharedMem['KineHdr'])[0]
        rDict['k_val1'] = unloadSerialData(buf, UdooSharedMem['k_val1'])[0]
        rDict['KineFtr'] = unloadSerialData(buf, UdooSharedMem['KineFtr'])[0]
        rDict['MsgType'] = '1'  # 1 for kine message
        #struct.pack_into(c, buf, 20, '0') # pack '0' as a char in buf at offset 20
        print('here')
        # set KineFlag to 0
        #struct.pack_into(mem["Type"]
    elif (unloadSerialData(buf, UdooSharedMem['EcgFlag'])[0] == '1'):
        rDict['EcgHdr'] = unloadSerialData(buf, UdooSharedMem['EcgHdr'])
        rDict['BPM'] = unloadSerialData(buf, UdooSharedMem['BPM'])
        rDict['MbFlag'] = unloadSerialData(buf, UdooSharedMem['MbFlag'])
        rDict['LvpFlag'] = unloadSerialData(buf, UdooSharedMem['LvpFlag'])
        rDict['LvpValue'] = unloadSerialData(buf, UdooSharedMem['LvpValue'])
        rDict['EcgFtr'] = unloadSerialData(buf, UdooSharedMem['EcgFtr'])
        rDict['MsgType'] = '0'  # 0 for ecg message
        print(rDict)
        #struct.pack_into(c, buf, 21, '0') # pack '0' as a char in buf at offset 21
        # set EcgFlag to 0 struct.pack_into() I think...
    else:
        val = unloadSerialData(buf, UdooSharedMem['EcgFlag'])
        print(val)
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


        #print("rDict should be correct, let's check...")
        #print(rDict)
        
        json_data = json.dumps(rDict)
        #print json_data
        print('send BtComm!')
        ## send BT comm here
        #btcomm.send(json_data)

        print 'Press Ctrl-C to exit'

  ## Delete fd when done?
  
if __name__ == '__main__':
    main()
