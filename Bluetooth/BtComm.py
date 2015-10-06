
import logging
from bluetooth import *
from threading import Thread

__author__ = 'alan'

class BtComm:

    NAME = "UDOO"
    UUID = "00001101-0000-1000-8000-00805F9B34FB"
    RF_PORT = 3

    def __init__(self):
        self.sock = BluetoothSocket(RFCOMM)
        self.sock.bind(("",BtComm.RF_PORT))
        self.sock.listen(1)
        self.port = self.sock.getsockname()[1]
        self.client_sock = None
        self.client_info = None
        self.parse_function = None

        advertise_service(self.sock, BtComm.NAME, service_id=BtComm.UUID)

    def start(self, callback):
        """Accepts a connection from bluetooth socket and starts a thread which continuously reads
        Writing to socket must be done after it is started.
        Keyword arguments:
        callback -- function with 1 param that will parse the input of the client socket in receive_thread
        """
        self.parse_function = callback
        logging.info("Waiting for connection on RFCOMM channel %d" % self.port);
        self.client_sock, self.client_info = self.sock.accept()
        logging.info("Accepted connection from %s %s", self.client_info[0], self.client_info[1])
        Thread(target=self.receive_thread)

    def receive_thread(self):
        try:
            while True:
                data = self.client_sock.recv(1024)
                logging.info("input data received [%s]" % data)
                self.parse_function(data)
        except IOError:
            logging.error("IOError Bluetooth Socket Receive")
            pass

    def send(self, data):
        logging.info("Sending Over Bluetooth %s",data)
        self.client_sock.send(data+"\n")#add new line so client can use readln on stream

