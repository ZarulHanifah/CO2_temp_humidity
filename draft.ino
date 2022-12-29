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

int p = 0;
void pageCounter();     // Loop-page-1
void pageLoop();
void (*pages[])() = { pageLoop } ;
int delayTime = 10;                      // seconds

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
  check_open_file();
} 

void loop() {
  u8g.firstPage();
  
  do {  
    pageLoop();
  } while( u8g.nextPage() );
  delay(delayTime * 1000);
  Serial.println(counter);
  counter++;
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

void pageLoop() {
  now = Rtc.GetDateTime();
  String f_time = get_time(now);
  // u8g.drawStr( 0, 10, f_time.c_str());

  u8g.setPrintPos(0, 10);
  u8g.print(f_time);

  if (!scd30.read()){ Serial.println("Error reading sensor data"); return; }

  scd30.getTemperatureSensor();
  scd30.getHumiditySensor();
  
  float temperature = scd30.temperature;
  float humidity = scd30.relative_humidity;
  float co2 = scd30.CO2;

  // CO2
  float f_co2;
  if (! isnan(co2)) {
    f_co2 = co2;
    Serial.println(f_co2);
  }
  
  u8g.setPrintPos(0, 25);
  u8g.print(String(f_co2) + " ppm CO2");
  
  // temp
  float f_temperature;
  if (! isnan(temperature)) {
    f_temperature = temperature;
    Serial.println(f_temperature);
  }

  u8g.setPrintPos(0, 40);
  u8g.print(String(f_temperature) + " oC");

  // humidity
  
  return 0;
}

void pageCounter() {
  String counter1 = String(counter);
  u8g.drawStr( 0, 15, counter1.c_str());
  counter++;
  // delay(delayTime * 1000);
  return 0; 
}

void check_SCD30_detect() {
  String message = "SCD30 found!";
  if (!scd30.begin()) {
    message = "Fail to find SCD30";
  }
  Serial.println(message);
  return 0;
}

void check_SCD30_set_interval() {
  String message = "Setting measurement interval";
  if (!scd30.setMeasurementInterval(delayTime)) {
    message = "Fail setting measurement interval";
  }
  Serial.println(message);
  return 0;
}

void check_SD_access() {
  String message = "SD card accessible";
  if (!SD.begin(pinCS)) {
    message = "SD card failed init";
  }
  Serial.println(message);
}

void check_open_file() {
  File myFile = SD.open(filename, FILE_WRITE);
  String message = " file access successful";
  if (! myFile) {
    message = " file access failed";
  }
  Serial.print(filename);
  Serial.println(message);

}

