import mmap
import os
import struct
import time
import json
import Queue
from threading import Thread
#from BtComm import BtComm
from UdooSharedMem import UdooSharedMem

import logging


class SerialReader:
    HEADER_VALUE = 5
    VALUE_NAMES = ("ser0", "ser1", "ser2", "ser3", "ser4")
    STRUCTURE_FORMAT = ("hdr", "val1", "val2", "val3", "ftr")

    def __init__(self, file_name):
        self.fd = os.open(file_name, os.O_RDWR)
        self.mappedFile = mmap.mmap(self.fd, mmap.PAGESIZE)
        self.serialValues = Queue.Queue()
        self.serialValueDict = {}
        self.callback = None  # callback function for handling received data

    def unloadSerialData(self, mem):
        return struct.unpack(mem["Type"], self.mappedFile[mem["Offset"]:mem["Offset"] + mem["Size"]])

    def fill_queue(self):
        logging.debug("Serial Values")
        for name in SerialReader.VALUE_NAMES:
            serial_value, = self.unloadSerialData(UdooSharedMem[name])
            logging.debug(serial_value)
            self.serialValues.put(serial_value)

    def fill_dict(self):
        while not self.serialValues.empty():
            data = self.serialValues.get()
            logging.debug("Header %f", data)
            if data == SerialReader.HEADER_VALUE:
                self.serialValueDict['hdr'] = data
                self.serialValueDict['val1'] = self.serialValues.get()
                self.serialValueDict['val2'] = self.serialValues.get()
                self.serialValueDict['val3'] = self.serialValues.get()
                self.serialValueDict['ftr'] = self.serialValues.get()
                logging.debug("Value Dict")
                logging.debug(self.serialValueDict)
                break
            else:
                self.serialValues.put(data)

    def start(self, callback):
        """
        Keyword arguments:
        callback -- function with 1 param that will handle received data
        """
        self.callback = callback
        Thread(target=self.read_thread).start()

    def read_thread(self):
        while 1:
            self.fill_queue()  # read values from mapped memory
            self.fill_dict()  # sort values and fill dictionary
            self.callback(self.serialValueDict)

            ## Delete fd when done?


def serialHandler(data):
    logging.debug("Handler received data")
    logging.debug(data)


if __name__ == '__main__':
    logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.DEBUG)
    serialReader = SerialReader("mmaptest")
    serialReader.start(serialHandler)
    while True:
        time.sleep(1)
