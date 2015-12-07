import logging
import os

def writePinValue(pin, pinValue):
    os.system("echo " + pinValue + " > " + pin + "value")

def init_gpio(gpio_1, gpio_2):
    logging.info("Initializing GPIO pins: " + gpio_1 + " " + gpio_2)
    os.system("echo out > " + gpio_1 + "direction")
    os.system("echo out > " + gpio_2 + "direction")
    writePinValue(gpio_1, "0")
    writePinValue(gpio_2, "0")