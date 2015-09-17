# simple test of sending and recieving json over bluetooth

import logging
import json
from BtComm import BtComm
from datetime import datetime


def parse_data(d):  # prints values from recieved over bluetooth socket, assumes reset and w_threshold keys exist
    obj_map = json.loads(d)
    reset = obj_map["reset"]
    w_threshold = obj_map["w_threshold"]
    print ("Reset: %s Warning Threshold: %d", reset, w_threshold)


def generate_data(): # generate sample data to send over bluetooth socket
    time = datetime.now().strftime("%H:%M:%S")
    user = "Hugh Jazz"
    accident = False
    warning = True
    heartrate = 78

    data = {"time": time, "user": user, "accident": accident, "warning": warning, "heartrate": heartrate}

    json_data = json.dumps(data)
    print json_data
    return json_data

logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.INFO)
btcomm = BtComm()
btcomm.start(parse_data)
btcomm.send(generate_data())
