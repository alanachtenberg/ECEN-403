#!/usr/bin/env python

import numpy as np
import cv2
import cv2.cv as cv
from video import create_capture
from common import clock, draw_str

FACE_CASCADE_FILE_NAME = "./cascades/haarcascade_frontalface_alt.xml"
EYE_CASCADE_FILE_NAME = "./cascades/haarcascade_eye.xml"
VIDEO_SOURCE = 3


def detect_face(img, cascade):
    #returns top left corner (x,y) and size(w,h) in array [x,y,w,h]
    rects = cascade.detectMultiScale(img, scaleFactor=1.28, minNeighbors=3, minSize=(60, 60), maxSize=(150,150),
                                     flags=cv.CV_HAAR_SCALE_IMAGE)
    if len(rects) == 0:
        return []
    rects[:, 2:] += rects[:, :2]# adds the first two elements to the last two elements, to get top left and bottom left points for drawing
    return rects


def detect_eyes(img, cascade):
    rects = cascade.detectMultiScale(img, scaleFactor=1.05, minNeighbors=3, minSize=(13, 13),
                                     flags=cv.CV_HAAR_SCALE_IMAGE)
    if len(rects) == 0:
        return []
    rects[:, 2:] += rects[:, :2]
    return rects

def draw_rects(img, rects, color):
    for x1, y1, x2, y2 in rects:
        cv2.rectangle(img, (x1, y1), (x2, y2), color, 2)


if __name__ == '__main__':
    fourcc = cv.CV_FOURCC('m','p','4','v')
    video_writer = cv2.VideoWriter("output.mov", fourcc,10,(320,240))
    cascade = cv2.CascadeClassifier(FACE_CASCADE_FILE_NAME)
    nested = cv2.CascadeClassifier(EYE_CASCADE_FILE_NAME)

    cam = create_capture(VIDEO_SOURCE, fallback='synth:bg=../cpp/lena.jpg:noise=0.05')
    cam.set(cv.CV_CAP_PROP_FRAME_WIDTH,320)
    cam.set(cv.CV_CAP_PROP_FRAME_HEIGHT,240)
    while True:
        ret, img = cam.read()
	gray = img
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        t = clock()
        rects = detect_face(gray, cascade)
        vis = img.copy()
        draw_rects(vis, rects, (0, 255, 0))
        for x1, y1, x2, y2 in rects:
            roi = gray[y1:y2, x1:x2]
            vis_roi = vis[y1:y2, x1:x2]
            subrects = detect_eyes(roi.copy(), nested)
            draw_rects(vis_roi, subrects, (255, 0, 0))
        dt = clock() - t

        draw_str(vis, (20, 20), 'time: %.1f ms' % (dt * 1000))
        cv2.imshow('facedetect', vis)
	video_writer.write(vis)
        if 0xFF & cv2.waitKey(5) == 27:
            break
    cv2.destroyAllWindows()
