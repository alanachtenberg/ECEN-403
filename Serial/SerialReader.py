import mmap
import os
import struct
import time
import json
import Queue
from BtComm import BtComm
from UdooSharedMem import UdooSharedMem


class SerialReader:
    HEADER_VALUE = 8
    VALUE_NAMES = ("ser0", "ser1", "ser2", "ser3")
    STRUCTURE_FORMAT = ("hdr", "val1", "val2", "ftr")

    def __init__(self, file_name):
        self.fd = os.open(file_name, os.O_RDWR)
        self.mappedFile = mmap.mmap(fd, mmap.PAGESIZE)
        self.serialValues = Queue.Queue()
        self.serialValueDict = {}
        self.callback = None # callback function for handling received data

    def unloadSerialData(self, mem):
        struct.unpack(mem["Type"], self.mappedFile[mem["Offset"]:mem["Offset"] + mem["Size"]])

    def fill_queue(self):
        for name in VALUE_NAMES:
            serial_value, = unloadSerialData(self.mappedFile, UdooSharedMem[name])
            self.serialValues.put(serial_value)
        logging.debug("Serial Values ", self.serialValues)

    def fill_dict(self):
        while not self.serialValues.empty():
            data = self.serialValues.get()
            logging.debug(data)
            if data == HEADER_VALUE:
                self.serialValueDict['hdr'] = data
                self.serialValueDict['val1'] = self.serialValues.get()
                self.serialValueDict['val2'] = self.serialValues.get()
                self.serialValueDict['ftr'] = self.serialValues.get()
                logging.debug("Sorted Value Dict ", self.serialValueDict)
                break
            else:
                self.serialValues.put(data)

    def start(self, callback):
        """
        Keyword arguments:
        callback -- function with 1 param that will handle recieved data
        """
        self.callback = callback
        Thread(target=self.read_thread, args=function)

    def read_thread(self):
        while 1:
            fill_queue()  # read values from mapped memory
            fill_dict() # sort values and fill dictionary
            self.callback(self.serialValueDict)

            ## Delete fd when done?

    if __name__ == '__main__':
        logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.INFO)
        main()
