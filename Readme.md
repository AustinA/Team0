# Team 0
###### CPE 656 Software Engineering Studio I

Using the object detection TensorFlow library to prioritize detected objects using lidar and video input

##  PretrainedKittiObjectsDetectionExample

Uses pretrained TensorFlow models and labels to detect objects in stationary images, and outputs a corresponding label and certainty to the image.

For this project to work, download a frozen pre-trained model model (graph), and unzip its content.
Modify the appropriate constants at the top of main.py

Link to the RNN KITTI trained model (graph): http://download.tensorflow.org/models/object_detection/faster_rcnn_resnet101_kitti_2018_01_28.tar.gz