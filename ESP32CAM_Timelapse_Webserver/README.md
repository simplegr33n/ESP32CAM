# ESP32CAM_Timelapse_Webserver

Code for an AI Thinker model ESP32-Cam to take timelapse photos and host a web server so the most recent picture can be previewed.

## Programming the ESP32-Cam

There are many resources online on how to program ESP32 cameras. Here's one using an Arduino Uno: [How to program ESP32 CAM using Arduino UNO](ESP32-CamCard/SD_Camerak&ab_channel=TechStudyCell "How to program ESP32 CAM using Arduino UNO").

Note that after installation, you will need to remove the jumper between GPIO0 and GND and reset the ESP32-Cam in order to recover the device's IP, which will be printed in the serial monitor, from which you can connect in a browser at **http://(device-ip)**

# Credit

For Webserver functionality, Rui Santos's [ESP32-CAM-Arduino-IDE Display-Web-Server](https://github.com/RuiSantosdotme/ESP32-CAM-Arduino-IDE/ "ESP32-CAM-Arduino-IDE Display-Web-Server") has been modified for implementation here.

For Timelapse photos, this project utilizes code from BnBe's [time-lapse-camera-esp32-cam-diy-8](https://github.com/bnbe-club/time-lapse-camera-esp32-cam-diy-8 "time-lapse-camera-esp32-cam-diy-8") and CurtisIreland's [ESP32-CamCard/SD_Camera](https://github.com/CurtisIreland/electronics/tree/master/ESP32-CamCard/SD_Camera).
