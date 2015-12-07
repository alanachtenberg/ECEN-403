from lib.FaceDetector import FaceDetector
from lib.BtComm import BtComm
import logging
import time
import os
import Queue
import serial
import gpio

last_face_value = False
last_eyes_value = False

rootPath = "/sys/class/gpio/"

output_pin_1 = rootPath + "gpio55/"
output_pin_2 = rootPath + "gpio57/"


def bluetooth_handler(message):
    logging.info("Recieved Bluetooth Message")
    logging.info(message)


def face_detection_handler(face, eyes):
    global last_face_value
    global last_eyes_value
    if face is not last_face_value:
        last_face_value = face
        logging.debug("Writing Face: %s", face)
        if face:
            gpio.writePinValue(output_pin_1, "1")
        else:
            gpio.writePinValue(output_pin_1, "0")
    if eyes is not last_eyes_value:
        last_eyes_value = eyes
        logging.debug("Writing Eyes: %s", face)
        if eyes:
            gpio.writePinValue(output_pin_2, "1")
        else:
            gpio.writePinValue(output_pin_2, "0")


def fill_dict(serialValues, UdooGate, serialValueDict):
    while not serialValues.empty():
        data = serialValues.get()
        if data == 'E':
            serialValueDict['TYPE'] = "ECG"
            serialValueDict['EcgHdr'] = data
            serialValueDict['HEART_RATE'] = serialValues.get()
            serialValueDict['MISSED_BEAT'] = serialValues.get()
            serialValueDict['LOW_VOLT_PEAK'] = serialValues.get()
            serialValueDict['LOW_VOLT_VAL'] = serialValues.get()
            serialValueDict['EcgFtr'] = serialValues.get()
            print(serialValueDict)

            break

        elif data == 'k':
            serialValueDict['TYPE'] = "VEHICLE"
            serialValueDict['KineHdr'] = data
            serialValueDict['COLLISION'] = serialValues.get()
            serialValueDict['VELOCITY'] = serialValues.get()
            serialValueDict['zero1'] = serialValues.get()
            serialValueDict['zero2'] = serialValues.get()
            serialValueDict['KineFtr'] = serialValues.get()
            print(serialValueDict)
            break

        else:
            serialValues.put(data)





if __name__ == '__main__':
    logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.DEBUG)
    gpio.init_gpio(output_pin_1,output_pin_2)
    face_detector = FaceDetector(0)
    face_detector.start(face_detection_handler)

    btComm = BtComm()
    btComm.start(bluetooth_handler)

    ser = serial.Serial('/dev/ttyACM0', 115200)
    ser.flushOutput()

    serialValues = Queue.Queue()
    serialValueDict = {}
    while 1:
        sData0 = ser.readline()  # blocking call
        sData1 = ser.readline()
        sData2 = ser.readline()
        sData3 = ser.readline()
        sData4 = ser.readline()
        sData5 = ser.readline()

        serialValues.put(sData0)
        serialValues.put(sData1)
        serialValues.put(sData2)
        serialValues.put(sData3)
        serialValues.put(sData4)
        serialValues.put(sData5)

        fill_dict(serialValues, UdooGate, serialValueDict)
        btComm.send(serialValueDict)

    print("main exit")
    sys.exit()

