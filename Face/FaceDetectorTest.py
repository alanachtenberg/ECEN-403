from FaceDetector import FaceDetector
import logging
import time
import sys

last_face_value = False
last_eyes_value = False

output_pin_1 = 0
output_pin_2 = 0


def face_detection_handler(face, eyes):
    global last_face_value
    global last_eyes_value
    if face is not last_face_value:
        last_face_value = face
        logging.debug("Writing Face: %s", face)
        if face:
            output_pin_1 = 1  # TODO write 1 to an output pin
        else:
            output_pin_1 = 0  # TODO write 0
    if eyes is not last_eyes_value:
        last_eyes_value = eyes
        logging.debug("Writing Eyes: %s", face)
        if eyes:
            output_pin_2 = 1  # TODO write 1 to another output pin
        else:
            output_pin_2 = 0  # TODO write 0

if __name__ == '__main__':
    logging.basicConfig(handlers=[logging.StreamHandler()], level=logging.DEBUG)
    face_detector = FaceDetector()
    face_detector.start(face_detection_handler)
    time.sleep(30)
    print("main exit")
    sys.exit()
