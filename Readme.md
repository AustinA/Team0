# Team 0
###### CPE 656 Software Engineering Studio I

Using the object detection TensorFlow library to prioritize detected objects using lidar and video input

## PretrainedObjectDetectionExample

Uses pretrained TensorFlow models and labels to detect objects in stationary images, and outputs a corresponding label and certainty to the image.

For this project to work, download a frozen pre-trained model model (graph), and unzip its content.
Modify the appropriate constants at the top of main.py.

The labeling for this project uses a .csv format. The corresponding label files and frozen inference graphs can be found on the Team 0 Google Drive.

## ConcurrentObjectDetection

Uses pretrained TensorFlow models and labels to detect objects in stationary images, and outputs a corresponding label and certainty to the image.

This project can run two concurrently graphs at once, performing object detection of differently trained graphs in parallel, and combining their
results into a single output.  In our use case, a car and pedestrian detector run concurrently with a road sign detector graph.

For this project to work, download a frozen pre-trained model model (graph), and unzip its content.
Modify the appropriate constants at the top of main.py.

The labeling for this project uses a .csv format. The corresponding label files and frozen inference graphs can be found on the Team 0 Google Drive.


## VideoObjectDetection

Uses a pretrained TensorFlow model and labels to detect objects from live video.

Performs object detection through TensorFlow on input from a camera. Depends on opencv-python, which can be installed using pip.

