import numpy as np
import cv2
import cv2.cv as cv
from threading import Thread
from video import create_capture
from common import clock, draw_str
import logging

__author__ = 'alan'


class FaceDetector:
    FACE_CASCADE_FILE_NAME = "./cascades/haarcascade_frontalface_alt.xml"
    EYE_CASCADE_FILE_NAME = "./cascades/haarcascade_eye.xml"

    def __init__(self, video_source=0):
        """
        :type videoSource: int
        """
        self.video_source = video_source
        self.face_cascade = cv2.CascadeClassifier(FaceDetector.FACE_CASCADE_FILE_NAME)
        self.eye_cascade = cv2.CascadeClassifier(FaceDetector.EYE_CASCADE_FILE_NAME)
        self.camera = create_capture(video_source, fallback='synth:bg=../cpp/lena.jpg:noise=0.05')
        self.callback = None
        self.gray_s_frame = None
        self.face_frames = None
        self.eye_frames = None

    def detect_face(self):
        self.face_frames = self.face_cascade.detectMultiScale(self.gray_s_frame, scaleFactor=1.28, minNeighbors=3,
                                                              minSize=(60, 60), maxSize=(150, 150),
                                                              flags=cv.CV_HAAR_SCALE_IMAGE)
        if len(self.face_frames) == 0:
            self.face_frames = None
            return False
        return True

    def detect_eyes(self):
        if self.face_frames is None:
            return False
        if len(self.face_frames) > 1:
            logging.error("More than one face detected")
            return False
        for x1, y1, x2, y2 in self.face_frames:
            roi = self.gray_s_frame[y1:y2, x1:x2]
            self.eye_frames = self.eye_cascade.detectMultiScale(roi.copy(), scaleFactor=1.05, minNeighbors=3,
                                                            minSize=(13, 13),
                                                            flags=cv.CV_HAAR_SCALE_IMAGE)
        if len(self.eye_frames) is 0:
            return False
        return True

    def start(self, output_handler):
        """starts the face detector on a new thread
        :param output_handler: handler for wether a face and or eyes are detected.
        """
        self.callback = output_handler
        thread = Thread(target=self.detection_thread)
        thread.daemon = True
        thread.start()

    def detection_thread(self):
        ret_val, image_frame = self.camera.read()  # read first frame, camera delays on first read significantly
        logging.info("Face and eye detection started")
        while True:
            t = clock()
            ret_val, image_frame = self.camera.read()
            self.gray_s_frame = cv2.cvtColor(image_frame, cv2.COLOR_BGR2GRAY)
            face_visible = self.detect_face()
            eyes_visible = self.detect_eyes()
            dt = clock() - t
            logging.debug("Face: %s Eyes: %s ms: %s", face_visible, eyes_visible, dt * 1000)
            self.callback(face_visible, eyes_visible)
