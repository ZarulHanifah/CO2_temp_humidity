
#include <ThreeWire.h>                                           // RTC DS1302
#include <RtcDS1302.h>
#include <SD.h>                                                  // SD card
#include <SPI.h>
#include <Adafruit_SCD30.h>                                      // SCD30
#include <U8glib.h>                                              // OLED

const int delayTime = 10;                                        // general init: seconds
ThreeWire myWire(7, 6, 8);                                       // RTC
RtcDS1302<ThreeWire> Rtc(myWire);
const int pinCS = 10;                                            // SD card
String filename;
Adafruit_SCD30 scd30;                                            // SCD30
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // OLED stuff
String message1;
String message2;
String message3;

void draw(void) {
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr(0, 20, "Damn");
}

String get_date(RtcDateTime rtcdatetime) {
  // uint32_t date = rtcdatetime.Day() + 100 * rtcdatetime.Month() + 10000 * rtcdatetime.Year());
  uint32_t date = rtcdatetime.Day();
  date = date + uint32_t(rtcdatetime.Month()) * 100 ;
  date = date + uint32_t(rtcdatetime.Year()) * 10000 ;
  return String(date);  
}

void setup() {
  u8g.firstPage(); do {
    message1 = "Initializing ...";
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0, 10, message1.c_str());
  } while (u8g.nextPage()); delay(delayTime);

  Serial.begin(9600);                           // just for logging  
  pinMode(pinCS, OUTPUT);                       // SD card pin
  Rtc.Begin();                                  // RTC
  RtcDateTime now = Rtc.GetDateTime();
  // filename = String(get_date(now)) + ".csv";    // file name

  // SCD30 init
  u8g.firstPage(); do {
    u8g.setFont(u8g_font_unifont);
    if (!scd30.begin()) {
      message1 = "Failed to find SCD30 chip";
      u8g.drawStr(0, 10, message1.c_str());
      while (1) { delay(10); }
    }
    message1 = "SCD30 Found!";
    u8g.drawStr(0, 10, message1.c_str());
    if (!scd30.setMeasurementInterval(delayTime)) {
      message1 = "Failed to set measurement interval";
      u8g.drawStr(0, 10, message1.c_str());
      while (1) { delay(10); }
    }
  } while (u8g.nextPage()); delay(delayTime);

  // SD card logging init
  u8g.firstPage(); do {
    if (!SD.begin(pinCS)) {
      message1 = "SD card failed init.";
      u8g.drawStr(0, 10, message1.c_str());
      return;
    }
    message1 = "SD card";
    message2 = "accessible!";
    u8g.drawStr(0, 10, message1.c_str());
    u8g.drawStr(0, 25, message2.c_str());

  } while (u8g.nextPage());
  
  delay(delayTime);

  // filename create/append
  u8g.firstPage(); do {
    RtcDateTime now = Rtc.GetDateTime();
    // uint32_t date = ((uint32_t)now.Day() + 100*(uint32_t)now.Month() + 10000*(uint32_t)now.Year());

    String fdate = get_date(now);
    String filename = String(fdate) + ".csv";
    Serial.println(fdate);
    Serial.println(filename);
    
    File myFile = SD.open(filename, FILE_WRITE);

    message1 = "File access";
    message2 = "successful";
    // message3 = filename;
    
    // if (! myFile) {
    //   Serial.println("Not writing. Cannot find " + filename);
    //   message1 = "Cannot create/ ";
    //   message2 = "access file";
    //   message3 = filename;
    // }
    
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0, 10, message1.c_str());  
    u8g.drawStr(0, 25, message2.c_str());
    u8g.drawStr(0, 40, filename.c_str());  
  } while (u8g.nextPage()); delay(delayTime);



  
}

// u8g.firstPage(); do {
//     u8g.setFont(u8g_font_unifont);
//     u8g.drawStr(0, 10, message1.c_str());  
// } while  (u8g.nextPage()); delay(delayTime);


 void loop() {
  
}