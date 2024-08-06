### Repository Content
* **/firmware** : Arduino example codes (.ino)
* **/hardware** : Schematic (.pdf)
* **/images** : Images (.png)
* **/production** : gerber file for pcb manufacturing (.zip)

You can purchase this product from [![Generic badge](https://img.shields.io/badge/Indonesia-Tokopedia-<COLOR>.svg)](https://www.tokopedia.com/geraicerdas/cerdas-gps-tracker-with-sim800-bno08x-and-rtc-esp32-s3) 
[![Generic badge](https://img.shields.io/badge/Worldwide-Tindie-blue)](https://www.tindie.com/products/geraicerdas/cerdas-uwb-tracker/)

# Cerdas GPS Tracker
The Cerdas GPS Tracker is an open-source development board designed for innovators building GPS tracker projects or related applications. Powered by the ESP32-S3 microcontroller, it offers high computational capabilities with the Xtensa® single-core 32-bit LX7 running up to 240 MHz. The ESP32-S3 supports WiFi and Bluetooth, providing extensive and flexible connectivity options.

MCU + Wifi + Bluetooth + MTK3333 + BNO080 + RV3028 + SIM800L + SD Module in one devices. 

<p float="left">
<img src="https://images.tokopedia.net/img/cache/900/VqbcmM/2024/3/5/956ddca7-6f69-4bfd-af2c-20b0f65653b0.jpg" width=400 /> 
<img src="https://images.tokopedia.net/img/cache/900/VqbcmM/2024/3/5/6e6649c4-8bc8-446e-a27e-64b5063150c8.jpg" width=400 /> 
<img src="https://images.tokopedia.net/img/cache/900/VqbcmM/2024/3/5/35240397-ed30-49b1-834f-22866227cd32.jpg" width=805 />
</p>

## How To Use
To use the Cerdas GPS Tracker, you need to program the board according to your requirements. We provide hardware test firmware that can be uploaded via the Arduino IDE to test all sensors and components. Before starting, ensure:

1. You have installed the necessary libraries.
2. Adjust the upload settings in your Arduino IDE.
3. The Cerdas GPS Tracker is in download mode.

#### Required Libraries:
- [RV-3028_C7-Arduino_Library](https://github.com/constiko/RV-3028_C7-Arduino_Library) by Constantin Koch
- [SparkFun_BNO080_Arduino_Library](https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library) by Sparkfun
- [Adafruit_GPS](https://github.com/adafruit/Adafruit_GPS) by Adafruit

#### Arduino IDE Settings:
- Board Name: **ESP32S3 Dev Module**
- USB CDC on Boot: **Enabled**
- USB DFU on Boot: **Disabled**
- Upload Mode: **UART0 / Hardware CDC**
- USB Mode: **Hardware CDC / JTAG**

#### Entering Download Mode:
1. Press and hold the EN button while the board is off.
2. Turn on the board.
3. Release the EN button.
4. The board is now in download mode.

## Pins Configuration

#### GPIO Configuration
- **GPIO13:** Enable power for GPS, RTC RV-3028, and BNO080  
  *Set GPIO13 to LOW to turn off GPS, RTC, and IMU.*

#### SD Module Pins
- **SCK:** GPIO7
- **MISO:** GPIO6
- **MOSI:** GPIO8
- **CS:** GPIO5

#### I2C for RTC RV-3028 and BNO080
- **SDA:** GPIO41
- **SCL:** GPIO40

#### Serial Pins for GPS
- **RX:** GPIO21
- **TX:** GPIO17

#### Serial Pins for SIM800L
- **RX:** GPIO2
- **TX:** GPIO1

## Notice
To activate SIM800L, you must physically press the GSM power button a few seconds until the GSM Power LED is on. Currently, it cannot be activated through programming.

## Development logs
V1.0
- Initial design

## PCB and Parts
If you want to make the hardware yourself, just download the gerber file in production folder. Send it to your fav pcb manufacturer. And dont forget to get the Bill of materials :
|Qty | Part Name | Parts | MPN |
| ------------- |:-------------|:-------------| -----:|

## License
*We invests time and resources providing this open-source hardware, please support us by purchasing our products.*

*Designed by **[Insan Sains](https://www.youtube.com/insansains)** for **[Gerai Cerdas](https://geraicerdas.com)**, with contributions from the open source community. Creative Commons Attribution/Share-Alike, all text above must be included in any redistribution. See license.txt for additional information.*
