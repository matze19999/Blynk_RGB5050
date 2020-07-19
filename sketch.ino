#include <FS.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiUDP.h>

const char* deviceName = "ANAVI";
char auth[] = ""; # Blynk Auth token
char ssid[] = "";
char pass[] = "";
char ip[] = ""; # IP of blynk server
int port = ; # Port of blynk server

const int pinAlarm = 16;
const int pinButton = 0;
const int pinLedRed = 12;
const int pinLedGreen = 14;
const int pinLedBlue = 13;

// rainbow effect
int r = 0;
int g = 0;
int b = 0;

int Brightness = 1023;
int Rainbowspeed = 25;
int Power = 0;
int Colorindex = 1;
unsigned long previousMillis = 0;

BLYNK_WRITE(V0)
{
    Power = param.asInt();
    if (Power == 0) dark();
    if (Power == 1) white();
}

BLYNK_WRITE(V1)
{
  if (Power == 1) {
    int Red = param[0].asInt();
    int Green = param[1].asInt();
    int Blue = param[2].asInt();
    analogWrite(pinLedRed, Red);
    analogWrite(pinLedGreen, Green);
    analogWrite(pinLedBlue, Blue);
  }
}

BLYNK_WRITE(V2)
{
  if (Power == 1) {
    Brightness = param.asInt();
    if (Colorindex == 1) white();
    if (Colorindex == 2) red();
    if (Colorindex == 3) green();
    if (Colorindex == 4) blue();
    if (Colorindex == 5) yellow();
    if (Colorindex == 6) purple();
    if (Colorindex == 7) aqua();
  }
}

BLYNK_WRITE(V3)
{
  if (Power == 1) {
    Colorindex = param.asInt();
    if (Colorindex == 1) white();
    if (Colorindex == 2) red();
    if (Colorindex == 3) green();
    if (Colorindex == 4) blue();
    if (Colorindex == 5) yellow();
    if (Colorindex == 6) purple();
    if (Colorindex == 7) aqua();
    while (Colorindex == 8){
        rainbow();
    }
  }
}

BLYNK_WRITE(V4)
{
    Rainbowspeed = abs(param.asInt());
}


void dark() {
  analogWrite(pinLedRed, 0);
  analogWrite(pinLedGreen, 0);
  analogWrite(pinLedBlue, 0);
}

void white() {
  analogWrite(pinLedRed, Brightness);
  analogWrite(pinLedGreen, Brightness);
  analogWrite(pinLedBlue, Brightness);
}

void red() {
  analogWrite(pinLedRed, Brightness);
  analogWrite(pinLedGreen, 0);
  analogWrite(pinLedBlue, 0);
}

void green() {
  analogWrite(pinLedRed, 0);
  analogWrite(pinLedGreen, Brightness);
  analogWrite(pinLedBlue, 0);
}

void blue() {
  analogWrite(pinLedRed, 0);
  analogWrite(pinLedGreen, 0);
  analogWrite(pinLedBlue, Brightness);
}

void yellow() {
  analogWrite(pinLedRed, Brightness);
  analogWrite(pinLedGreen, Brightness);
  analogWrite(pinLedBlue, 0);
}

void purple() {
  analogWrite(pinLedRed, Brightness);
  analogWrite(pinLedGreen, 0);
  analogWrite(pinLedBlue, Brightness);
}

void aqua() {
  analogWrite(pinLedRed, 0);
  analogWrite(pinLedGreen, Brightness);
  analogWrite(pinLedBlue, Brightness);
}


void rainbow() {
        setColor(255, 0, 0);    // red
        setColor(0, 255, 0);    // green
        setColor(0, 0, 255);    // blue
        setColor(255, 255, 0);  // yellow
        setColor(80, 0, 80);    // purple
        setColor(0, 255, 255);  // aqua
}

void setColor(int red, int green, int blue) {
  while ( r != red || g != green || b != blue ) {
    if ( r < red ) r += 1;
    if ( r > red ) r -= 1;

    if ( g < green ) g += 1;
    if ( g > green ) g -= 1;

    if ( b < blue ) b += 1;
    if ( b > blue ) b -= 1;

    analogWrite(pinLedRed, r);
    analogWrite(pinLedGreen, g);
    analogWrite(pinLedBlue, b);
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
    while (currentMillis - previousMillis <= Rainbowspeed) {
      currentMillis = millis();
      Blynk.run();
    }
  }
}


void connect(){
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(2, LOW);
    WiFi.mode(WIFI_STA);
    WiFi.hostname(deviceName);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(2, HIGH);

    ArduinoOTA.setHostname(deviceName);
    delay(200);
    ArduinoOTA.begin();

    ArduinoOTA.onStart([] () {
        Blynk.disconnect();
    });

    ArduinoOTA.onEnd([]() {
      ESP.restart();
    });

    Blynk.config(auth, ip, port);
    Blynk.connect();
}

void setup() {
  connect();

  pinMode(pinAlarm, OUTPUT);
  pinMode(pinLedRed, OUTPUT);
  pinMode(pinLedGreen, OUTPUT);
  pinMode(pinLedBlue, OUTPUT);
}

void loop() {
    ArduinoOTA.handle();
    Blynk.run();  
}
