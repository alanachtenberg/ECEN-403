# simple test of sending and recieving json over bluetooth

import logging
import json
import time
from BtComm import BtComm
from datetime import datetime


def parse_data(data):  # prints values from recieved over bluetooth socket
    logging.debug("Recieved message from phone: %s", data)


def generate_data():  # generate sample data to send over bluetooth socket
    time = datetime.now()
    hour = time.strftime("%H")
    minute = time.strftime("%M")
    second = time.strftime("%S")
    accident = False
    velocity = 45
    relative_velocity = 10
    warning = True
    heartrate = 78

    data = {"time": {"hour": hour, "minute": minute, "second": second}, "accident": accident, "velocity": velocity,
            "relativeVelocity": relative_velocity, "warning": warning, "heartrate": heartrate}

    json_data = json.dumps(data)
    return json_data


if __name__ == '__main__':
    logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.DEBUG)
    BtComm.DEBUG_MODE = True  # let class know not to call hciconfig which requires sudo priveleges
    btcomm = BtComm()
    btcomm.start(parse_data)
    btcomm.send(generate_data())
    time.sleep(30)  # Keep socket open to allow time for client to read
    print("main exit")
