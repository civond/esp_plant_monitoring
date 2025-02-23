
<h1>Plant Monitoring</h1>
   
<div>
  This is an ESP32-based plant monitoring project created using PlatformIO and the Arduino framework. It has the functionality of collecting sensor data for soil temperature, soil humidity ambient light which are stored on a micro SD card. Current data is also displayed on the SSD 1306 screen. The plant data can be pushed in real time to the internet using the ESP32's built-in wifi capability to create visualizations on the environment over time.
  <br/><br/>
   
   In the future, I plan on incorporating additional sensors to measure ambient temperature and humidity (ex: SHT30) and establish a web repository where recent data is displayed and perhaps automatic watering once soil humidity reaches a critical dryness.
  <br/><br/>
</div>

<h2>Wiring Diagram</h2>
<div align="center">
  <img src="PinLayout.png" alt="Pin Layout" align=center width=500px></img>
</div>

<div>
  <h3>Parts Used:</h3>
  <ol>
    <li>ESP-WROOM-32 Dev kit</li>
    <li>SSD1306 128x64 OLED LCD Screen (I2C)</li>
    <li>STEMMA Soil Sensor</li>
    <li>MicroSD SPI Breakout Board</li>
    <li>TCA9548A Multiplexer</li>
    <li>VEML7700 Ambient Light Sensor</li>
  </ol>

  <h3>Dependencies:</h3>
  <ul>
    <li>Adafruit seesaw</li>
    <li>Adafruit VEML7700</li>
    <li>Adafruit SSD1306</li>
  </ul>
</div>
