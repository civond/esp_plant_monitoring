#include <Wire.h>
#include "FS.h"
#include "SD.h"
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_seesaw ss;
#define MUX_Address 0x70


// TCA9548A Channel Select
void TCA_Select(uint8_t i2c_bus) {
  if (i2c_bus > 7) return;
  Wire.beginTransmission(MUX_Address);
  //Sensor_Delay();
  Wire.write(1 << i2c_bus);
  //Sensor_Delay();
  Wire.endTransmission();
}


// TCA9548A Scan (there are 8 channels)
void TCA_Scan() {
  for (uint8_t t=0; t<8; t++) {
    TCA_Select(t);
    //Serial.println("TCA Port: " + String(t));
    for (uint8_t addr=0; addr<=127; addr++) {
      if (addr == MUX_Address) continue;
      Wire.beginTransmission(addr);
      if (!Wire.endTransmission()) {
        Serial.println("TCA Port: " + String(t));
        Serial.println("\tFound I2C 0x" + String(addr, HEX));
      }
    }
  }
}


// Initialize VEML7700
void start_veml(const int VEML7700_Channel, float lux) {
  TCA_Select(VEML7700_Channel);
  if (!veml.begin()) {
    Serial.println("VEML7700 not found.");
  } else {
    Serial.println("VEML7700 initialized with lux: " + String(lux));
  }

  veml.setGain(VEML7700_GAIN_1_8);
  veml.setIntegrationTime(VEML7700_IT_100MS);
  veml.powerSaveEnable(true);
  veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);
  veml.enable(true);
}


// Initialize Stemma
void start_stemma(const int Stemma_Channel, float tempC, uint16_t capread) {
  TCA_Select(Stemma_Channel);
  if (!ss.begin(0x36)) {
    Serial.println("Stemma not found.");
  } else {
    Serial.println("Stemma initialized with temp: " + String(tempC) + "; moisture: " + String(capread));
  }
}


// Make Base Directory on SD Card
void createDir(fs::FS &fs, const char * path) {
    if (fs.exists(path)) {
        Serial.println("\t" + String(path) + " dir already exists.");
    } else if (fs.mkdir(path)) {
      Serial.println("\t" + String(path) + " created!");
    } else {
      Serial.println("\tmkdir failed");
  }
}


// Count files in directory
int countFilesInDirectory(const char* write_dir) {
  int fileCount = 0;
  File dir = SD.open(write_dir);
  
  if (!dir) {
    Serial.println("Failed to open " + String(write_dir));
    return -1;
  }

  // Check if it's a directory
  if (!dir.isDirectory()) {
    Serial.println(String(write_dir) + " is not a directory");
    return -1;
  }

  File file = dir.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      fileCount++;
    }
    file = dir.openNextFile();
  }
  dir.close();

  return fileCount;
}


// Write a file on SD card
void writeFile(fs::FS &fs, const char * path, const char * message){
  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
  } else {
    Serial.println("\tWrite failed");
  }
  file.close();
}

// Initialize WiFi
void start_wifi(const char* ssid, const char* password) {
  // Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to: " + String(ssid));
  int i = 0;

  // If not connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    i++;
    delay(100);
    // Break if 30 retries
    if (i>30) {
      Serial.println("Failed to connect to WiFi (Check SSID / Pass)");
      break;
    }
  }

  // If connected to WiFi
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to Wifi network!");
    Serial.println("\tLocal ESP32 IP: " + String(WiFi.localIP()));
  }
}