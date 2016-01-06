#include <LiquidCrystal.h>
#include <Sunrise.h>
#include <DS3231.h>

void setup()
{
  LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  
  
  lcd.begin(16,2);
  lcd.setCursor(0,0);

  lcd.clear();
  lcd.print("Hello World");
}

void loop()
{
  
}



