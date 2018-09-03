#!/usr/bin/env python

"""
Uses the TensorFlow API object detection API and any pre-trained neural network from the model zoo
to detect objects from input images, boxing their outline and writing it to an output file.
"""
__author__ = "Austin Alderton"

import csv
import glob
import os

import numpy as np
import tensorflow as tf
from PIL import Image
from object_detection.utils import ops as utils_ops
from object_detection.utils import visualization_utils as vis_util

# Constants
MODEL_NAME = "uah-signs-50k"
PATH_TO_FROZEN_GRAPH = MODEL_NAME + "/model_50k.pb"
PATH_TO_LABELS = MODEL_NAME + "/labels.csv"


# Main subroutine.  Loads the model (graph) and its categories, and loops through each input image to apply the
# output boxes to them through the results of the provided model (graph). The files are then written to output.
def main():
    detection_graph = load_model()
    category_index = load_colloquial_labels()

    # For every jpg in the img/ directory
    for file_name in glob.glob('img/*.jpg'):
        # Load the image
        image = Image.open(file_name)
        image_np = load_image_into_numpy_array(image)
        # Actual detection results
        output_dict = execute_inference_for_image(image_np, detection_graph)
        # Visualize the results of a detection.
        modified_image = vis_util.visualize_boxes_and_labels_on_image_array(
            image_np,
            output_dict['detection_boxes'],
            output_dict['detection_classes'],
            output_dict['detection_scores'],
            category_index,
            instance_masks=output_dict.get('detection_masks'),
            use_normalized_coordinates=True,
            line_thickness=8)
        vis_util.save_image_array_as_png(modified_image, os.path.join("output/", os.path.basename(file_name)))


# Load the frozen graph from the file system and return the graph.
def load_model():
    detection_graph = tf.Graph()
    with detection_graph.as_default():
        obj_detect_graph_def = tf.GraphDef()
        with tf.gfile.GFile(PATH_TO_FROZEN_GRAPH, 'rb') as fid:
            serialized_graph = fid.read()
            obj_detect_graph_def.ParseFromString(serialized_graph)
            tf.import_graph_def(obj_detect_graph_def, name="")
    return detection_graph


# Load the labels of the provided graph.
def load_colloquial_labels():
    labels = {}
    with open(PATH_TO_LABELS, 'r') as csv_file:
        lbl_reader = csv.reader(csv_file, delimiter=',')
        for row in lbl_reader:
            labels[int(row[0])] = dict(id=row[0], name=row[2])
    return labels


# Call the tensors to process the images using the graph.
def execute_inference_for_image(image, graph):
    """Call the tensors to process the images using the graph.

       This method may be called concurrently from multiple threads.

       Args:
         image: The image to pass to the tensor
         graph:  The model to process the image.
       """
    # Create a default context manager
    with graph.as_default():
        # Create a new Tensor Flow Session from the generated graph
        with tf.Session() as sess:
            # Get operations from the input graph.
            ops = tf.get_default_graph().get_operations()
            # Get the names of the tensors
            all_tensor_names = {output.name for op in ops for output in op.outputs}
            tensor_dict = {}
            for key in [
                'num_detections', 'detection_boxes', 'detection_scores',
                'detection_classes', 'detection_masks'
            ]:
                tensor_name = key + ':0'
                # If any of the built-in visualization tensors exist in the operations of the graph
                if tensor_name in all_tensor_names:
                    # Add it to our dictionary
                    tensor_dict[key] = tf.get_default_graph().get_tensor_by_name(
                        tensor_name)
            # If a detection_masks tensor exists in the dictionary
            if 'detection_masks' in tensor_dict:
                # The following processing is only for single image
                detection_boxes = tf.squeeze(tensor_dict['detection_boxes'], [0])
                detection_masks = tf.squeeze(tensor_dict['detection_masks'], [0])
                # Reframe is required to translate mask from box coordinates to image coordinates and fit the image size
                real_num_detection = tf.cast(tensor_dict['num_detections'][0], tf.int32)
                detection_boxes = tf.slice(detection_boxes, [0, 0], [real_num_detection, -1])
                detection_masks = tf.slice(detection_masks, [0, 0, 0], [real_num_detection, -1, -1])
                detection_masks_reframed = utils_ops.reframe_box_masks_to_image_masks(
                    detection_masks, detection_boxes, image.shape[0], image.shape[1])
                detection_masks_reframed = tf.cast(
                    tf.greater(detection_masks_reframed, 0.5), tf.uint8)
                # Follow the convention by adding back the batch dimension
                tensor_dict['detection_masks'] = tf.expand_dims(
                    detection_masks_reframed, 0)
            # Get the image tensor, and run the inference on it.
            image_tensor = tf.get_default_graph().get_tensor_by_name('image_tensor:0')

            # Run inference
            output_dict = sess.run(tensor_dict,
                                   feed_dict={image_tensor: np.expand_dims(image, 0)})

            # all outputs are float32 numpy arrays, so convert types as appropriate
            output_dict['num_detections'] = int(output_dict['num_detections'][0])
            output_dict['detection_classes'] = output_dict[
                'detection_classes'][0].astype(np.uint8)
            output_dict['detection_boxes'] = output_dict['detection_boxes'][0]
            output_dict['detection_scores'] = output_dict['detection_scores'][0]
            if 'detection_masks' in output_dict:
                output_dict['detection_masks'] = output_dict['detection_masks'][0]
    return output_dict


def load_image_into_numpy_array(image):
    """Create a numpy array from an image

       This method may be called concurrently from multiple threads.

       Args:
         image: The image to be turned into a numpy array.

       Returns:
           Numpy array representation of an image.
       """
    (im_width, im_height) = image.size
    return np.array(image.getdata()).reshape(
        (im_height, im_width, 3)).astype(np.uint8)


if __name__ == "__main__":
    main()
