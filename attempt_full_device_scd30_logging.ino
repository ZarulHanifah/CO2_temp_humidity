
// general init values
int delayTime = 10; // seconds
String filename;

// RTC DS1302
#include <ThreeWire.h>
#include <RtcDS1302.h>
ThreeWire myWire(7, 6, 8);        // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);    // RTC Object


// SD card log
#include <SD.h>
#include <SPI.h>
int pinCS = 10; // for SD card logging

// and SCD30
#include <Adafruit_SCD30.h>
Adafruit_SCD30  scd30;

void setup() {
  Serial.begin(9600);
  pinMode(pinCS, OUTPUT);
  
  // setup RTC DS1302
  Rtc.Begin();
  RtcDateTime now = Rtc.GetDateTime();
  uint32_t date = ( (uint32_t)now.Day() + 100*(uint32_t)now.Month() + 10000*(uint32_t)now.Year());
  uint32_t time = ( (uint32_t)now.Second() + 100*(uint32_t)now.Minute() + 10000*(uint32_t)now.Hour() );
  
  filename = String(date) + ".csv";
  // Serial.println(filename);
  // setup SCD30
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  // Try to initialize!
  if (!scd30.begin()) {
    Serial.println("Failed to find SCD30 chip");
    while (1) { delay(10); }
  }
  Serial.println("SCD30 Found!");

  if (!scd30.setMeasurementInterval(delayTime)){
    Serial.println("Failed to set measurement interval");
    while(1){ delay(10);}
  }

  // SD card logging

  if (! SD.begin(pinCS)) {
    Serial.println("SD card failed init.");
    return ;
  }

  // init records
  Serial.print("Measurement Interval: "); 
  Serial.print(scd30.getMeasurementInterval()); 
  Serial.println(" seconds");

  Serial.print("Date: ");
  Serial.print(now.Day()); Serial.print("/");
  Serial.print(now.Month()); Serial.print("/");
  Serial.print(now.Year()); Serial.println("");
  
  // SD logging
  File myFile = SD.open(filename, FILE_WRITE);

  if (! myFile) {
    Serial.println("Not writing. Cannot find " + filename);
  }

  Serial.println("Time\tTemperature (C)\tHumidity (%)\t CO2(ppm)");

  myFile.println();

  myFile.print("Date: ");
  myFile.print(now.Day()); myFile.print("/");
  myFile.print(now.Month()); myFile.print("/");
  myFile.print(now.Year()); myFile.println("");

  myFile.print("Measurement Interval: "); 
  myFile.print(scd30.getMeasurementInterval()); 
  myFile.println(" seconds");

  myFile.println("Time\tTemperature (C)\tHumidity (%)\t CO2(ppm)");
  myFile.close();
  
}

void loop() {
  if (scd30.dataReady()){
    // get time
    RtcDateTime now = Rtc.GetDateTime();
    
    if (now.Hour() < 10)
      Serial.print("0");
    Serial.print(now.Hour());
    Serial.print(":");

    if (now.Minute() < 10)
      Serial.print("0");
    Serial.print(now.Minute());
    Serial.print(":");

    if (now.Second() < 10)
      Serial.print("0");
    Serial.print(now.Second());
    Serial.print("\t");

    // get SCD30 data
    if (!scd30.read()){ Serial.println("Error reading sensor data"); return; }

    float temperature = scd30.temperature;
    float humidity = scd30.relative_humidity;
    float co2 = scd30.CO2;
    
    Serial.print(temperature); Serial.print("\t");
    Serial.print(humidity); Serial.print("\t");
    Serial.print(co2, 3); Serial.print("\t");
    Serial.println();

    File myFile = SD.open(filename, FILE_WRITE);
    
    if (! myFile) {
      return ;
    }

    if (now.Hour() < 10)
      myFile.print("0");
    myFile.print(now.Hour());
    myFile.print(":");

    if (now.Minute() < 10)
      myFile.print("0");
    myFile.print(now.Minute());
    myFile.print(":");

    if (now.Second() < 10)
      myFile.print("0");
    myFile.print(now.Second());
    myFile.print("\t");

    myFile.print(temperature); myFile.print("\t");
    myFile.print(humidity); myFile.print("\t");
    myFile.print(co2, 3); myFile.print("\t");
    myFile.println();
    myFile.close();
  }
  delay(delayTime * 1000);
}


