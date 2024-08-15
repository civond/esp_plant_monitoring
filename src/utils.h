#include <Wire.h>
#include "FS.h"
#include "SD.h"
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

// Make Base Directory on SD Card
void createDir(fs::FS &fs, const char * path) {
    Serial.printf("\tCreating Dir: %s\n", path);
    if (fs.exists(path)) {
        Serial.println("\t" + String(path) + " already exists.");
    } else if (fs.mkdir(path)) {
      Serial.println("\t" + String(path) + " created!");
    } else {
      Serial.println("\tmkdir failed");
  }
}

// Initialize WiFi
void start_wifi(const char* ssid, const char* password) {
  // Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to: " + String(ssid));
  int i = 0;

  // If not connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
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