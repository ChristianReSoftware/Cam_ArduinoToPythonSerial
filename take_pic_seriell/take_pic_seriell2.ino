#include <Wire.h>
#include "esp_camera.h"
//#include "camera_index.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "esp32-hal-ledc.h"
#include "sdkconfig.h"

String msg;

//#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

int count = 0;
camera_fb_t *fb = NULL;
esp_err_t res;


void setup()
{
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  Serial.println("hallo");

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_240X240;//FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;//PIXFORMAT_GRAYSCALE;//PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 3;//von 12
  config.fb_count = 1;
  //config.exposure_value = 1000; // Beispiel: 1000 µs (1 ms)





  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }  
  else
  {
    Serial.printf("Camera init;");
  }


  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  //s->set_vflip(s, 1); // flip it back
  //s->set_ae_level(s, -2);
  // deaktiviere automatische Belichtungsanpassung
  s->set_aec2(s, 1);
  //s->set_aec_value(s, 300); // Beispiel: Belichtungszeit von 1000 µs (1 ms
  s->set_agc_gain(s, 1);       // 0 to 30
  s->set_brightness(s, -2); // Helligkeit reduzieren
  s->set_contrast(s,0); // kontrast
  s->set_saturation(s, 0);     // Saettigung -2 to 2
  //s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable
  //s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
  

  //s->set_saturation(s, -2); // lower the saturation

  Serial.printf("Camera ready to take Image;");



}//end setup

// neue Methode:
void sendImageSerial(camera_fb_t *fb)
{

 // Senden Sie zuerst die Größe des Bildes
  uint32_t imageSize = fb->len;
  uint8_t parityBit = 0;
  for (int i = 0; i < 32; i++)
  {
      parityBit ^= (imageSize >> i) & 1;
  }

  for (int i = 0; i < 4; i++) 
  {
      uint8_t sizeByte = (imageSize >> (i * 8)) & 0xFF;//little Endian
      Serial.write(sizeByte);
  }
  
  Serial.write(parityBit);  // Senden des Paritätsbits

  //Serial.write((uint8_t *) fb->buf, imageSize);


  // Bildpixel über die serielle Schnittstelle senden
  for (size_t i = 0; i < imageSize; i++) 
  {
    uint8_t dataByte = fb->buf[i];  // Konvertiere das Byte in uint8
    Serial.write(dataByte);
  }


  
}

void loop()
{
  if(count < 1)
  {
    //Take foto and Send it back
    //fb = NULL;
    res = ESP_OK;

    delay(100);// neu

    fb = esp_camera_fb_get();
    count++;
    if (!fb)
    {
        Serial.print("Image Fail");
    }
  }
    
  while (!Serial.available())
  {
    delay(100);
  }
  // Bestätigungsnachricht empfangen
  Serial.read(); // Leere den seriellen Puffer

  if (count < 2)
  {
    // Bilddaten über die serielle Verbindung senden
    //fb = esp_camera_fb_get();
    sendImageSerial(fb);
    count++;
  }
  
}


