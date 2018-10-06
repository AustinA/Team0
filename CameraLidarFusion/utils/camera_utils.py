import PyCapture2
import numpy as np

"""
Utilities for pulling output from a PT-Grey camera.
"""

def initialize_camera(index):
    """
    Initializes a PT Grey camera by its enumerated index, and returns the camera instance
    :return: A camera instance representing the camera and its controls.
    """
    bus = PyCapture2.BusManager()
    camera = PyCapture2.Camera()
    uid = bus.getCameraFromIndex(index)
    camera.connect(uid)
    camera.startCapture()
    return camera


def numpy_image_from_camera_image(pycap_image):
    """
    Converts a raw image from the camera to a usable format with OpenCV
    :param pycap_image:
    :return:
    """
    row_bytes = float(len(pycap_image.getData())) / float(pycap_image.getRows())
    cv_image = np.array(pycap_image.getData(), dtype="uint8").reshape((pycap_image.getRows(), pycap_image.getCols()))
    return cv_image
