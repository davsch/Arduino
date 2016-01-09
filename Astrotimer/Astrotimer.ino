#include <LiquidCrystal.h>
#include <Sunrise.h>
#include <DS3231.h>

#define DST 1

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Init the sunrise lib with longitude, latitude and timezone (UTC+1/UTC+2)
Sunrise sunrise(59.3293235, 18.0685808, DST);

// Init the LCD Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  

bool blinkColon = true;

void setup()
{
  // Define LCD size and clear screen 
  lcd.begin(16,2);
  
  lcd.clear();

  // Create custom characters for sunrise & sunset
  byte charUp[8] = {
    B00000,
    B00000,
    B00100,
    B01110,
    B11111,
    B00000,
    B11111,
  };

  byte charDown[8] = {
    B00000,
    B00000,
    B11111,
    B01110,
    B00100,
    B00000,
    B11111,
  };

  lcd.createChar(0, charUp);
  lcd.createChar(1, charDown);

  rtc.begin();
  // If necessary - set date & time
  //rtc.setDOW(FRIDAY);
  //rtc.setTime(20,24,0);
  //rtc.setDate(9, 1, 2016);

  

  sunrise.Actual(); //Actual, Civil, Nautical, Astronomical
  
  //lcd.print("Time:" + dateStr);
  //lcd.setCursor(1, 0);
  //lcd.print("Sunrise:" + sunriseTimeStr);
}

void loop()
{
  String dateStr = rtc.getDateStr(1, 2);
  String timeStr = rtc.getTimeStr();
  String strColon = ":";
  
  if(!blinkColon)
  {
    strColon = " ";      
  }
    
  blinkColon = !blinkColon;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(dateStr + " " + timeStr.substring(0, 2) + strColon + timeStr.substring(3, 5));
  lcd.setCursor(0, 1);  
  
  String strSunrise = getSunrise(dateStr);

  lcd.write("*");
  lcd.write(byte(0));
  lcd.print(strSunrise);

  String strSunset = getSunset(dateStr);
  
  lcd.write(" *");
  lcd.write(byte(1));
  lcd.print(strSunset);
      
  delay(1000);
}



String getSunrise(String dateStr)
{
  byte h, m;
  int t;

  int intYear = dateStr.substring(0, 2).toInt();
  int intMonth = dateStr.substring(3, 5).toInt();
  int intDay = dateStr.substring(6, 8).toInt();
  
  t = sunrise.Rise((char)intMonth, (char)intDay);

  if(t>0){
    h = sunrise.Hour();
    m = sunrise.Minute();
  }
    
  return String(h) + ":" + String(m);
}

String getSunset(String dateStr)
{
  byte h, m;
  int t;

  int intYear = dateStr.substring(0, 2).toInt();
  int intMonth = dateStr.substring(3, 5).toInt();
  int intDay = dateStr.substring(6, 8).toInt();
  
  t = sunrise.Set((char)intMonth, (char)intDay);

  if(t>0){
    h = sunrise.Hour();
    m = sunrise.Minute();
  }

  if(m < 10){
    return String(h) + ":0" + String(m);
  }
  else{
    return String(h) + ":" + String(m);
  }
}

