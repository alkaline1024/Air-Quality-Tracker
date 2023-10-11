#include <DHT.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <LiquidCrystal_I2C.h>


extern void sendCommand(uint8_t);
extern void display7Segment(int position, int segmentData);

extern DHT dht;
extern LiquidCrystal_I2C lcd;


String format2digits(int number)
{
  if (number >= 0 && number < 10)
  {
    return "0" + String(number);
  }
  return String(number);
}

void readDisplayDHT11()
{
  float humidity = dht.readHumidity();
  _delay_ms(50);
  float temperature = dht.readTemperature();
  _delay_ms(50);

  lcd.setCursor(0, 0);
  lcd.print("H:" + String(humidity, 1) + "% T:" + String(temperature, 1) + char(0xDF) + "C  ");
}

tmElements_t tm; // for DS1307
void readDisplayDS1307()
{
  lcd.setCursor(0, 1);
  if (RTC.read(tm))
  {
    sendCommand(0x44);
   

    lcd.print(format2digits(tm.Day));
    lcd.print("/");
    lcd.print(format2digits(tm.Month));
    lcd.print("/");
    lcd.print(tmYearToCalendar(tm.Year));
    lcd.print(" ");
    lcd.print(format2digits(tm.Hour));
    lcd.print(":");
    lcd.print(format2digits(tm.Minute));
  }
  else
  {
    lcd.print("RTC ERROR!");
  }

  sendCommand(0x44);
}

