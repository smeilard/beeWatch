/* For I2C communication */
#include <Wire.h>

/* RTC lib */
#include "RTClib.h"
RTC_DS1307 RTC;

/* BME280 lib*/
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

unsigned long delayTime;
char toBeLogged[50];

#include "extEEPROM.h"
extEEPROM myEEPROM(kbits_32, 1, 64, 0x58);
unsigned long address = 0;

void setup() {
  /* init console */
    Serial.begin(9600);
    Serial.println(F("BME280 test"));

    /* BME280 initialisation */
    bool status;
    status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    Serial.println("BME280 sensor initialized");

    Serial.println();

    /* TinyRTC initialisation */
    RTC.begin();
    if (! RTC.isrunning()) {
      Serial.println("RTC is NOT running!");
      RTC.adjust(DateTime(__DATE__, __TIME__));
    }
  
    byte i2cStat = myEEPROM.begin(myEEPROM.twiClock100kHz);
    if ( i2cStat != 0 ) {
      Serial.println(F("I2C Problem"));
    }

    delayTime = 5000;
}

void printBME280Values() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
}

void printDateTime(){
    DateTime now = RTC.now(); 
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println(); 
}

void log2eeprom() {
  uint32_t ts = RTC.now().unixtime();
  float temp = bme.readTemperature();
  float hum = bme.readHumidity();
  memset(toBeLogged, 0, sizeof(toBeLogged));
  snprintf(toBeLogged, sizeof(toBeLogged), "%d;%f;%f\n", ts, temp, hum);
  byte i2cStat = myEEPROM.write(address, toBeLogged, strlen(toBeLogged));
    if ( i2cStat != 0 ) {
    //there was a problem
    Serial.print(F("I2C Problem: "));
    if ( i2cStat == EEPROM_ADDR_ERR) {
      Serial.println(F("Wrong address"));
    } else {
      Serial.print(F("I2C error: "));
      Serial.print(i2cStat);
      Serial.println(F(""));
    }
  } else {
    address += strlen(toBeLogged);
    Serial.println("Ecriture ok");
  }
}
void loop() {
  printDateTime();
  printBME280Values();
  log2eeprom();
  delay(delayTime);
  Serial.println(); 
  Serial.println(); 
}
