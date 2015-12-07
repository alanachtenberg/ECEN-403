# test sending results to server


# simple test of sending and recieving json over bluetooth

import logging
import json
import time
from BtComm import BtComm
from datetime import datetime


def parse_data(data):  # prints values from recieved over bluetooth socket
    logging.debug("Recieved message from phone: %s", data)


def generate_ecg_data():
    heart_rate = 100
    missed_beat = True
    low_volt_peak = True
    low_volt_val = 2.5
    type = "ecg"
    data = {"TYPE": type, "HEART_RATE": heart_rate, "MISSED_BEAT": missed_beat,
            "LOW_VOLT_PEAK": low_volt_peak, "LOW_VOLT_VAL": low_volt_val}
    json_data = json.dumps(data)
    return json_data


def generate_vehicle_data():  # generate sample data to send over bluetooth socket
    collision = False
    velocity = 45
    type = "vehicle"
    data = {"TYPE": type, "COLLISION": collision, "VELOCITY": velocity}

    json_data = json.dumps(data)
    return json_data


if __name__ == '__main__':
    logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.DEBUG)
    btcomm = BtComm()
    btcomm.start(parse_data)
    btcomm.send(generate_vehicle_data())
    time.sleep(5)
    btcomm.send(generate_ecg_data())
    time.sleep(30)  # Keep socket open to allow time for client to read
    print("main exit")
