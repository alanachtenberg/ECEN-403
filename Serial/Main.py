__author__ = 'alan'
import logging
import BtComm
import SerialReader

class Main:
    def bluetoothHandler(self, data):
        print(data)


    def serialHandler(self, data):
        btComm.send(data)


if __name__ == '__main__':
    logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.INFO)

    btComm = BtComm.BtComm()
    #btComm.start(Main.bluetoothHandler)

    serialReader = SerialReader.SerialReader("mmaptest")
    serialReader.start(Main.serialHandler)

    while True:
        # do facial
        print("doing facial")
