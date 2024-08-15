#include <Arduino.h>
#include <Adafruit_VEML7700.h>
#include <Adafruit_seesaw.h>
#include <Wire.h>
#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Utility functions
#include "utils.h"

// Define global variables here
#define MUX_Address 0x70

// SD card pins

/*#define SCK  18
#define MISO  19
#define MOSI  23
#define CS  5
SPIClass spi = SPIClass(VSPI);
SdExFat sd;*/

// Sensors
const int VEML7700_Channel = 0;
const int Stemma_Channel = 1;
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_seesaw ss;

// Setup function
void setup() {
  const int baud_rate = 115200;
  const char* ssid = "SSID";
  const char* password = "password";

  // Initialize Wire and Serial
  Wire.begin();
  Serial.begin(baud_rate);
  while (!Serial) {}

  // Wifi
  start_wifi(ssid, password);

  // Initialize SD Card
  //spi.begin(SCK, MISO, MOSI, CS);

  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  } else {
    Serial.println("SD Card Mounted.");
  }
  createDir(SD, "/logs");

  // Scan TCA9548A channels for i2c devices
  TCA_Scan();

  // VEML7700
  TCA_Select(VEML7700_Channel);
  if (!veml.begin()) {
    Serial.println("VEML7700 not found.");
  } else {
    Serial.println("VEML7700 initialized!");
  }
  veml.setGain(VEML7700_GAIN_1_8);
  veml.setIntegrationTime(VEML7700_IT_100MS);
  veml.powerSaveEnable(true);
  veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);
  veml.enable(true);


  // Stemma
  TCA_Select(Stemma_Channel);
  if (!ss.begin(0x36)) {
    Serial.println("Stemma not found.");
  } else {
    Serial.println("Stemma initialized!");
  }

}

// Main
void loop() {
  // Gather reading every 2 seconds
  // Implement a circular buffer (~30 points)
  
  // VEML7700
  TCA_Select(VEML7700_Channel);
  float lux = veml.readLux();
  Serial.println("VEML7700:");
  Serial.println("\tLux: " + String(lux));

  // Stemma
  TCA_Select(Stemma_Channel);
  Serial.println("Stemma:");
  float tempC = ss.getTemp();
  uint16_t capread = ss.touchRead(0);

  Serial.println("\tTemp: " + String(tempC) + "; Moisture: " + String(capread));


  delay(2000);
}

