#include <Arduino.h>
#include <Adafruit_VEML7700.h>
#include <Adafruit_seesaw.h>
#include <Wire.h>
#include <WiFi.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// SSD1306
#include "./ssd1306_driver/U8x8lib.h"
#ifdef U8X8_HAVE_HW_I2C
#endif
//#ifdef U8X8_HAVE_HW_SPI
//#endif
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 
TwoWire myWire = TwoWire(1); // Use Wire1 (second I2C bus)

// Utility functions
#include "utils.h"
#include "config.h"

// SD card pins

/*#define SCK  18
#define MISO  19
#define MOSI  23
#define CS  5
SPIClass spi = SPIClass(VSPI);
*/

// Sensors
const int VEML7700_Channel = 0;
const int Stemma_Channel = 1;
const char* write_dir = "/logs";
const int baud_rate = 115200;

void pre(void)
{
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);    
  u8x8.clear();

  u8x8.inverse();
  u8x8.print("  Plant Stats  ");
  u8x8.setFont(u8x8_font_chroma48medium8_r);  
  u8x8.noInverse();
  u8x8.setCursor(0,1);
}


// Setup function
void setup() {

  // Initialize Wire and Serial
  Wire.begin();
  myWire.begin(32, 33);
  u8x8.begin();

  Serial.begin(baud_rate);
  while (!Serial) {}
  Serial.println("\n-------START SETUP-------");

  // SSD1306
  pre();

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

  // Write to OLED
  u8x8.setCursor(0,2);
  u8x8.print("Lux: " + String(lux));

  u8x8.setCursor(0,3);
  u8x8.print("Temp: " + String(tempC));

  u8x8.setCursor(0,4);
  u8x8.print("Cap: " + String(capread));

  delay(1000);
}

