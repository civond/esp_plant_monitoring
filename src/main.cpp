#include <Arduino.h>
#include <Adafruit_VEML7700.h>
#include <Adafruit_seesaw.h>
#include <Wire.h>
#include <WiFi.h>

// Define global variables here
#define MUX_Address 0x70
const int VEML7700_Channel = 0;
const int Stemma_Channel = 1;
Adafruit_VEML7700 veml = Adafruit_VEML7700();
Adafruit_seesaw ss;

// TCA Channel Select
void TCA_Select(uint8_t i2c_bus) {
  if (i2c_bus > 7) return;
  Wire.beginTransmission(MUX_Address);
  //Sensor_Delay();
  Wire.write(1 << i2c_bus);
  //Sensor_Delay();
  Wire.endTransmission();
}

void setup() {
  const int baud_rate = 115200;
  const char* ssid = "Verizon_9DV9GK";
  const char* password = "cowl4-salty-add";

  // Initialize Wire and Serial
  Wire.begin();
  Serial.begin(baud_rate);
  while (!Serial) {}

  // Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to: " + String(ssid));
  int i = 0;

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
  // If connected
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to Wifi network!");
    Serial.println("\tLocal ESP32 IP: " + String(WiFi.localIP()));
  }


  // TCA Scan (There are 8 channels)
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

void loop() {

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

