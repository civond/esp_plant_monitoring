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
#include "config.h"

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
const char* write_dir = "/logs";
//const char* ssid = "Verizon_9DV9GK";
//const char* password = "cowl4-salty-add";
const int baud_rate = 115200;


// Setup function
void setup() {

  // Initialize Wire and Serial
  Wire.begin();
  Serial.begin(baud_rate);
  while (!Serial) {}
  Serial.println("\n-------START SETUP-------");
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

  // Initialize sensors
  float lux = 0.0f;
  float tempC = 0.0f;
  uint16_t capread = 0;

  start_veml(VEML7700_Channel, lux); // Lux sensor
  start_stemma(Stemma_Channel, tempC, capread); // Moisture sensor

  Serial.println("-------END-------\n");
}

// Main
void loop() {
  // Gather reading every 2 seconds
  // Implement a circular buffer (~30 points)
  
  // VEML7700
  TCA_Select(VEML7700_Channel);
  float lux = veml.readLux();

  // Stemma
  TCA_Select(Stemma_Channel);
  float tempC = ss.getTemp();
  uint16_t capread = ss.touchRead(0);

  // Generate path and write data to .txt file
  int numFiles = countFilesInDirectory(write_dir);
  String write_path = String(write_dir) + "/" + String(numFiles) + ".txt";
  String data = "Lux:" + String(lux) + ";Temp:" + String(tempC) +";Moisture:" + String(capread);

  writeFile(SD, write_path.c_str(), data.c_str());
  Serial.println(String(write_path));
  Serial.println("\tLux: " + String(lux) + "; Temp: " + String(tempC) + "; Moisture: " + String(capread));
  delay(2000);
}

