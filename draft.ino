#include <ThreeWire.h>                                           // RTC DS1302
#include <RtcDS1302.h>
#include <SD.h>                                                  // SD card
#include <SPI.h>
#include <Adafruit_SCD30.h>                                      // SCD30
#include <U8glib.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);
ThreeWire myWire(7, 6, 8);                                       // RTC
RtcDS1302<ThreeWire> Rtc(myWire);
const int pinCS = 10;

Adafruit_SCD30 scd30;

int counter = 0;
RtcDateTime now;
String filename;
// File myFile;

int p = 0;
int delayTime = 5;                      // seconds

void setup() {  
  u8g.setFont(u8g_font_unifont);
  u8g.setColorIndex(1);
  Rtc.Begin();
  Serial.begin(9600);
  
  now = Rtc.GetDateTime();
  filename = get_date(now) + ".csv";
  
  check_SCD30_detect();
  check_SCD30_set_interval();
  check_SD_access();
  // check_open_file();

  Serial.println("#Time\tTemperature (C)\tHumidity (%)\t CO2(ppm)");
  
  File myFile = SD.open(filename, FILE_WRITE);

  if (! myFile) {
    Serial.println("Not writing. Cannot find " + filename);
  }
  
  myFile.println("#Time\tTemperature (C)\tHumidity (%)\t CO2(ppm)");
  myFile.close();
} 

void loop() {
  if (scd30.dataReady()){
    // time
    now = Rtc.GetDateTime();
    String f_time = get_time(now);
    
    if (!scd30.read()){ Serial.println("Error reading sensor data"); return; }
    
    // SCD30 data
    float co2 = scd30.CO2;
    float temperature = scd30.temperature;
    float humidity = scd30.relative_humidity;
    
    u8g.firstPage();
    do {  
      pageLoop(f_time, co2, temperature, humidity);
    } while( u8g.nextPage() );
    
    Serial.print(f_time); Serial.print("\t");
    Serial.print(co2, 3); Serial.print("\t");
    Serial.print(temperature); Serial.print("\t");
    Serial.print(humidity); Serial.print("\t");
    Serial.println();

    File myFile = SD.open(filename, FILE_WRITE);
    
    if (! myFile) {
      Serial.println("Not writing. Cannot find " + filename);
    }
    myFile.print(f_time); myFile.print("\t");
    myFile.print(co2, 3); myFile.print("\t");
    myFile.print(temperature); myFile.print("\t");
    myFile.print(humidity); myFile.print("\t");
    myFile.println();

    myFile.close();
    
    delay(delayTime * 1000);
  }
}

int pageLoop(String time, float co2, float temperature, float humidity) {
  u8g.setPrintPos(0, 10); u8g.print(time);

  u8g.setPrintPos(0, 25); u8g.print(co2);
  u8g.setPrintPos(72, 25); u8g.print("ppm CO2");

  u8g.setPrintPos(0, 40); u8g.print(temperature);
  u8g.drawCircle(72, 32, 2);
  u8g.setPrintPos(78, 40); u8g.print("C");

  u8g.setPrintPos(0, 55); u8g.print(humidity);
  u8g.setPrintPos(72, 55); u8g.print("\% hum.");
}

void check_SCD30_detect() {
  Serial.print("SCD30: ");
  String message = "found!";
  if (!scd30.begin()) {
    message = "not found";
  }
  Serial.println(message);
  return 0;
}

void check_SCD30_set_interval() {
  Serial.print("SCD30: setting measurement interval ");
  String message = "pass";
  if (!scd30.setMeasurementInterval(delayTime)) {
    message = "fail";
  }
  Serial.println(message);
  return 0;
}

void check_SD_access() {
  Serial.print("SD card: ");
  String message = "found!";
  if (!SD.begin(pinCS)) {
    message = "not found";
  }
  Serial.println(message);
}

void check_open_file() {
  Serial.print("SD card: ");
  Serial.print(filename);
  Serial.print(" file access ");
  File myFile = SD.open(filename, FILE_WRITE);
  // myFile = SD.open(filename, FILE_WRITE);
  String message = "successful";
  if (! myFile) {
    message = "failed";
  }
  Serial.println(message);
}

String get_time(RtcDateTime rtcdatetime) {
  uint32_t hour = rtcdatetime.Hour();
  uint32_t minute = rtcdatetime.Minute();
  uint32_t second = rtcdatetime.Second();

  String f_hour = String(hour);
  if (hour < 10) {f_hour = "0" + f_hour;}

  String f_minute = String(minute);
  if (minute < 10) {f_minute = "0" + f_minute;}

  String f_second = String(second);
  if (second < 10) {f_second = "0" + f_second;}

  String f_time = f_hour + ":" + f_minute + ":" + f_second;
  return f_time;  
}

String get_date(RtcDateTime rtcdatetime) {
  uint32_t date = rtcdatetime.Day();
  date = date + uint32_t(rtcdatetime.Month()) * 100 ;
  date = date + uint32_t(rtcdatetime.Year()) * 10000 ;
  return String(date);  
}