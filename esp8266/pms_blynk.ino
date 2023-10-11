#define BLYNK_TEMPLATE_ID "TMPL6xaRjGL92"
#define BLYNK_TEMPLATE_NAME "PMS7003Template"
#define BLYNK_AUTH_TOKEN "ct3blJmW7AhiID9Ig3wc0aM81wZFECOp"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

#define RX_PIN 5
#define TX_PIN 4

SoftwareSerial pms_serial(RX_PIN, TX_PIN);

char ssid[] = "R202 WiFi";
char pass[] = "!q2w3e4r5t";
BlynkTimer timer;

uint8_t sw_code = 0;
uint16_t analog_20_per = 0;
uint8_t g_led_for_blink = 0;
uint8_t r_led_for_blink = 1;
int last_time = 0;
bool delay_mode = false;
bool blynk_control_all_led = false;


BLYNK_CONNECTED()
{
  Blynk.setProperty(V100 ,"offImageUrl", "https://staticimage.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V100 ,"onImageUrl", "https://staticimage.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V100, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstartdevice-was-made");
}

void ReadPMS()
{
  if (pms_serial.available()) {
    String data = pms_serial.readStringUntil('\n'); 
    int pm1_0, pm2_5, pm10_0;
    sscanf(data.c_str(), "%d,%d,%d", &pm1_0, &pm2_5, &pm10_0);

    Serial.print("Received pm1_0: ");
    Serial.println(pm1_0);
    Serial.print("Received pm2_5: ");
    Serial.println(pm2_5);
    Serial.print("Received pm10_0: ");
    Serial.println(pm10_0);
    Serial.println();

    Blynk.virtualWrite(V0, pm1_0);
    Blynk.virtualWrite(V1, pm2_5);
    Blynk.virtualWrite(V2, pm10_0);
    if (pm2_5 < 50) {
      Blynk.virtualWrite(V3, HIGH);
      Blynk.virtualWrite(V4, LOW);
      Blynk.virtualWrite(V5, LOW);
    }
    else if (pm2_5 < 100) {
      Blynk.virtualWrite(V3, LOW);
      Blynk.virtualWrite(V4, HIGH);
      Blynk.virtualWrite(V5, LOW);
    }
    else {
      Blynk.virtualWrite(V3, LOW);
      Blynk.virtualWrite(V4, LOW);
      Blynk.virtualWrite(V5, HIGH);
    }
  }
}

void setup()
{
  Serial.begin(38400);
  pms_serial.begin(31250);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(100L, ReadPMS);
}

void loop()
{
  Blynk.run();
  timer.run();
  
}