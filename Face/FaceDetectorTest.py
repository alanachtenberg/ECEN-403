from FaceDetector import FaceDetector
import logging
import time
import os

last_face_value = False
last_eyes_value = False

rootPath = "/sys/class/gpio/"

output_pin_1 = rootPath +"gpio55/"
output_pin_2 = rootPath +"gpio57/"


def face_detection_handler(face, eyes):
    global last_face_value
    global last_eyes_value
    if face is not last_face_value:
        last_face_value = face
        logging.debug("Writing Face: %s", face)
        if face:
            writePinValue(output_pin_1, "1")
        else:
            writePinValue(output_pin_1, "0")
    if eyes is not last_eyes_value:
        last_eyes_value = eyes
        logging.debug("Writing Eyes: %s", face)
        if eyes:
            writePinValue(output_pin_2, "1")
        else:
            writePinValue(output_pin_2, "0")

def writePinValue(pin , pinValue):
	os.system("echo "+pinValue+" > "+pin+"value")
	
def initGPIO():
	logging.info("Initializing GPIO pins: "+output_pin_1+" "+output_pin_2)
	os.system("echo out > "+output_pin_1+ "direction")
	os.system("echo out > "+output_pin_2+ "direction")
	writePinValue(output_pin_1,"0")
	writePinValue(output_pin_2,"0")
	
if __name__ == '__main__':
    logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.DEBUG)
    initGPIO()
    face_detector = FaceDetector(3)
    face_detector.start(face_detection_handler)
    while True:
	time.sleep(10)
    print("main exit")
    sys.exit()
