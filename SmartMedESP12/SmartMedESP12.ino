#include <TaskScheduler.h>
Scheduler runner;
void task1code();
void task2code();
void task3code();
Task Task1(20000, TASK_FOREVER, &task1code);
Task Task2(1000, TASK_FOREVER, &task2code);
Task Task3(250, TASK_FOREVER, &task3code);
#include <ESP8266HTTPClient.h>
String serverName = "http://ikwmystery.atwebpages.com";

// Wifi
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
//char* ssid = "ff8";
//char* password = "Tekanawww9735";

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
int avg;
int eat_avg;
int timeOutMin = 2; // minutes
int noBottleStart = 0;
int lastEat = 100;
const int LDR = A0;
const int ldrSet = 800;
int status = 0; // 0=ok  1=it's time 2=forgot 3=placeBottleBack
int previousStatus = 0;
int alarms;
bool sw = 0;
int ldrVal;
int nAlarm;

#include <EEPROM.h>
#define pinLed 10
#define pinBuzz 16
bool ledOn = true;
bool buzzOn = false;
bool ledBlink = false;
bool buzzBlink = false;

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

void getSetting(){
  if(Firebase.ready()){
    bool ledOnSetting, buzzOnSetting;
    Firebase.RTDB.getBool(&fbdo, "led", &ledOnSetting);
    Firebase.RTDB.getBool(&fbdo, "buzzer", &buzzOnSetting);
    if(ledOnSetting != ledOn){
      ledOn = ledOnSetting;
      EEPROM.put(0, ledOn);
      Serial.print("led ");
      Serial.println(ledOn);
      EEPROM.commit();
    }
    if(buzzOnSetting != buzzOn){
      buzzOn = buzzOnSetting;
      EEPROM.put(1, buzzOn);
      Serial.print("buzz ");
      Serial.println(buzzOn);
      EEPROM.commit();
    }
  }
}

void setLogs(int ho, int mi, int et){
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm * ptm = localtime ((time_t *)&epochTime); 
  FirebaseJson json;
  int year = ptm->tm_year+1900;
  int month = ptm->tm_mon+1;
  int date = ptm->tm_mday;
  json.set("year", year);
  json.set("month", month);
  json.set("date", date);
  json.set("hour", ho);
  json.set("minute", mi);
  json.set("eat-time", et);
  String sMonth, sDate;
  if(month < 10) sMonth = "0"+String(month);
  else sMonth = String(month);
  if(date < 10) sDate = "0"+String(date);
  else sDate = String(date);
  if(!Firebase.RTDB.set(&fbdo, "logs/"+ String(year)+ sMonth+ sDate+ String(ho)+ String(mi), &json)) Serial.println("Firebase set log error");
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
  if(sta == 2) LINE.notify("Forgot to take medicine");
  if(sta == 3) LINE.notify("Place Med Container back");
}

void task1code(){
  getTime();
  getTimeSet();
  Serial.print(timeNow.tm_hour);
  Serial.print(" ");
  Serial.println(timeNow.tm_min);

  if(lastEat > 10) for(int i = 0;i < alarms;i++){
    if(timeNow.tm_hour == alarmTime[i].tm_hour && timeNow.tm_min == alarmTime[i].tm_min){
      status = 1;
      lastEat = 0;
      Serial.println(status);
      nAlarm = i;
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
  if(noBottleStart >= 2){
    notifyStatus(3);
    digitalWrite(pinLed, 1);
  }
  else digitalWrite(pinLed, 0);
  lastEat++;
  getSetting();
}

void task2code(){
  if(status == 1){
    notifyStatus(1);
    eat = false;
    int start_wait = millis();
    if(ledOn) digitalWrite(pinLed, 1);
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
        eat = true;
      }
    }
    if(status != 2){
      avg = int(avg*0.2+(millis()-start_wait)/1000*0.8);
      Firebase.RTDB.setInt(&fbdo, "/avg", avg);
    }
    int start_eat = millis();
    int eat_t = start_eat;
    ldrVal = analogRead(LDR);
    while(ldrVal < ldrSet){
      delay(1000);
      if(millis() - eat_t > eat_avg*1000+60000){
        notifyStatus(3);
        eat_t += 30000;
      }
      ldrVal = analogRead(LDR);
    }
    if(start_eat == eat_t){
      int eatingTime = (millis()-start_eat)/1000;
      eat_avg = int(eat_avg*0.2+eatingTime*0.8);
      Firebase.RTDB.setInt(&fbdo, "/eat-avg", eat_avg);
      setLogs(alarmTime[nAlarm].tm_hour, alarmTime[nAlarm].tm_min, eatingTime);
    }
    digitalWrite(pinLed, 0);
    status = 0;
    notifyStatus(0);
  }
}

void task3code(){
  if(ledBlink){
    digitalWrite(pinLed, 1);
    delay(250);
    digitalWrite(pinLed, 0);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pinLed, OUTPUT);
  pinMode(pinBuzz, OUTPUT);
  digitalWrite(pinLed, 1);
  digitalWrite(pinBuzz, 0);
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("AutoConnectAP","password");
  if(!res){
    Serial.println("Failed to connect");
    ledBlink = true;
  }
  else{
    Serial.println("Connected");
    ledBlink = false;
    digitalWrite(pinLed, 0);
  }

  EEPROM.begin(32);
  EEPROM.get(0, ledOn);
  EEPROM.get(1, buzzOn);
  Serial.println("led/buzz setting");
  Serial.println(ledOn);
  Serial.println(buzzOn);
  
  timeClient.begin();
  LINE.setToken(LINE_TOKEN);
  Serial.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Firebase OK");
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
    ledBlink = true;
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  getTime();
  getTimeSet();
  getSetting();
  Serial.print("timeNow ");
  Serial.print(timeNow.tm_hour);
  Serial.print(" ");
  Serial.println(timeNow.tm_min);
  if(timeNow.tm_hour == 7 && timeNow.tm_min == 0) ledBlink = true;

  // get avg
  if(!Firebase.RTDB.getInt(&fbdo, "/avg", &avg)) Serial.println("get avg error");
  else{
    Serial.print("avg ");
    Serial.println(avg);
  }
  if(!Firebase.RTDB.getInt(&fbdo, "/eat-avg", &avg)) Serial.println("get eat-avg error");
  else{
    Serial.print("eat-avg ");
    Serial.println(avg);
  }

  runner.addTask(Task1);
  Task1.enable();
  runner.addTask(Task2);
  Task2.enable();
  runner.addTask(Task3);
  if(ledBlink) Task3.enable();
}

void loop() {
  runner.execute();
}
