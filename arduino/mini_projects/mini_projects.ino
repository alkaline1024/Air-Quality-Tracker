#include "FUNC_LCD.h"
#include "FUNC_TM1638.h"
#include "FUNC_SLEEP.h"
#include "FUNC_SETTIMERTC.h"

#include <PMS.h>
#include <DHT.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define ESP8266_RX 2
#define ESP8266_TX 3
#define ODROID_RX 4
#define ODROID_TX 5
#define PMS_RX 10
#define PMS_TX 11

#define DHTPIN 6
#define DHTTYPE DHT11

#define G_LED 14
#define O_LED 15
#define R_LED 16

const int strobe = 7;
const int clock = 8;
const int data = 9;

SoftwareSerial esp8266_serial(ESP8266_RX, ESP8266_TX); // RX, TX
SoftwareSerial odroid_serial(ODROID_RX, ODROID_TX);    // RX, TX
SoftwareSerial pms_serial(PMS_RX, PMS_TX);             // RX, TX

PMS pms(pms_serial);
PMS::DATA pms_data;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long started_time;
unsigned long ended_time;

void setup()
{
  setTimeRTC();

  Serial.begin(115200);
  esp8266_serial.begin(31250);
  odroid_serial.begin(19200);
  pms_serial.begin(9600);

  // SERIAL PORT
  pinMode(ESP8266_RX, INPUT);
  pinMode(ESP8266_TX, OUTPUT);
  pinMode(ODROID_RX, INPUT);
  pinMode(ODROID_TX, OUTPUT);
  pinMode(PMS_RX, INPUT);
  pinMode(PMS_TX, OUTPUT);
  // LED FOR PM LEVEL
  pinMode(G_LED, OUTPUT);
  pinMode(O_LED, OUTPUT);
  pinMode(R_LED, OUTPUT);
  // TM1638
  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  // DHT
  pinMode(DHTPIN, INPUT);

  sendCommand(0x8f); // TM1638 set full brightness
  reset();           // TM1638 RESET

  lcd.init();      // LCD INIT
  lcd.backlight(); // LCD show black light

  dht.begin(); // Begin DHT

  readDisplayDHT11();  // Read DHT11 and display on LCD
  readDisplayDS1307(); // Read DS1307 and display on LCD
  started_time = millis();
}

int pm_mode = 25;
int pm_value = 0;
void loop()
{
  _delay_ms(200);
  pms_serial.begin(9600);
  if (pms.readUntil(pms_data))
  {
    display7Segment(0xc1, 1); // if pms7003 readable display 1st led
    Serial.println();
    Serial.println("PM 1.0 (ug/m3): " + String(pms_data.PM_AE_UG_1_0));
    Serial.println("PM 2.5 (ug/m3): " + String(pms_data.PM_AE_UG_2_5));
    Serial.println("PM 10.0 (ug/m3): " + String(pms_data.PM_AE_UG_10_0));

    _delay_ms(200);
    odroid_serial.begin(19200);
    odroid_serial.println(String(pms_data.PM_AE_UG_1_0) + "," + String(pms_data.PM_AE_UG_2_5) + "," + String(pms_data.PM_AE_UG_10_0));
    esp8266_serial.begin(31250);
    esp8266_serial.println(String(pms_data.PM_AE_UG_1_0) + "," + String(pms_data.PM_AE_UG_2_5) + "," + String(pms_data.PM_AE_UG_10_0));
  }
  display7Segment(0xc1, 0);

  uint8_t buttons = readTM1638_Buttons();
  switch (buttons)
  {
  case 1:
    pm_mode = 100;
    break;
  case 2:
    pm_mode = 25;
    break;
  case 4:
    pm_mode = 10;
    break;
  }
  switch (pm_mode)
  {
  case 100:
    pm_value = pms_data.PM_AE_UG_10_0;
    break;
  case 25:
    pm_value = pms_data.PM_AE_UG_2_5;
    break;
  case 10:
    pm_value = pms_data.PM_AE_UG_1_0;
    break;
  }

  displayPMSMode(pm_mode);
  displayPMSValue(pm_value);
  displayLEDLevel(pms_data.PM_AE_UG_2_5);

  ended_time = millis();
  if (ended_time - started_time > 10000)
  {
    started_time = millis();
    readDisplayDHT11();
    readDisplayDS1307();
  }
}

void displayLEDLevel(int pm_2_5_value)
{
  if (pm_2_5_value < 50)
  {
    digitalWrite(G_LED, HIGH);
    digitalWrite(O_LED, LOW);
    digitalWrite(R_LED, LOW);
  }
  else if (pm_2_5_value < 100)
  {
    digitalWrite(G_LED, LOW);
    digitalWrite(O_LED, HIGH);
    digitalWrite(R_LED, LOW);
  }
  else
  {

    digitalWrite(G_LED, LOW);
    digitalWrite(O_LED, LOW);
    digitalWrite(R_LED, HIGH);
  }
}
