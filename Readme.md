# Team 0
###### CPE 656 Software Engineering Studio I

Using the object detection TensorFlow library to prioritize detected objects using lidar and video input

###### PretrainedKittiObjectsDetectionExample

Uses the pretrained RCNN model (graph) on the KITTI dataset from Tensor Flow's model zoo to read in images, and output those images with 
bounding boxes around the identified objects.

For this project to work, download the KITTI trained RCNN model (graph), and unzip its content into the faster_rcnn_resnet101_kitti_2018_01_28 folder.
Alternatively, the MODEL_NAME variable in main.py can be modified to point to the desired directory of the frozen graph.

Link to the RNN KITTI trained model (graph): http://download.tensorflow.org/models/object_detection/faster_rcnn_resnet101_kitti_2018_01_28.tar.gz