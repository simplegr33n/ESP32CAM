# ESP32CAM_Timelapse

Code for an AI Thinker model ESP32-Cam to take timelapse photos and save to SD card.

## Programming the ESP32-Cam

There are many resources online on how to program ESP32 cameras. Here's one using an Arduino Uno: [How to program ESP32 CAM using Arduino UNO](ESP32-CamCard/SD_Camerak&ab_channel=TechStudyCell "How to program ESP32 CAM using Arduino UNO").

Note that after installation, you will need to remove the jumper between GPIO0 and GND and reset the ESP32-Cam in order to recover the device's IP, which will be printed in the serial monitor. You may also need to port forward ports 21 and 50009 on your router for the device (or configure and forward your own choice of ports).

# Credit

For Timelapse photos, this project utilizes code from BnBe's [time-lapse-camera-esp32-cam-diy-8](https://github.com/bnbe-club/time-lapse-camera-esp32-cam-diy-8 "time-lapse-camera-esp32-cam-diy-8") and CurtisIreland's [ESP32-CamCard/SD_Camera](https://github.com/CurtisIreland/electronics/tree/master/ESP32-CamCard/SD_Camera) for SD card saving.
