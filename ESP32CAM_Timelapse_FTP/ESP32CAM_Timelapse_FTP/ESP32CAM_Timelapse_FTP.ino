/****************************************************************************************
 * Example code for capturing timelapse photos on an ESP32-Cam,
 * as well as making the file SD card and images available via
 * FTP.
 * 
 * This code utilizes code from these prior projects:
 * **************************************************
 * * Hosting an FTP server on the ESP32
 * * * * https://github.com/fa1ke5/ESP32_FTPServer_SD_MMC
 * 
 * * Taking timelapse images:
 * * * * https://github.com/bnbe-club/time-lapse-camera-esp32-cam-diy-8
 * 
 * *Saving images to SD card:
 * * * * https://github.com/CurtisIreland/electronics/tree/master/ESP32-CamCard/SD_Camera
 * 
 * 
 * - simplegr33n 2021
 ***************************************************************************************/

#include "auth.h" // .gitignore'd credential file
#include "FS.h"
#include "SD_MMC.h"
#include "esp_camera.h"
#include <EEPROM.h> // read and write from flash memory
#include <WiFi.h>

boolean WIFI_CONNECTED = false;

// define number of bytes to set aside in persistant memory
#define EEPROM_SIZE 1
int pictureNumber = 0; // used to sequence pictures when not connected to WIFI

// Wifi vars
const char *wifi_ssid = WIFI_SSID;
const char *wifi_password = WIFI_PASSWORD;

// Time server
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -25200; // Mountain Time (MT) offset = -25200
const int daylightOffset_sec = 0;

// Select camera pin configuration
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// Photograph timing
unsigned long lastPhotoMillis = -1;
const int photoDelaySeconds = 600; // time between photos in seconds

// FTP Server
#include <WiFiClient.h>
#include "ESP32FtpServer.h"

const char *ftp_user = FTP_USER;
const char *ftp_password = FTP_PASSWORD;

FtpServer ftpSrv; //set #define FTP_DEBUG in ESP32FtpServer.h to see ftp verbose on serial

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Booting...");

    //// Camera setup
    initCamera();

    ////  WIFI setup
    //// TODO: if no wifi, use an EEPROM stored value to increment photos in a non-timestamped folder
    WiFi.begin(wifi_ssid, wifi_password);
    Serial.println("Connecting to wifi...");
    int wifiConnectAttempts = 0;
    while (WiFi.status() != WL_CONNECTED && wifiConnectAttempts < 20) // Wait for connection
    {
        wifiConnectAttempts += 1;
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        WIFI_CONNECTED = true;

        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(wifi_ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        /////FTP Setup -- ensure SD is started before ftp
        //////////////
        if (SD_MMC.begin())
        {
            Serial.println("SD initialized");
            ftpSrv.begin(ftp_user, ftp_password); // Default ports: 21, 50009 for PASV // Set in ESP32FtpServer.h
        }

        ///// Time setup
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }
    else
    {
        Serial.println("");
        Serial.println("Unable to connect to WIFI");
        Serial.println("Saving sequentially numbered images.");
    }
}

void loop()
{
    //// Handle FTP
    ftpSrv.handleFTP();

    //// Take photo on first loop, at set delay, and on millis() overflow
    if ((lastPhotoMillis == -1) || (millis() - lastPhotoMillis > photoDelaySeconds * 1000) || (millis() < lastPhotoMillis))
    {
        lastPhotoMillis = millis();
        TriggerCamera();
    }
}

///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///
///   ///   ///   ///   ///   ///   ///   ///   ///   ///   ///

/// Take and Save Photo
void TriggerCamera()
{
    if (!SD_MMC.begin())
    {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("No SD_MMC card attached");
        return;
    }
    Serial.println("SD Card Initialized");

    if (WIFI_CONNECTED)
    {
        //// get time
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            Serial.println("Failed to obtain time");
            return;
        }

        // get date and time Strings
        char dateStringBuff[16]; //16 chars should be enough
        char timeStringBuff[16];
        strftime(dateStringBuff, sizeof(dateStringBuff), "%Y-%m-%d", &timeinfo);
        strftime(timeStringBuff, sizeof(timeStringBuff), "%Hh%Mm%S", &timeinfo);
        String dateString(dateStringBuff); // "2021-01-18"
        String dateDirString = "/" + dateString;
        String timeString(timeStringBuff); // "15h59m21"

        const char *dateDirChars = dateDirString.c_str();
        createDir(SD_MMC, dateDirChars); // create directory if not existing

        // Call function to capture the image and save it as a file
        String path = "/" + dateString + "/" + dateString + "_" + timeString + ".jpg";
        if (get_image(SD_MMC, path.c_str()) != ESP_OK)
        {
            Serial.println("Failed to capture picture");
        }
        else
        {
            Serial.println("Captured picture");
        }
    }
    else
    {
        // initialize EEPROM with predefined size
        EEPROM.begin(EEPROM_SIZE);
        pictureNumber = EEPROM.read(0) + 1;

        createDir(SD_MMC, "/notime"); // create directory if not existing

        // Call function to capture the image and save it as a file
        String path = "/notime/notime_" + String(pictureNumber) + ".jpg";
        if (get_image(SD_MMC, path.c_str()) != ESP_OK)
        {
            Serial.println("Failed to capture picture");
        }
        else
        {
            Serial.println("Captured picture (NO TIME)");
            EEPROM.write(0, pictureNumber);
            EEPROM.commit();
        }
    }
}

void initCamera()
{
    // Initial camera configuration
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG; // This is very important for saving the frame buffer as a JPeg

    //init with high specs to pre-allocate larger buffers
    if (psramFound())
    {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    }
    else
    {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    /*****
You may need to adjust these settings to get a picture you like. 
Look in app_httpd.cpp in the CameraWebServer as an example on how these are used.
sensor_t * s = esp_camera_sensor_get();
int res = 0;
if(!strcmp(variable, "framesize")) {
    if(s->pixformat == PIXFORMAT_JPEG) res = s->set_framesize(s, (framesize_t)val);
}
else if(!strcmp(variable, "quality")) res = s->set_quality(s, val);
else if(!strcmp(variable, "contrast")) res = s->set_contrast(s, val);
else if(!strcmp(variable, "brightness")) res = s->set_brightness(s, val);
else if(!strcmp(variable, "saturation")) res = s->set_saturation(s, val);
else if(!strcmp(variable, "gainceiling")) res = s->set_gainceiling(s, (gainceiling_t)val);
else if(!strcmp(variable, "colorbar")) res = s->set_colorbar(s, val);
else if(!strcmp(variable, "awb")) res = s->set_whitebal(s, val);
else if(!strcmp(variable, "agc")) res = s->set_gain_ctrl(s, val);
else if(!strcmp(variable, "aec")) res = s->set_exposure_ctrl(s, val);
else if(!strcmp(variable, "hmirror")) res = s->set_hmirror(s, val);
else if(!strcmp(variable, "vflip")) res = s->set_vflip(s, val);
else if(!strcmp(variable, "awb_gain")) res = s->set_awb_gain(s, val);
else if(!strcmp(variable, "agc_gain")) res = s->set_agc_gain(s, val);
else if(!strcmp(variable, "aec_value")) res = s->set_aec_value(s, val);
else if(!strcmp(variable, "aec2")) res = s->set_aec2(s, val);
else if(!strcmp(variable, "dcw")) res = s->set_dcw(s, val);
else if(!strcmp(variable, "bpc")) res = s->set_bpc(s, val);
else if(!strcmp(variable, "wpc")) res = s->set_wpc(s, val);
else if(!strcmp(variable, "raw_gma")) res = s->set_raw_gma(s, val);
else if(!strcmp(variable, "lenc")) res = s->set_lenc(s, val);
else if(!strcmp(variable, "special_effect")) res = s->set_special_effect(s, val);
else if(!strcmp(variable, "wb_mode")) res = s->set_wb_mode(s, val);
else if(!strcmp(variable, "ae_level")) res = s->set_ae_level(s, val);
 *****/

    Serial.println("Camera initialized!");
}

// Get image
static esp_err_t get_image(fs::FS &fs, const char *path)
{
    // Variable definitions for camera frame buffer
    camera_fb_t *fb = NULL;
    int64_t fr_start = esp_timer_get_time();

    // Get the contents of the camera frame buffer
    fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Camera capture failed");
        return ESP_FAIL;
    }

    // Save image to file
    size_t fb_len = 0;
    fb_len = fb->len;
    File file = fs.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to open file in writing mode");
    }
    else
    {
        file.write(fb->buf, fb->len); // payload (image), payload length
        Serial.printf("Saved file to path: %s\n", path);
    }
    file.close();

    // Release the camera frame buffer
    esp_camera_fb_return(fb);
    int64_t fr_end = esp_timer_get_time();
    Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
    return ESP_OK;
}

///
///
/// SD funcs
void createDir(fs::FS &fs, const char *path)
{
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path))
    {
        Serial.println("Dir created");
    }
    else
    {
        Serial.println("mkdir failed");
    }
}
