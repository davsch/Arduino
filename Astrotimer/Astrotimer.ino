#include <LiquidCrystal.h>
#include <Sunrise.h>
#include <DS3231.h>



// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
Sunrise sunrise(55.32, 18.06, 0);

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

    sunrise.Astronomical(); //Actual, Civil, Nautical, Astronomical

  byte h,m;
  int t;
  // t= minutes past midnight of sunrise (6 am would be 360)
  t=sunrise.Rise(5,5); // (month,day) - january=1
  if(t>=0){
    h=sunrise.Hour();
    m=sunrise.Minute();
  }
}

void loop()
{

    
}

string getSunrise()
{
  String date = rtc.getDateStr(1, 2);
  char *tmp;
  tmp = strtok(&date[0], ".");

  byte h, m;
  int t;
  t = sunrise.Rise(tmp[0], tmp[1]);

  if(t>0){
    h = sunrise.Hour();
    m = sunrise.Minute();
  }

  return 
}

string getSunset()
{
  
}

