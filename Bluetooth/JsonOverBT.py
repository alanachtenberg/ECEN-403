# simple test of sending and recieving json over bluetooth

import logging
import json
import time
from BtComm import BtComm
from datetime import datetime


def parse_data(d):  # prints values from recieved over bluetooth socket, assumes reset and w_threshold keys exist
    obj_map = json.loads(d)
    reset = obj_map["reset"]
    w_threshold = obj_map["w_threshold"]
    print ("Reset: %s Warning Threshold: %d", reset, w_threshold)


def generate_data():  # generate sample data to send over bluetooth socket
    time = datetime.now()
    hour = time.strftime("%H")
    minute = time.strftime("%M")
    second = time.strftime("%S")
    accident = False
    velocity = 45
    relativeVelocity = 10
    warning = True
    heartrate = 78

    data = {"time": {"hour": hour, "minute": minute, "second": second}, "accident": accident, "velocity": velocity,
            "relativeVelocity": relativeVelocity, "warning": warning, "heartrate": heartrate}

    json_data = json.dumps(data)
    return json_data


logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.INFO)
btcomm = BtComm()
btcomm.start(parse_data)
btcomm.send(generate_data())
time.sleep(30)  # Keep socket open to allow time for client to read
