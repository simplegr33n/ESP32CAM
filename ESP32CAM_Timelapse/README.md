# ESP32CAM_Timelapse_FTP

Code for an AI Thinker model ESP32-Cam to take timelapse photos and host an FTP server so the pictures can be previewed, copied, or deleted.

## Programming the ESP32-Cam

There are many resources online on how to program ESP32 cameras. Here's one using an Arduino Uno: [How to program ESP32 CAM using Arduino UNO](ESP32-CamCard/SD_Camerak&ab_channel=TechStudyCell "How to program ESP32 CAM using Arduino UNO").

Note that after installation, you will need to remove the jumper between GPIO0 and GND and reset the ESP32-Cam in order to recover the device's IP, which will be printed in the serial monitor. You may also need to port forward ports 21 and 50009 on your router for the device (or configure and forward your own choice of ports).

## FTP Server

It's worth noting that as a low power/low memory device, the ESP32 doesn't serve as an ideal FTP server, but it does allow one to remotely check how the camera is performing, ie. by viewing the most recent picture to see that you have the camera oriented correctly.

# Credit

For FTP functionality, fa1ke5's [ESP32_FTPServer_SD](https://github.com/fa1ke5/ESP32_FTPServer_SD_MMC "ESP32_FTPServer_SD") has been modified for implementation here, itself based on robo8080's [ESP32_FTPServer_SD](https://github.com/robo8080/ESP32_FTPServer_SD "ESP32_FTPServer_SD") and nailbuster's [Simple FTP Server for using esp8266 SPIFFs](https://github.com/nailbuster/esp8266FTPServer "Simple FTP Server for using esp8266 SPIFFs").

For Timelapse photos, this project utilizes code from BnBe's [time-lapse-camera-esp32-cam-diy-8](https://github.com/bnbe-club/time-lapse-camera-esp32-cam-diy-8 "time-lapse-camera-esp32-cam-diy-8") and CurtisIreland's [ESP32-CamCard/SD_Camera](https://github.com/CurtisIreland/electronics/tree/master/ESP32-CamCard/SD_Camera).
