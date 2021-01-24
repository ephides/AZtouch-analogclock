#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <math.h>

#include "secrets.h"

const char* ssid = STASSID;
const char* password = STAPSK;

/*__Pin definitions for the Arduino MKR__*/
#define TFT_CS   5
#define TFT_DC   4
#define TFT_MOSI 23
#define TFT_CLK  18
#define TFT_RST  22
#define TFT_MISO 19
#define TFT_LED  15  


#define HAVE_TOUCHPAD
#define TOUCH_CS 14
#define TOUCH_IRQ 2 
/*_______End of definitions______*/

/*____Calibrate Touchscreen_____*/
#define MINPRESSURE 10      // minimum required force for touch event
#define TS_MINX 370
#define TS_MINY 470
#define TS_MAXX 3700
#define TS_MAXY 3600
/*______End of Calibration______*/

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

int old_sec=0;
int old_sec_x=0;
int old_sec_y=0;
int old_min_x=0;
int old_min_y=0;
int old_hour_x=0;
int old_hour_y=0;

void setup() {
  pinMode(TFT_LED, OUTPUT); // define as output for backlight control
  
  tft.begin();          
  tft.setRotation(3);   // landscape mode  
  tft.fillScreen(ILI9341_BLACK);// clear screen   

  tft.setCursor(50,110);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.print("Connecting...");
  digitalWrite(TFT_LED, LOW);    // LOW to turn backlight on; 
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

  configTzTime("CET-1CEST-2,M3.5.0/02,M10.5.0/03","at.pool.ntp.org", "time.nist.gov");
  
  ArduinoOTA.begin();
  tft.fillScreen(ILI9341_BLACK);// clear screen
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  if (old_sec != p_tm->tm_sec)
  {
  tft.drawLine(160,120,old_sec_x,old_sec_y, ILI9341_BLACK);
  if (p_tm->tm_sec==0) {
    tft.drawLine(160,120,old_min_x,old_min_y,ILI9341_BLACK);
    tft.drawLine(160,120,old_hour_x,old_hour_y,ILI9341_BLACK);
  }

  //tft.fillScreen(ILI9341_BLACK);
  //tft.drawCircle(160, 120, 80, ILI9341_GREEN);
  for (int i=0; i<12; i++) {
    float angle=M_PI*i/6;
    int x1 = (160+sin(angle)*80);
    int y1 = (120-cos(angle)*80);
    int x2 = (160+sin(angle)*(i%3?70:65));
    int y2 = (120-cos(angle)*(i%3?70:65));
    tft.drawLine(x1,y1,x2,y2,ILI9341_WHITE);  
  }
  float angle=((p_tm->tm_hour%12)+(p_tm->tm_min/60.0))*M_PI/6;
  int x = 160+sin(angle)*50;
  int y = 120-cos(angle)*50;
  tft.drawLine(160,120,x,y,ILI9341_WHITE);
  old_hour_x=x;
  old_hour_y=y;

  angle= (p_tm->tm_min/30.0)*M_PI;
  x = 160+sin(angle)*75;
  y = 120-cos(angle)*75;
  tft.drawLine(160,120,x,y,ILI9341_WHITE);
  old_min_x=x;
  old_min_y=y;

  angle= (p_tm->tm_sec/30.0)*M_PI;
  x = 160+sin(angle)*80;
  y = 120-cos(angle)*80;
  tft.drawLine(160,120,x,y,ILI9341_RED);
  old_sec_x=x;
  old_sec_y=y;

  }
  old_sec=p_tm->tm_sec;
}
