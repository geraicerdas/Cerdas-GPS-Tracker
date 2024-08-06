/* CERDAS GPS TRACKER HARDWARE TEST

Before you begin, please make sure :
1. You have installed required libraries
2. Upload setting on your Arduino IDE
3. Your Cerdas GPS Tracker is on Download mode

https://github.com/constiko/RV-3028_C7-Arduino_Library
By: Constantin Koch

https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library
By: Sparkfun

https://github.com/adafruit/Adafruit_GPS
By: Adafruit

Board : ESP32S3 Dev Mod
USB CDC on Boot : Enabled
USB DFU on Boot : Disabled
Upload Mode : UART0 / Hardware CDC
USB Mode : Hardware CDC / JTAG

To enter download mode, it maybe not your default
Press and hold EN button while the board is off
Turn on the board, and release the EN button
Now you're in the download mode

*/

#include <RV-3028-C7.h>
#include "SparkFun_BNO080_Arduino_Library.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <HardwareSerial.h>
#include <Adafruit_GPS.h>

// this is pin for enabling others hardware for low power purposes
// you can set EN_Module to LOW if you don't use others hardware
// Hardware that will be affected :
// 1. GPS
// 2. RTC RV-3028
// 3. IMU BNO080
#define EN_Module 13

// SPI pins for SD Card
#define SCK  7
#define MISO  6
#define MOSI  8
#define CS  5

// Serial pins for GPS
#define GPS_RX 21
#define GPS_TX 17
#define GPS_BAUDRATE 9600
HardwareSerial GPSSerial(1);
Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO false
uint32_t timer = millis();

// Serial pins for SIM800L
#define GSM_RX 2
#define GSM_TX 1
#define GSM_BAUDRATE 9600
HardwareSerial sim800(2);

// I2C pins for RTC and IMU
#define SDA 41
#define SCL 40
SPIClass spi = SPIClass(HSPI);
RV3028 rtc;
BNO080 myIMU;

// Struktur untuk menyimpan pasangan kode operator dan nama provider
struct Operator {
  int code;
  const char* name;
};

// Daftar operator (Indonesia) yang diketahui
Operator operators[] = {
  {1, "Indosat"},
  {3, "XL"},
  {5, "Telkomsel"},
  {6, "Axis"},
  {8, "Three"}
};
const int operatorCount = sizeof(operators) / sizeof(operators[0]);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(3000);
  Serial.println("Cerdas GPS Tracker");
  Serial.println("Hardware Test");

  // Activate the module
  pinMode(EN_Module, OUTPUT);
  digitalWrite(EN_Module, HIGH);

  Wire.begin(SDA,SCL);
  //---------------------------------------------------------------------------------
  //                                RTC RV-3028-C7
  //---------------------------------------------------------------------------------
  Serial.print("RTC.................................... ");
  delay(1000);
  if (rtc.begin() == false) Serial.println("[Failed]");
  else {
    Serial.println("[OK]");
    delay(1000);
    Serial.print("Setting date to... ");
    delay(1000);
    if (rtc.setToCompilerTime() == false) Serial.println("[Failed]");
    else {
      String currentTime = rtc.stringTimeStamp();
      Serial.print(currentTime);
      Serial.println(" [OK]");
    }
  }

  //---------------------------------------------------------------------------------
  //                                 IMU - BNO080
  //---------------------------------------------------------------------------------
  delay(1000);
  Serial.print("BNO080. Try using default address 0x"); Serial.print(BNO080_DEFAULT_ADDRESS);
  delay(1000);
  if (myIMU.begin(BNO080_DEFAULT_ADDRESS, Wire)) {
    Serial.println("  [OK]");
    myIMU.enableRotationVector(50); //Send data update every 50ms
  } else Serial.println("  [Failed]");

  delay(1000);
  Serial.println("Try getting initial IMU data...........");
  delay(1000);
  unsigned long startMillis = millis();
  unsigned long maxWaitResponse = 6000L;
  while(true) {
    if ((millis()-startMillis) > maxWaitResponse) {
      Serial.println("                [Failed]");
      delay(1000);
      break;
    }
    if (myIMU.dataAvailable() == true) {
      float quatI = myIMU.getQuatI();
      float quatJ = myIMU.getQuatJ();
      float quatK = myIMU.getQuatK();
      float quatReal = myIMU.getQuatReal();
      float quatRadianAccuracy = myIMU.getQuatRadianAccuracy();

      Serial.print(quatI, 2); Serial.print(F(","));
      Serial.print(quatJ, 2); Serial.print(F(","));
      Serial.print(quatK, 2); Serial.print(F(","));
      Serial.print(quatReal, 2); Serial.print(F(","));
      Serial.print(quatRadianAccuracy, 2);
      Serial.println("              [OK]");
      delay(1000);
      break;
    } 
  }

  //---------------------------------------------------------------------------------
  //                                  MEMORY CARD
  //---------------------------------------------------------------------------------
  Serial.print("Try mounting Memory Card............... ");
  delay(1000);
  // added here, because we use custom SPI
  spi.begin(SCK, MISO, MOSI, CS);
  if (!SD.begin(CS,spi,80000000)) {
    Serial.println("[Failed]");
    delay(1000);
  } else {
    Serial.println("[OK]");
    delay(1000);

    Serial.print("Memory Card Type : ");
    delay(1000);

    uint8_t cardType = SD.cardType();
    bool memoryAttached = true;
    if(cardType == CARD_NONE){
      Serial.println("[No memory card attached]");
      memoryAttached = false;
    } else if(cardType == CARD_MMC) Serial.println("MMC                  [OK]");
    else if(cardType == CARD_SD) Serial.println("SDSC                 [OK]");
    else if(cardType == CARD_SDHC) Serial.println("SDHC                 [OK]");
    else Serial.println("UNKNOWN              [OK]");

    delay(1000);
    Serial.print("Memory Card Size : ");
    delay(1000);
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    if (memoryAttached) Serial.printf(" %11uMB       [OK]\n", cardSize);
    else Serial.println("  - ");
  }
  delay(1000);

  //---------------------------------------------------------------------------------
  //                                      GPS
  //---------------------------------------------------------------------------------
  GPSSerial.begin(GPS_BAUDRATE, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.print("GPS.................................... ");
  delay(1000);
  if (GPS.begin(9600)) {
    Serial.println("[OK]");
    delay(1000);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
    GPS.sendCommand(PGCMD_ANTENNA);
    delay(1000);
    GPSSerial.println(PMTK_Q_RELEASE);
  } else {
    Serial.println("[Failed]");
    delay(1000);
  }
  

  //---------------------------------------------------------------------------------
  //                                     SIM800L
  //---------------------------------------------------------------------------------
  sim800.begin(GSM_BAUDRATE, SERIAL_8N1, GSM_RX, GSM_TX);

  // Memeriksa koneksi AT Command
  checkATCommand("AT Command SIM800L.....................", "AT", "OK");

  // Memeriksa kekuatan sinyal
  checkSignalQuality();

  // Memeriksa IMEI
  checkIMEI();

  // Memeriksa status kartu SIM
  checkATCommand("SIM Card Status........................", "AT+CPIN?", "+CPIN: READY");

  // Memeriksa registrasi jaringan
  checkNetworkRegistration();


}

void loop() {
  // put your main code here, to run repeatedly:

}

//---------------------------------------------------------------------------------
//                          GPS Funtion to get GPS Data
//---------------------------------------------------------------------------------
void readGPS() {
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
    }
  }  
}

//---------------------------------------------------------------------------------
//                          SIM800L FUNCTIONS
//---------------------------------------------------------------------------------
void checkATCommand(String message, String command, String expectedResponse) {
  Serial.print(message);
  String response = sendATCommand(command);
  delay(1000);
  if (response.indexOf(expectedResponse) != -1) {
    Serial.println(" [OK]");
  } else {
    Serial.println(" [Failed]");
  }
}

void checkSignalQuality() {
  Serial.print("Check Signal Quality................... ");
  String response = sendATCommand("AT+CSQ");
  delay(1000);
  if (response.indexOf("OK") != -1) {
    int startIndex = response.indexOf("+CSQ: ") + 6;
    int endIndex = response.indexOf(",", startIndex);
    String signalQuality = response.substring(startIndex, endIndex);
    Serial.print("[OK] (");
    Serial.print(signalQuality);
    Serial.println(")");
  } else {
    Serial.println("[Failed]");
  }
}

void checkIMEI() {
  Serial.print("IMEI................................... ");
  String response = sendATCommand("AT+CGSN");
  delay(1000);
  if (response.indexOf("OK") != -1) {
    String imei = extractInfo(response, "AT+CGSN", "OK");
    Serial.print("[OK] (");
    Serial.print(imei);
    Serial.println(")");
  } else {
    Serial.println("[Failed]");
  }
}

void checkNetworkRegistration() {
  Serial.print("Network Registration................... ");
  String response = sendATCommand("AT+COPS?");
  delay(1000);
  if (response.indexOf("OK") != -1) {
    int startIndex = response.indexOf("+COPS: ") + 7;
    int endIndex = response.indexOf(",", startIndex);
    String operatorCodeStr = response.substring(startIndex, endIndex);
    operatorCodeStr.trim();
    int operatorCode = operatorCodeStr.toInt();
    const char* provider = "Unknown";

    for (int i = 0; i < operatorCount; i++) {
      if (operators[i].code == operatorCode) {
        provider = operators[i].name;
        break;
      }
    }

    Serial.print("[OK] (");
    Serial.print(provider);
    Serial.println(")");
  } else {
    Serial.println("[Failed]");
  }
}

String sendATCommand(String command) {
  sim800.println(command);
  
  String response = "";
  unsigned long startTime = millis();
  
  while (millis() - startTime < 2000) { // Membaca selama maksimal 2 detik
    while (sim800.available()) {
      char c = sim800.read();
      response += c;
    }
  }
  
  return response;
}

String extractInfo(String response, String startDelimiter, String endDelimiter) {
  int startIndex = response.indexOf(startDelimiter) + startDelimiter.length();
  int endIndex = response.indexOf(endDelimiter, startIndex);
  if (startIndex != -1 && endIndex != -1) {
    String info = response.substring(startIndex, endIndex);
    info.trim(); // Trim whitespace here
    return info;
  }
  return "";
}