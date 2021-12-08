#include <Scheduler.h>
#include <ESP8266HTTPClient.h>
String serverName = "http://ikwmystery.atwebpages.com";

// Wifi
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
char* ssid = "ff8";
char* password = "Tekanawww9735";
#include <ESPAsyncWebServer.h>

// Time
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
const long  gmtOffset_sec = 25200;
NTPClient timeClient(ntpUDP, "pool.ntp.org", gmtOffset_sec);

// Line
//#include <WiFiClientSecure.h>
#include <TridentTD_LineNotify.h>
#define LINE_TOKEN "0ZoNWMJCyYmoHO1Obb3POVLzSP57IHa3RQPR0IFraQx"

// Firebase
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define API_KEY "AIzaSyDC06NHOw7ZXuUneZsZvUcaW4PkfOMsNWc"
#define DATABASE_URL "https://smart-med-con-default-rtdb.asia-southeast1.firebasedatabase.app/"
FirebaseData fbdo;
FirebaseJson json;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

bool eat = false;
int timeOutMin = 2; // minutes
int noBottleStart = 0;
const int LDR = A0;
const int ldrSet = 800;
int status = 0; // 0=ok  1=it's time 2=forgot 3=placeBottleBack
int previousStatus = 0;
int alarms;
bool sw = 0;
int ldrVal;

struct tm alarmTime[50];
struct tm timeNow;
struct tm timeOut;

void getTime(){
  timeClient.update();
  timeNow.tm_hour = timeClient.getHours();
  timeNow.tm_min = timeClient.getMinutes();
}

void getTimeSet(){
  if(Firebase.ready()){
    Firebase.RTDB.getJSON(&fbdo, "/alarms");
    json = fbdo.to<FirebaseJson>();
    size_t len = json.iteratorBegin();
    int hour, minute;
    alarms = 0;
    for(size_t i = 0;i < len;i++){
      FirebaseJson::IteratorValue value = json.valueAt(i);
      if(String(value.key.c_str()) == "hour") hour = atoi(value.value.c_str());
      if(String(value.key.c_str()) == "minute") {
        minute = atoi(value.value.c_str());
        //Serial.println(hour);
        //Serial.println(minute);
        alarmTime[alarms].tm_hour = hour;
        alarmTime[alarms++].tm_min = minute;
      }
    }
    json.iteratorEnd();
  }
  else Serial.println("Firebase not ready");
}

void notifyStatus(int sta){
  Serial.print("sending ");
  Serial.println(sta);
//  if(WiFi.status()== WL_CONNECTED){
//    HTTPClient http;
//    String serverPath = serverName;
//    if(sta == 1) serverPath += "/SMC.php?send=1";
//    if(sta == 2) serverPath += "/SMC.php?send=2";
//    if(sta == 0) serverPath += "/SMC.php?send=3";
//    if(sta == 3) serverPath += "/SMC.php?send=4";
//    http.begin(serverPath.c_str());
//    int httpResponseCode = http.GET();
//    if (httpResponseCode>0) {
//      //Serial.print("HTTP Response code: ");
//      //Serial.println(httpResponseCode);
//      String payload = http.getString();
//      //Serial.println(payload);
//    }
//    else {
//      Serial.print("Error code: ");
//      Serial.println(httpResponseCode);
//    }
//    http.end();
//  }
//  else {
//    Serial.println("WiFi Disconnected");
//  }
  //lastTime = millis();
  if(sta == 0) LINE.notify("OK");
  if(sta == 1) LINE.notify("Time to take medicine");
  if(sta == 2) LINE.notify("Forgot to take medicine, take it now");
  if(sta == 3) LINE.notify("Place Med Container back");
}

class Task1code : public Task {
public:
  void loop(){
    getTime();
    getTimeSet();
    Serial.print(timeNow.tm_hour);
    Serial.print(" ");
    Serial.println(timeNow.tm_min);
  
    for(int i = 0;i < alarms;i++){
      if(timeNow.tm_hour == alarmTime[i].tm_hour && timeNow.tm_min == alarmTime[i].tm_min){
        status = 1;
        Serial.println(status);
        timeOut.tm_hour = alarmTime[i].tm_hour + 0;
        timeOut.tm_min += alarmTime[i].tm_min + timeOutMin;
        if(timeOut.tm_min >= 60) {
          timeOut.tm_min -= 60; timeOut.tm_hour += 1;
        }
      }
    }
    ldrVal = analogRead(LDR);
    if(ldrVal < ldrSet) noBottleStart++;
    else noBottleStart = 0;
    if(noBottleStart >= 5){
      notifyStatus(3);
    }
    delay(60000);
  }
}task_1;

class Task2code : public Task {
public:
  void setup(){
    LINE.notify("start");
  }
  void loop(){
    if(status == 1){
      notifyStatus(1);
      eat = false;
      while(eat == false){
        ldrVal = analogRead(LDR);
        Serial.println(ldrVal);
        eat = true;
        for(int i = 0;i < 10;i++){
          ldrVal = analogRead(LDR);
          if(ldrVal >= ldrSet) eat = false;
          delay(100);
        }
        Serial.print("timeOut ");
        Serial.print(timeOut.tm_hour);
        Serial.print(" ");
        Serial.println(timeOut.tm_min);
        if(timeNow.tm_min == timeOut.tm_min && timeNow.tm_hour == timeOut.tm_hour && status != 2){
          status = 2;
          notifyStatus(2);
        }
      }
      status = 0;
      notifyStatus(0);
    }
    delay(1000);
  }
}task_2;

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  LINE.setToken(LINE_TOKEN);
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Firebase OK");
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  getTime();
  getTimeSet();
  Serial.print(timeNow.tm_hour);
  Serial.print(" ");
  Serial.println(timeNow.tm_min);

  Scheduler.start(&task_1);
  Scheduler.start(&task_2);
  Scheduler.begin();
}

void loop() {
}
