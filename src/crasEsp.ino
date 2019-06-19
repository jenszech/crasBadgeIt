/* e-paper display lib */
#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
/* include any other fonts you want to use https://github.com/adafruit/Adafruit-GFX-Library */
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>

/* WiFi  libs*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
/* Util libs */
#include <Time.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

// 'wi-day-sunny', 48x48px
const unsigned char day_sunny_small [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff,
	0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f,
	0xff, 0xff, 0xff, 0xf3, 0xfe, 0x7f, 0xcf, 0xff, 0xff, 0xf1, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xf0,
	0xff, 0xff, 0x0f, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0x1f, 0xff, 0xff, 0xfc, 0xfc, 0x3f, 0x3f, 0xff,
	0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xff, 0x87, 0xe1,
	0xff, 0xff, 0xff, 0xff, 0x8f, 0xf1, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf8, 0xff, 0xff, 0xff, 0xff,
	0x1f, 0xf8, 0xff, 0xff, 0xfe, 0x07, 0x1f, 0xfc, 0xe0, 0x7f, 0xfe, 0x07, 0x3f, 0xfc, 0xe0, 0x7f,
	0xff, 0x0f, 0x1f, 0xfc, 0xf0, 0xff, 0xff, 0xff, 0x1f, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x1f, 0xf8,
	0xff, 0xff, 0xff, 0xff, 0x8f, 0xf1, 0xff, 0xff, 0xff, 0xff, 0x83, 0xc1, 0xff, 0xff, 0xff, 0xff,
	0xc0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0xfc, 0x3f, 0x3f, 0xff,
	0xff, 0xf8, 0xff, 0xff, 0x1f, 0xff, 0xff, 0xf0, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xf1, 0xff, 0xff,
	0x8f, 0xff, 0xff, 0xf3, 0xfe, 0x7f, 0xcf, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff,
	0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
const char* host = "172.21.32.205";

/* Always include the update server, or else you won't be able to do OTA updates! */
/**/const int port = 8888;
/**/ESP8266WebServer httpServer(port);
/**/ESP8266HTTPUpdateServer httpUpdater;
/*                                                                                */

/* Configure pins for display */
GxIO_Class io(SPI, SS, 0, 2);
GxEPD_Class display(io); // default selection of D4, D2

void setup()
{
  display.init();

  pinMode(1,INPUT_PULLUP); //down
  pinMode(3,INPUT_PULLUP); //left
  pinMode(5,INPUT_PULLUP); //center
  pinMode(12,INPUT_PULLUP); //right
  pinMode(10,INPUT_PULLUP); //up

  /* WiFi Manager automatically connects using the saved credentials, if that fails it will go into AP mode */
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("Badgy AP");

  if(digitalRead(5) == 0) {
    /* Once connected to WiFi, startup the OTA update server if the center button is held on boot */
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    showIP();
    while(1){
      httpServer.handleClient();
    }
  }

  if (getRoomData()) {
    // Success in getting weather an forecast data.  Sleep for an 60e6 microseconds -- one minute.
    ESP.deepSleep(60e6, WAKE_RF_DEFAULT);
  } else {
    // A failure of some sort.  Wait for 5 seconds and then retry.
    ESP.deepSleep(5e6, WAKE_RF_DEFAULT);
  }
}

void loop()
{
  // loop is never executed in this program as the setup does all the work
  // then puts the ESP into a deep sleep which will cause a reset at the
  // conclusion which runs setup again.
}

void configModeCallback (WiFiManager *myWiFiManager){
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,50);
  display.println("Connect to Badgy AP");
  display.println("to setup your WiFi!");
  display.update();
}

void showText(char *text)
{
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(10,70);
  display.println(text);
  display.println("Auto-retry in 5 seconds.");
  display.update();
}

void showIP(){
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,10);

  String url = WiFi.localIP().toString() + ":"+String(port)+"/update";
  byte charArraySize = url.length() + 1;
  char urlCharArray[charArraySize];
  url.toCharArray(urlCharArray, charArraySize);

  display.println("You are now connected!");
  display.println("");
  display.println("Go to:");
  display.println(urlCharArray);
  display.println("to upload a new sketch.");
  display.update();
}

bool getRoomData()
{
  String roomID= "Raum_Textunes_Adlerh@tde.thalia.de";
  String url = "/rooms/" + roomID;

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 3001;
  if (!client.connect(host, httpPort)) {
    showText("connection failed");
    return false;
  }

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      showText(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  // Read response
  String roomName;
  int appointmentsCount;
  String currentAppointmentName = "";
  String displayTime;
  int currentAppointmentIndex = -1;
  String currentTime;

  while(client.available()){
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    //Serial.println(status);
    if(strcmp(status, "HTTP/1.1 200 OK") != 0){
      showText("HTTP Status Error!");
      return false;
    }

    /* Find the end of headers */
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      showText("Invalid Response...");
      return false;
    }

    /* Start parsing the JSON in the response body */
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(client);
    if(!root.success()){
      showText("JSON parsing failed!");
      return false;
    }

    roomName = root["roomName"].as<String>();
    currentTime = root["currentTime"].as<String>();

    appointmentsCount = root["appointments"].size();

    for (int i = 0; i <= appointmentsCount; i++) {
      bool isCurrent = root["appointments"][i]["isCurrent"];
      if (isCurrent) {
        currentAppointmentIndex = i;
      }
    }

    if (currentAppointmentIndex >= 0) {
      currentAppointmentName = root["appointments"][currentAppointmentIndex]["title"].as<String>();
      displayTime = root["appointments"][currentAppointmentIndex]["displayTime"].as<String>();
    }
  }

  /* Display Room data conditions */
  display.setRotation(1); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* small = &FreeMonoBold9pt7b;
  const GFXfont* big = &FreeMonoBold12pt7b;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(big);

  int currentXpos = 0;
  const int lineSpacing = 20;
  int paddingLeft = 4;

  currentXpos += lineSpacing;
  display.setCursor(paddingLeft, currentXpos);
  display.print(roomName);

  display.setFont(small);

  currentXpos += lineSpacing + lineSpacing;
  display.setCursor(paddingLeft, currentXpos);
  display.print("Aktueller Termin:");

  if (currentAppointmentIndex >= 0) {
    currentXpos += lineSpacing;
    display.setCursor(paddingLeft, currentXpos);
    display.print(currentAppointmentName);

    currentXpos += lineSpacing + lineSpacing;
    display.setCursor(paddingLeft, currentXpos);
    display.print(displayTime);
  }

  // show sun icon
  display.drawBitmap(day_sunny_small, 220, 55, 48, 48, GxEPD_WHITE);

  display.setCursor(220, currentXpos);
  display.print(currentTime);

  display.update();

  return true;
}
