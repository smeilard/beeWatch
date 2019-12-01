/* For I2C communication */
#include <Wire.h>

// include the SD library:
#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 10;


/* RTC lib */
#include "RTClib.h"
RTC_DS1307 RTC;

/* BME280 lib*/
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

// Doc de l'eeprom utilisée (32 Ko) :
// http://www.belling.com.cn/media/file_object/bel_product/BL24C256A/datasheet/BL24C256A.pdf
#define EEPROM_I2C_ID 0x50
int address = 0;

unsigned long delayTime;

char filename[12];

void setup() {
  /* init console */
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  Wire.setClock(400000);

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

  delayTime = 300000;

  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  dumpEeprom2SD();
}

void newCSVfilename() {
  DateTime now = RTC.now();
  sprintf(filename, "%02d%02d%02d%02d.csv", now.year()-2000, now.month(), now.day(), now.hour());
  Serial.print ("Nouveau nom de fichier : ");
  Serial.println(filename);
}


void dumpEeprom2SD() {
  float temp, hum, pressure;
  uint32_t ts;
  byte *byte_ts = (byte *)(&ts);
  byte *byte_temp = (byte *) (&temp);
  byte *byte_hum = (byte *) (&hum);
  byte *byte_pressure = (byte *) (&pressure);
  int i = 0;
  newCSVfilename();
  Serial.print ("Sauvegardes des données dans le fichier : ");
  Serial.println(filename);
  File dataFile = SD.open(filename, FILE_WRITE);
  if ( dataFile == 0 ) {
    Serial.println("probleme d'ouverture de fichier");
  }



  Serial.println("Lecture de l'EEprom ...");
  do {
    Wire.beginTransmission(EEPROM_I2C_ID);
    Wire.write((int)(address >> 8));   // MSB
    Wire.write((int)(address & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_I2C_ID, 16); // on demande à lire 16 octets
    // Lecture du timestamp
    for (int i = 0; i < 4; i++) {
      if (Wire.available()) {
        byte_ts[i] = Wire.read();
      };
    }
    address += 4;
    // Lecture de la temperature
    for (int i = 0; i < 4; i++) {
      if (Wire.available()) {
        byte_temp[i] = Wire.read();
      };
    }
    address += 4;
    //Lecture taux humidite
    for (int i = 0; i < 4; i++) {
      if (Wire.available()) {
        byte_hum[i] = Wire.read();
      };
    }
    address += 4;
    //Lecture pression atmosphérique
    for (int i = 0; i < 4; i++) {
      if (Wire.available()) {
        byte_pressure[i] = Wire.read();
      };
    }
    address += 4;

    // if the file is available, write to it:
    if (dataFile) {
      String toBeLogged = "";
      toBeLogged += String(ts, DEC);
      toBeLogged += ",";

      toBeLogged += String(temp);
      toBeLogged += ",";


      toBeLogged += String(pressure / 100.0F);
      toBeLogged += ",";

      toBeLogged += String (hum);
      dataFile.println(toBeLogged);
      Serial.print(address, DEC);
      Serial.print( " : ");
      Serial.println(toBeLogged);
    }
    // TODO : 4096 : truc codé en dur, à corriger
  } while ( ts > 1546297200 && ts < 1893452400 && address <= 4096 ); // tant que ts est dans l'interval  [01/01/2019 - 01/01/2030](test moisi pour verifier que c'est bien unde datea
  if (dataFile) {
    dataFile.close();
    Serial.println ("Fermeture du fichier sauvegardé");
  }
  // on a tout ecrit sur la carte SD, on remet à zero l'adresse où ecrire dans l'eeprom
  address = 0;

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

void printDateTime() {
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
  float pressure = bme.readPressure();
  Serial.print("taille de la date : ");
  Serial.println(sizeof(ts));
  Serial.print("taille d'un float : ");
  Serial.println(sizeof(temp));
  Wire.beginTransmission(EEPROM_I2C_ID);
  Wire.write((int)((address) >> 8));   // MSB
  Wire.write((int)((address) & 0xFF)); // LSB
  Wire.write((byte *)&ts, sizeof(ts)); // write timestamp (4 bytes)
  Wire.write((byte *)&temp, sizeof(temp)); // write temperature (4 bytes)
  Wire.write((byte *)&hum, sizeof(hum)); // write humidity (4 bytes)
  Wire.write((byte *)&pressure, sizeof(pressure)); // write timestamp (4 bytes)
  Wire.endTransmission();
  address += 16;
  if (address >= 4096 ) {
    dumpEeprom2SD();
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
