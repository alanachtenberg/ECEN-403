import logging
from threading import Thread
import bluetooth as BT

__author__ = 'alan'


class BtComm:
    NAME = "UDOO"
    UUID = "00001101-0000-1000-8000-00805F9B34FB"
    RF_PORT = 3

    def __init__(self):
        self.sock = BT.BluetoothSocket(BT.RFCOMM)
        self.sock.bind(BtComm.RF_PORT)
        self.sock.listen(1)
        self.port = self.sock.getsockname()[1]
        self.client_sock, self.client_info = None, None
        self.parse_function = None

        BT.advertise_service(self.sock, BtComm.NAME, service_id=BtComm.UUID,
                             service_classes=[BT.uuid, BT.SERIAL_PORT_CLASS],
                             profiles=[BT.SERIAL_PORT_PROFILE])

    def start(self, callback):
        """Accepts a connection from bluetooth socket and starts a thread which continuously reads
        Writing to socket must be done after it is started.
        Keyword arguments:
        callback -- function with 1 param that will parse the input of the client socket in receive_thread
        """
        self.parse_function = callback
        logging.info("Waiting for connection on RFCOMM channel %d" % self.port);
        self.client_sock, self.client_info = self.sock.accept()
        logging.info("Accepted connection from ", self.client_info)
        Thread(target=self.receive_thread, args=function)

    def receive_thread(self):
        try:
            while True:
                data = self.client_sock.recv(1024)
                logging.info("input data received [%s]" % data)
                self.parse_function(data)
        except IOError:
            logging.error("Bluetooth Socket Receive")
            pass

    def send(self, data):
        self.client_sock.send(data)

    def __del__(self):
        self.client_sock.close()
        self.sock.close()


def test(text):
    logging.info(text)

if __name__ == "__main__":
    btcomm = BtComm()
    btcomm.start(test)
    btcomm.send("Hello I am a UDOO\n")