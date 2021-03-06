
import tensorflow as tf
from utils import graph_utils, visualization_utils, camera_utils
from constants import constants
import cv2
import time
import numpy as np
import cython_utils

"""
Main Program for CPE 656 Team 0.

Fuses lidar and image data to generate velocity vectors of detected objects.
"""


def main():
    """
    Main loop.
    :return: Nothing.
    """
    # Initialize the OpenCV capture
    camera_1 = camera_utils.initialize_camera(0)
    camera_2 = camera_utils.initialize_camera(1)

    # Create graph instance
    obj_detection_graph = graph_utils.load_model(tf, constants.PATH_TO_FROZEN_GRAPH)

    print("Create graph")

    # Create the category indices
    category_index = graph_utils.get_category_indices(constants.PATH_TO_OBJECT_LABELS,
                                                      constants.MAX_NUMBER_OF_OBJECT_CLASSES)

    # create sessions for each graph
    session1 = tf.Session(graph=obj_detection_graph)

    while True:
        execute_session(session1, camera_1, category_index, "0")
        execute_session(session1, camera_2, category_index, "1")

        if constants.DISPLAY_LIVE_OUTPUT:
            # Wait for q to close the window
            if visualization_utils.wait_for_q(cv2):
                break

    camera_1.stopCapture()
    camera_2.stopCapture()



def execute_session(session, camera, category_index, camera_id):
    """
    Runs a session of Tensor flow on a particular image frame.

    :param session: The session to run
    :param camera: The camera instance to pull information from.
    :param category_index: Numerical representation of the detection classifications.
    """


    time1 = time.time()
    # Create a numpy image
    image = camera.retrieveBuffer()

    image_np = cython_utils.tonumpyarray(image.getData(), constants.IMAGE_HEIGHT, constants.IMAGE_WIDTH)
    image_np = cv2.cvtColor(image_np, cv2.COLOR_BAYER_BG2BGR)

    # Expand dimensions since the model expects images to have shape: [1, None, None, 3]
    image_np_expanded = np.expand_dims(image_np, axis=0)
    image_tensor = session.graph.get_tensor_by_name('image_tensor:0')
    # Each box represents a part of the image where a particular object was detected.
    boxes = session.graph.get_tensor_by_name('detection_boxes:0')
    # Each score represent how level of confidence for each of the objects.
    # Score is shown on the result image, together with the class label.
    scores = session.graph.get_tensor_by_name('detection_scores:0')
    classes = session.graph.get_tensor_by_name('detection_classes:0')

    num_detections = session.graph.get_tensor_by_name('num_detections:0')

    # Actual detection from the result of the neural network
    (boxes, scores, classes, num_detections) = session.run(
            [boxes, scores, classes, num_detections],
            feed_dict={image_tensor: image_np_expanded})

    # Display live output if the constants boolean was set
    if constants.DISPLAY_LIVE_OUTPUT:
        # Visualize output in CV 2 window
        visualization_utils.visualize_output(cv2, image_np, boxes, classes, scores, category_index, camera_id)

    time2 = time.time()
    print("Camera #" + camera_id + " time: " + str(time2 - time1))


if __name__ == '__main__':
    main()
