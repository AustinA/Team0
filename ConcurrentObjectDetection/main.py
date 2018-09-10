"""
Uses the TensorFlow API object detection API and any pre-trained neural networks from the model zoo
to detect objects from input images, boxing their outline and writing it to an output file.

This script performs object detection through two concurrent neural networks.
"""
__author__ = "Austin Alderton"

import csv
import glob
import os
from threading import Thread

import numpy as np
import tensorflow as tf
from PIL import Image
from object_detection.utils import visualization_utils as vis_util

# Constants for Kitti trained object detection (cars and pedestrians)
MODEL_NAME_1 = "faster_rcnn_resnet101_kitti_2018_01_28"
PATH_TO_FROZEN_GRAPH_1 = MODEL_NAME_1 + "/frozen_inference_graph.pb"
PATH_TO_LABELS_1 = MODEL_NAME_1 + "/labels.csv"

# Constants for road sign trained object detection (road signs)
MODEL_NAME_2 = "signs_50k"
PATH_TO_FROZEN_GRAPH_2 = MODEL_NAME_2 + "/model_50K.pb"
PATH_TO_LABELS_2 = MODEL_NAME_2 + "/labels.csv"

# local lists of neural network results
kitti_list = []
signs_list = []


def main():
    # load the images and file names
    (file_names, numpy_images) = load_images()

    # create a graph and category indices that will detect cars and pedestrians from a frozen inference
    kitti_detection_graph = load_model(PATH_TO_FROZEN_GRAPH_1)
    kitti_category_indices = load_colloquial_labels(PATH_TO_LABELS_1)

    # create a graph and category indices that will detect signs from a frozen inference
    signs_detection_graph = load_model(PATH_TO_FROZEN_GRAPH_2)
    signs_category_indices = load_colloquial_labels(PATH_TO_LABELS_2)

    # create sessions for each graph
    session1 = tf.Session(graph=kitti_detection_graph)
    session2 = tf.Session(graph=signs_detection_graph)

    # create threads that will perform object detections for each graph in parallel
    thread1 = Thread(target=worker_runnable, args=(session1, numpy_images, kitti_list))
    thread2 = Thread(target=worker_runnable, args=(session2, numpy_images, signs_list))

    # start the threads
    thread1.start()
    thread2.start()

    # serialize the recombination and writing of the output images
    thread1.join()
    thread2.join()

    # for each result, apply the detections of each graph and save the output image
    for i in range(0, len(kitti_list)):
        mod_image = vis_util.visualize_boxes_and_labels_on_image_array(
            numpy_images[i],
            np.squeeze(kitti_list.__getitem__(i)[0]),
            np.squeeze(kitti_list.__getitem__(i)[2]).astype(np.int32),
            np.squeeze(kitti_list.__getitem__(i)[1]),
            kitti_category_indices,
            use_normalized_coordinates=True,
            line_thickness=8)

        mod_image = vis_util.visualize_boxes_and_labels_on_image_array(
            mod_image,
            np.squeeze(signs_list.__getitem__(i)[0]),
            np.squeeze(signs_list.__getitem__(i)[2]).astype(np.int32),
            np.squeeze(signs_list.__getitem__(i)[1]),
            signs_category_indices,
            use_normalized_coordinates=True,
            line_thickness=8)

        # save the output image
        vis_util.save_image_array_as_png(mod_image,
                                         os.path.join("output/", os.path.basename(file_names.__getitem__(i))))


def worker_runnable(session, images, list_to_add_to):
    """
    The function that the thread executes.  Runs a tensor flow sesion on a graph, outputting the results of the
    neural network operations to a list.

    :param session: The tensor flow session to run
    :param images: The input images
    :param list_to_add_to: List to add the graph results to
    """
    for image in images:
        test = execute_session(session, image)
        list_to_add_to.append(test)


def load_images():
    """
    Returns numpy images and the file names of the input images.

    :return: Two lists, the numpy arrays of the input and their file names.
    """
    list_return = []
    file_name_return = []
    for file_name in glob.glob('img/*.jpg'):
        # Load the image
        image = Image.open(file_name)
        image_np = load_image_into_numpy_array(image)
        if image_np is not None:
            list_return.append(image_np)
            file_name_return.append(file_name)
    return file_name_return, list_return


# Load the frozen graph from the file system and return the graph.
def load_model(path_to_frozen_graph):
    """
    Loads a frozen serialized Tensor flow graph into Tensor Flow

    :param path_to_frozen_graph: Path to the frozen graph
    :return: A Tensor Flow graph with the loaded structures and weights of the serialized graph.
    """
    detection_graph = tf.Graph()
    with detection_graph.as_default():
        obj_detect_graph_def = tf.GraphDef()
        with tf.gfile.GFile(path_to_frozen_graph, 'rb') as fid:
            serialized_graph = fid.read()
            obj_detect_graph_def.ParseFromString(serialized_graph)
            tf.import_graph_def(obj_detect_graph_def, name="")
    return detection_graph


# Load the labels of the provided graph.
def load_colloquial_labels(path_to_labels):
    """
    Loads the labels of things detected using the object detection neural network
    :param path_to_labels: Path to the labels file in CSV format
    :return: List of labels
    """
    labels = {}
    with open(path_to_labels, 'r') as csv_file:
        lbl_reader = csv.reader(csv_file, delimiter=',')
        for row in lbl_reader:
            labels[int(row[0])] = dict(id=row[0], name=row[2])
    return labels


# Call the tensors to process the images using the graph.
def execute_session(session, image):
    """
    Runs a session of Tensor flow on a particular image.

    :param session: The session to run
    :param image: The image to perform the neural network against.
    :return: The detection boxes, scores, classes, and number of detections (the result of the object detection
    algorithm on the input image.
    """
    # Expand dimensions since the model expects images to have shape: [1, None, None, 3]
    image_np_expanded = np.expand_dims(image, axis=0)
    image_tensor = session.graph.get_tensor_by_name('image_tensor:0')
    # Each box represents a part of the image where a particular object was detected.
    boxes = session.graph.get_tensor_by_name('detection_boxes:0')
    # Each score represent how level of confidence for each of the objects.
    # Score is shown on the result image, together with the class label.
    scores = session.graph.get_tensor_by_name('detection_scores:0')
    classes = session.graph.get_tensor_by_name('detection_classes:0')

    num_detections = session.graph.get_tensor_by_name('num_detections:0')

    # Actual detection.
    (boxes, scores, classes, num_detections) = session.run(
        [boxes, scores, classes, num_detections],
        feed_dict={image_tensor: image_np_expanded})

    return boxes, scores, classes, num_detections


def load_image_into_numpy_array(image):
    """Create a numpy array from an image

       This method may be called concurrently from multiple threads.

       :param: image: The image to be turned into a numpy array.
       :return: Numpy array representation of an image.
       """
    (im_width, im_height) = image.size
    return np.array(image.getdata()).reshape(
        (im_height, im_width, 3)).astype(np.uint8)


if __name__ == "__main__":
    main()
