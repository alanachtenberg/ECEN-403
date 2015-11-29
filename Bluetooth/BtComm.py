import logging
from bluetooth import *
from threading import Thread

__author__ = 'alan'


class BtComm:
    NAME = "UDOO"
    UUID = "1afe39b3-2c5c-4bf4-a2c2-267ee767fd9d"
    RF_PORT = 3

    def __init__(self):
        self.thread = None
        self.sock = BluetoothSocket(RFCOMM)
        self.sock.bind(("", BtComm.RF_PORT))
        self.sock.listen(1)
        self.port = self.sock.getsockname()[1]
        self.client_sock = None
        self.client_info = None
        self.callback = None
        advertise_service(self.sock, BtComm.NAME, service_id=BtComm.UUID)

    @staticmethod
    def set_device_as_discoverable():
        if not os.geteuid() == 0:
            sys.exit("\nYou must be root to run this application, please use sudo and try again.\n")
        os.system("sudo hciconfig hci0 piscan")  # sets device as discoverable

    def start(self, parse_function):
        """Accepts a connection from bluetooth socket and starts a thread which continuously reads
        Writing to socket must be done after it is started.
        :param parse_function: function with 1 param that will parse the input of the client socket in receive_thread
        """
        self.callback = parse_function
        logging.info("Waiting for connection on RFCOMM channel %d" % self.port);
        self.client_sock, self.client_info = self.sock.accept()
        logging.info("Accepted connection from %s %s", self.client_info[0], self.client_info[1])
        Thread(target=self.receive_thread).start()

    def receive_thread(self):
        try:
            while True:
                data = self.client_sock.recv(1024)
                logging.info("input data received [%s]" % data)
                if data == "CLOSING CONNECTION":
                    logging.info("Client closed connection receive_thread exiting")
                    self.close()
                    return
                self.callback(data)
        except IOError:
            logging.error("IOError Bluetooth Socket recieve_thread")
            pass

    def close(self):
        self.sock.close()
        logging.info("Closed socket")

    def send(self, data):
        logging.info("Sending Over Bluetooth %s", data)
        self.client_sock.send(data + "\n")  # add new line so client can use readln on stream
