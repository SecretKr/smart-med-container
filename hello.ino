#include "BMP280.h"

#include "SHT31.h"

#include "MPU9250.h"

#include "commGPS.h"

#include "commRTC.h"

#include "commTF.h"

#include "avr/wdt.h"


#
define GPSser Serial3# define UHFser Serial1# define UHFser2 Serial2# define DUGser Serial# define bee 46# define ENABLE_SERVO 0

# define SERVO_CLOSE 75# define SERVO_OPEN 130

# define SF_TIME_PROC 15


# define TEST_IO_CFG 51
uint8_t test_io_falg = 0;

Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_BMP280 bmp;

String UHFout = "";
String fileName = "";
float airTemp, airHumility;

float basePressure, pressure, BmpTem, BmpAltitude;
float baseBmpAltitude = 0, bmpHight = 0, bmpHightBuf = 0;
int localBaseTime[6], localTime[6]; //y,m,d,h,f,s

uint32_t timeMark;

#
define I2Cclock 400000# define I2Cport Wire

MPU9250 mpu;

struct commGD gpsData;
float distance = 0, disbuf = 0, altbuf = 500;

#
define SETGPS 2000# define SETPRE 2000# define SETDIS 30000

# define NSETGPSBASE
float baseLat = 4315.00, baseLon = 11142.10;
int baseTime[2],

  baseAltitude = 0, gpsHight = 0;

uint8_t poweron_gps_flag = 0;

float allAcc;

uint32_t time5S_cnt = 0;

float batteryVoltage;

int cntNum = 0;


float data[40];

float axyz[3], gxyz[3], mxyz[3];

void setup() {
    Serial.begin(115200);
    UHFser.begin(115200);

    UHFser.println("|*------------*-------------*|");
    UHFser.println("|******  COMMSAT V0.9  ******|");
    UHFser.println("|******      Qsat2     ******|");
    UHFser.println("|******      H:30km    ******|");
    UHFser.println("|******     2021.5~7   ******|");
    UHFser.println("|*------------*-------------*|");
    UHFser.print("GPS timeout min:");
    UHFser.print(GPS_TIMEOUT_MIN);
    UHFser.println();

    RTCinit();
    digitalWrite(TEST_IO_CFG, HIGH);
    pinMode(TEST_IO_CFG, INPUT);
    delay(10);
    if (digitalRead(TEST_IO_CFG) == HIGH) {
      test_io_falg = 0;
      UHFser.println("WORK!!!");#
      if ENBALE_UHF2
      UHFser2.println("WORK!!!");#
      endif
    } else {
      test_io_falg = 1;
      UHFser.println("TEST!!!");#
      if ENBALE_UHF2
      UHFser2.println("TEST!!!");#
      endif

    }

    tone(bee, 500, 50);
    delay(70);
    tone(bee, 1000, 50);
    delay(70);
    tone(bee, 5000, 50);
    delay(70);
    tone(bee, 500, 50);
    delay(70);
    tone(bee, 1000, 50);
    delay(70);
    delay(1000); {
      uint16_t test_cnt = 0;
      while ((digitalRead(TEST_IO_CFG) == HIGH) && (test_cnt < 500)) {
        delay(10);
        test_cnt++;
      }
    }


    Wire.begin();

    {
      byte error, address;
      int nDevices;
      nDevices = 0;

      for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
          UHFser.println(address, HEX);#
          if ENBALE_UHF2
          UHFser2.println(address, HEX);#
          endif

          nDevices++;
        } else if (error == 4) {
          UHFser.print("Unknow error at address 0x");
          if (address < 16)
            UHFser.print("0");
          UHFser.println(address, HEX);
        }
      }
      if (nDevices == 0)
        UHFser.println("No I2C devices found");
      else {

      }
      delay(1000);
      UHFser.println("IIC check complete");

      #
      if ENBALE_UHF2
      UHFser2.println("IIC check complete");#
      endif
    }

    if (!sht31.begin(0x44)) {
      UHFser.println("Couldn't find SHT31");
    } else {
      UHFser.println("SHT init complete");
    }

    if (!bmp.begin()) {
      UHFser.println(F("Could not find a valid BMP280 sensor, check wiring or "
        }
        else {

          bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
            Adafruit_BMP280::SAMPLING_X2,
            Adafruit_BMP280::SAMPLING_X16,
            Adafruit_BMP280::FILTER_X16,
            Adafruit_BMP280::STANDBY_MS_500);
          UHFser.println("Pressure init complete");
        }

        GPS_init(); UHFser.println("GPS init complete");

        delay(200); mpu.setup(); delay(300); //5000
        mpu.calibrateAccelGyro();

        UHFser.println("IMU init complete");


        UHFser.println("ALL sensor init complete");


        UHFser.println("b4"); tfInit(); UHFser.println("after"); preFile( & fileName); UHFser.println(fileName); UHFser.println("begin");

        {
          BmpTem = bmp.readTemperature();
          delay(10);
          basePressure = bmp.readPressure();
          baseBmpAltitude = bmp.readAltitude(1013.25);
        }

        //获取后蜂鸣
        tone(bee, 500, 100); delay(120);

        # ifdef NSETGPSBASE location( & gpsData, test_io_falg);

        if (gps_timeout_flag == 1) {
          poweron_gps_flag = 1;
          UHFser.println("1.GPS OK");
          adjTime(gpsData.year, gpsData.month, gpsData.day, gpsData.hour, gpsData.minu, gpsData.sece);
        } else if (gps_timeout_flag == 2) {
          poweron_gps_flag = 0;
          UHFser.println("2.GPS timeout");
        } else if (gps_timeout_flag == 3) {
          poweron_gps_flag = 1;
          UHFser.println("3.GPS timeout");
          adjTime(gpsData.year, gpsData.month, gpsData.day, gpsData.hour, gpsData.minu, gpsData.sece);
        }

        baseLat = gpsData.latitude; baseLon = gpsData.longitude;

        baseAltitude = gpsData.altitude;# endif

        getTime( & localBaseTime[0], & localBaseTime[1], & localBaseTime[2], & localBaseTime[3], & localBaseTime[4], & localBaseTime[5]);
        for (int Ti = 0; Ti < 6; Ti++)
          UHFser.print(localBaseTime[Ti]); UHFser.println("");


        tone(bee, 1000, 100); delay(120); tone(bee, 1000, 100); delay(120); tone(bee, 1000, 100); delay(120);
        //----------------------

        wdt_enable(WDTO_2S);

        timeMark = millis() - 200;
      }

      void loop() {
        wdt_reset();
        //1
        sht31.orderTh();
        sht31.getTh( & airTemp, & airHumility);

        data[0] = airTemp;
        data[1] = airHumility;

        //static int thread2_entry(struct pt *pt){

        BmpTem = bmp.readTemperature();
        pressure = bmp.readPressure();

        BmpAltitude = bmp.readAltitude(1018.25);

        bmpHight = BmpAltitude - baseBmpAltitude;

        if (bmpHight - bmpHightBuf > 500)
          bmpHight = bmpHightBuf;
        else
          bmpHightBuf = bmpHight;

        data[2] = basePressure;
        data[3] = pressure;
        data[4] = BmpAltitude;


        //static int thread3_entry(struct pt *pt){
        unsigned long timeoutGps;
        double degLat, degLon, degBaseLat, degBaseLon;

        timeoutGps = millis();
        while (GPSser.available() && (millis() - timeoutGps < 1000)) {
          GPS_Parsing( & gpsData);
        }

        if ((gps_timeout_flag == 1) || (gps_timeout_flag == 3)) {
          gpsHight = gpsData.altitude - baseAltitude;

          degBaseLat = DM2DD(baseLat);
          degBaseLon = DM2DD(baseLon);
          degLat = DM2DD(gpsData.latitude);
          degLon = DM2DD(gpsData.longitude);

          distance = CalGPSDistance(degBaseLat, degBaseLon, degLat, degLon);

          if (distance - disbuf > 100)
            distance = disbuf;
          else
            disbuf = distance;
          if (gpsData.altitude - altbuf > 1000)
            gpsData.altitude = altbuf;
          else
            altbuf = gpsData.altitude;
        } else {
          if (gpsData.fix > 0) {
            if (poweron_gps_flag == 0) {
              poweron_gps_flag = 1;
              baseLat = gpsData.latitude;
              baseLon = gpsData.longitude;
              baseAltitude = gpsData.altitude;
            }

            if ((gpsData.HDOP > 3) && (gpsData.HDOP < 7)) {
              gps_timeout_flag = 3;
            } else if ((gpsData.HDOP > 0) && (gpsData.HDOP <= 3)) {
              gps_timeout_flag = 1;
            }

          }
        }

        data[5] = distance;
        data[6] = gpsData.latitude;
        data[7] = gpsData.longitude;
        data[8] = gpsData.altitude;
        data[9] = gpsData.sat;
        data[10] = gpsData.HDOP;

        data[11] = gpsData.fix;
        data[12] = gpsData.fixq;
        data[13] = gpsData.speed;
        data[14] = baseLat;
        data[15] = baseLon;
        data[16] = gpsData.hour;
        data[17] = gpsData.minu;
        data[18] = gpsData.sece;

        //static int thread4_entry(struct pt *pt){

        mpu.update();
        mpu.getRawData(axyz, gxyz, mxyz);

        allAcc = sqrt(axyz[0] * axyz[0] + axyz[1] * axyz[1] + axyz[2] * axyz[2]);

        data[19] = axyz[0];
        data[20] = axyz[1];
        data[21] = axyz[2];

        data[22] = gxyz[0];
        data[23] = gxyz[1];
        data[24] = gxyz[2];

        data[25] = mxyz[0];
        data[26] = mxyz[1];
        data[27] = mxyz[2];

        data[28] = mpu.getPitch();
        data[29] = mpu.getRoll();
        data[30] = mpu.getYaw();

        data[31] = allAcc;




        //static int thread5_entry(struct pt *pt){

        batteryVoltage = smoothFliter(analogRead(A9) * 0.00488) * 6;

        data[32] = batteryVoltage;


        //static int thread6_entry(struct pt *pt){
        getTime( & localTime[0], & localTime[1], & localTime[2], & localTime[3], & localTime[4], & localTime[5]);

        data[34] = localTime[3];
        data[35] = localTime[4];
        data[36] = localTime[5];


        //static int thread7_entry(struct pt *pt){             


        UHFout = "0:TMP:" + (String)(airTemp) + " 1:HUM:" + (String)(airHumility);
        UHFser.println(UHFout);
        UHFout = "";

        UHFout = "2:bPRS:" + (String)(basePressure) + " 3:PRS:" + (String)(pressure) + " 4:ALT:" + (String)(BmpAltitude);
        UHFser.println(UHFout);
        UHFout = "";

        UHFout = "5:DIS:" + (String)(distance) + " 6:LAT:" + (String)(gpsData.latitude) + " 7:LON:" + (String)(gpsData.longitude);
        UHFser.println(UHFout);
        UHFout = "";
        UHFout = "8:GALT:" + (String)(gpsData.altitude) + " 9:NSAT:" + (String)(gpsData.sat) + " 10:HDOP:" + (String)(gpsData.HDOP);
        UHFser.println(UHFout);
        UHFout = "";

        UHFout = "11:fix:" + (String)(gpsData.fix) + " 12:fixq:" + (String)(gpsData.fixq);
        UHFser.println(UHFout);
        UHFout = "";
        UHFout = "13:spd:" + (String)(gpsData.speed) + " 14:bLat:" + (String)(baseLat) + " 15:bLon:" + (String)(baseLon);
        UHFser.println(UHFout);
        UHFout = "";
        UHFout = "16:hour:" + (String)(gpsData.hour) + " 17:min:" + (String)(gpsData.minu) + " 18:sec:" + (String)(gpsData.sece);
        UHFser.println(UHFout);
        UHFout = "";
        UHFout = "19:AX:" + (String)(data[19]) + " 20:AY:" + (String)(data[20]);
        UHFser.println(UHFout);
        UHFout = "21:AZ:" + (String)(data[21]) + " 31:ACC:" + (String)(allAcc);
        UHFser.println(UHFout);
        UHFout = "";
        UHFout = "22:GX:" + (String)(data[22]) + " 23:GY:" + (String)(data[23]) + " 24:GZ:" + (String)(data[24]);
        UHFser.println(UHFout);
        UHFout = "";
        UHFout = "25:MX:" + (String)(data[25]) + " 26:MY:" + (String)(data[26]) + " 27:MZ:" + (String)(data[27]);
        UHFser.println(UHFout);
        UHFout = "";
        UHFout = "28:PIT:" + (String)(data[28]) + " 29:ROL:" + (String)(data[29]) + " 30:YAW:" + (String)(data[30]);
        UHFser.println(UHFout);
        UHFout = "";

        UHFout = "32:c5S:" + (String)(time5S_cnt) + " 33:bat:" + (String)(batteryVoltage) + " 34:bH:" + (String)(bmpHight) + " 35:gH:" + (String)(gpsHight);
        UHFser.println(UHFout);
        UHFout = "";


        //static int thread9_entry(struct pt *pt){   
        openFile();
        SwriteData("\r\ncode run:");

        iwriteData(time5S_cnt / 12);

        SwriteDataLn("min");
        SwriteData("\r\ncode speed:");
        iwriteData(millis() - timeMark);
        SwriteDataLn("ms");
        timeMark = millis();

        SwriteData("\r\nlocal Time");
        iwriteData(localTime[3]);
        SwriteData(":");
        iwriteData(localTime[4]);
        SwriteData(":");
        iwriteDataLn(localTime[5]);

        for (cntNum = 0; cntNum < 37; cntNum++) {

          iwriteData(cntNum);
          SwriteData(": ");
          writeData(data[cntNum]);
          SwriteData("\t");

        }

        SwriteDataLn("");
        SwriteDataLn("");
        closeFile();


        //static int thread10_entry(struct pt *pt){             

        //tone(bee,1000,100);
        time5S_cnt++;
      }

      # define fliterNumber 10
      float smoothFliter(float newRcVal) {


        static float arr[fliterNumber];
        static float sum;
        float outVal;

        sum = sum - arr[fliterNumber - 1];

        for (int i = fliterNumber - 1; i > 0; i--) {
          arr[i] = arr[i - 1];
        }

        arr[0] = newRcVal;

        sum = sum + arr[0];

        outVal = sum / fliterNumber;

        return outVal;
      }