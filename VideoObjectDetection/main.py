import numpy as np
import tensorflow as tf
from object_detection.utils import label_map_util
from object_detection.utils import visualization_utils as vis_util
import cv2

# Constants for Kitti trained object detection (cars and pedestrians)
MODEL_NAME_1 = "ssd_mobilenet_v1_coco_2018_01_28"
PATH_TO_FROZEN_GRAPH_1 = MODEL_NAME_1 + "/frozen_inference_graph.pb"
PATH_TO_LABELS_1 = MODEL_NAME_1 + "/mscoco_label_map.pbtxt"


def main():
    # Initialize the OpenCV capture
    capture = cv2.VideoCapture(0)

    # Create graph instance
    kitti_detection_graph = load_model(PATH_TO_FROZEN_GRAPH_1)

    print("Create graph")

    # Create the category indices
    label_map = label_map_util.load_labelmap(PATH_TO_LABELS_1)
    categories = label_map_util.convert_label_map_to_categories(label_map, max_num_classes=90, use_display_name=True)
    category_index = label_map_util.create_category_index(categories)

    # create sessions for each graph
    session1 = tf.Session(graph=kitti_detection_graph)

    print("Create session")

    execute_session(session1, capture, category_index)


def execute_session(session, capture, category_index):
    """
    Runs a session of Tensor flow on a particular image.

    :param session: The session to run
    :param capture: The image to perform the neural network against.
    :param category_index: Numerical representation of the detection classifications.
    """

    while True:

        # Create a numpy image
        ret, image_np = capture.read()

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

        vis_util.visualize_boxes_and_labels_on_image_array(
            image_np,
            np.squeeze(boxes),
            np.squeeze(classes).astype(np.int32),
            np.squeeze(scores),
            category_index,
            use_normalized_coordinates=True,
            line_thickness=8)

        # Display output for fun
        cv2.imshow('object detection', cv2.resize(image_np, (800, 600)))

        # Kill the window by pressing q
        if cv2.waitKey(25) & 0xFF == ord('q'):
            cv2.destroyAllWindows()
            break


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


if __name__ == '__main__':
    main()
