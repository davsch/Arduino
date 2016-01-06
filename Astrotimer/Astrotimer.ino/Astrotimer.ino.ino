#include <LiquidCrystal.h>
#include <Sunrise.h>
#include <DS3231.h>



// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

void setup()
{
  LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  
  
  lcd.begin(16,2);
  lcd.setCursor(0,0);

  lcd.clear();

  rtc.begin();
  lcd.setCursor(0,1);

  String date = rtc.getDateStr(1, 2);
  char *tmp;

  tmp = strtok(&date[0], ".");
  lcd.print("M:");
  lcd.print(tmp[0]);
  lcd.print("D:");
  lcd.print(tmp[1]);
}

void loop()
{
  
}



