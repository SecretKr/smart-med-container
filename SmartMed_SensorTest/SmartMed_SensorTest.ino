const int LDR = A0;
#define pinBuzz 16

#include <ESP8266WiFi.h>
#include <WiFiManager.h>

// Line
#include <TridentTD_LineNotify.h>
String LINE_TOKEN = "0ZoNWMJCyYmoHO1Obb3POVLzSP57IHa3RQPR0IFraQx"; // 43

// Firebase
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define DATABASE_URL "https://smart-med-con-default-rtdb.asia-southeast1.firebasedatabase.app/"
FirebaseData fbdo;
FirebaseJson json;
FirebaseAuth auth;
FirebaseConfig config;
#include "arduino_secrets.h"

// Time
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
const long  gmtOffset_sec = 25200;
NTPClient timeClient(ntpUDP, "pool.ntp.org", gmtOffset_sec);
#include <DS3231.h>
#include <Wire.h>
DS3231 RTCclock;
RTClib RTC;

struct tm alarmTime[50];
struct tm timeNow;
struct tm timeOut;

void getTime(){
  timeClient.update();
  timeNow.tm_hour = timeClient.getHours();
  timeNow.tm_min = timeClient.getMinutes();
  timeNow.tm_sec = timeClient.getSeconds();
}

#include <EEPROM.h>

void setup() {
  Serial.begin(115200);
  pinMode(pinBuzz, OUTPUT);
//  WiFi.mode(WIFI_STA);
//  WiFiManager wm;
//  bool res;
//  res = wm.autoConnect("SMC-testing","password");
//  if(!res) Serial.println("Failed to connect");
//  else Serial.println("Connected");
//  timeClient.begin();
//  getTime();
//  Serial.print("timeNow ");
//  Serial.print(timeNow.tm_hour);
//  Serial.print(" ");
//  Serial.print(timeNow.tm_min);
//  Serial.print(" ");
//  Serial.println(timeNow.tm_sec);
//  if(timeNow.tm_hour == 7 && timeNow.tm_min == 0){
//    Serial.println("get time failed");
//  }
  Wire.begin();
  delay(500);
  Serial.println("ready");
//  RTCclock.setHour(timeNow.tm_hour);
//  RTCclock.setMinute(timeNow.tm_min);
//  RTCclock.setSecond(timeNow.tm_sec);
}

int feq = 0;

void loop() {
//  while (Serial.available()) {
//    char c = Serial.read();
//    if(c == 'u') feq = 1000;
//    if(c == 'd') feq = 0;
//  }
//  Serial.print("Ldr = ");
//  Serial.println(analogRead(LDR));
//  Serial.print("feq = ");
//  Serial.println(feq);
//  
//  tone(pinBuzz, feq);
//  delay(100);
//  tone(pinBuzz, 0);
//  delay(100);
//  tone(pinBuzz, feq);
//  delay(100);
//  tone(pinBuzz, 0);
//  
//  delay(700);
  DateTime now = RTC.now();
  Serial.print(now.hour());
  Serial.print(" ");
  Serial.print(now.minute());
  Serial.print(" ");
  Serial.println(now.second());
  delay(1000);
}
