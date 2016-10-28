#include <LiquidCrystal.h>
#include <Sunrise.h>
#include <DS3232RTC.h>
#include <RCSwitch.h>
#include <Time.h>

// Initialize the DS3231 using the hardware interface
//DS3231  rtc(SDA, SCL);

// TODO - fix DST!
// Initialize the sunrise lib with longitude, latitude and timezone (UTC+1/UTC+2)
Sunrise sunrise(59.3293235, 18.0685808, +2);
RCSwitch rcSwitch = RCSwitch();

// Initialize the LCD Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

bool blinkColon = true;
int intYear = 0, intMonth = 0, intDay = 0, t_sunrise = 0, t_sunset = 0;

// Assign switch codes based on sniffed data from remote control
const int SWITCH3_ON = 1397079;
const int SWITCH3_OFF = 1397076;
  
byte sunrise_h, sunrise_m, sunset_h, sunset_m;

void setup()
{
  Serial.begin(9600);
  
  rcSwitch.enableTransmit(10);
  rcSwitch.setPulseLength(437);
  
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus() != timeSet) 
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time"); 
  
  
  // Define LCD size and clear screen
  lcd.begin(16, 2);

  lcd.clear();

  // Create fancy custom characters for sunrise & sunset
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

  // Initialize real-time clock
  //rtc.begin();

  // If necessary - set date & time
  /*
     rtc.setDOW(WEDNESDAY);
     rtc.setTime(22,51,0);
     rtc.setDate(26, 10, 2016);
  */

  // Set type of time for sunrise library
  sunrise.Actual(); //Actual, Civil, Nautical, Astronomical
}

/*
   Round we go...
*/
void loop()
{
  /*
  String dateStr = rtc.getDateStr(1, 2);
  String timeStr = rtc.getTimeStr();
  
*/
  
  intYear = year();//dateStr.substring(0, 2).toInt();
  intMonth = month();//dateStr.substring(3, 5).toInt();
  intDay = day();//dateStr.substring(6, 8).toInt();

  
  // Get sunrise data based on date
  t_sunrise = sunrise.Rise((char)intMonth, (char)intDay);
  String strSunrise = getAsFormattedString(t_sunrise, sunrise.Hour(), sunrise.Minute());

  // Get sunset data based on date
  t_sunset = sunrise.Set((char)intMonth, (char)intDay);
  String strSunset = getAsFormattedString(t_sunset, sunrise.Hour(), sunrise.Minute());

  if(hour() < 5)
  {
    Serial.println("Before 5 in the morning - switch off");
    rcSwitch.send(1397076, 24);
  }
  else if(!isPastSunrise(t_sunrise))
  {
     // If it's not past sunrise - set switch to 'On'  
     Serial.println("Before sunrise - switch on");
     rcSwitch.send(1397079, 24);
  }
  else if(isPastSunrise(t_sunrise) && !isPastSunset(t_sunset))
  {
    // If it's past sunrise but not after sunset - set switch to 'Off'  
    Serial.println("After sunrise, before sunset - switch off");
    rcSwitch.send(1397076, 24);
  }
  else if(isPastSunset(t_sunset) && (hour() < 22))
  {
    // If it's past sunrise but not after sunset - set switch to 'On'
    Serial.println("After sunset - switch on");
    rcSwitch.send(1397079, 24);
  }
  else
  {
    Serial.println("After 10 in the evenining - switch off");
    rcSwitch.send(1397076, 24);
  }
  
  updateLcdScreen(strSunrise, strSunset);

  // Refresh after 1 minute...
  delay(60000);
}

/*
   Returns sunrise as string in 24-h hh:mm format
*/
String getAsFormattedString(int t, byte h, byte m)
{
  if (t > 0) {
    h = sunrise.Hour();
    m = sunrise.Minute();
  }

  return String(h) + ":" + String(m);
}

void updateLcdScreen(String strSunrise, Srting strSunset)
{
  String strColon = ":";
  // Add some action to your display...
  if (!blinkColon)
  {
    strColon = " ";
  }

  blinkColon = !blinkColon;

  // Clear old data from display
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print(String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + strColon + String(minute()));
  lcd.setCursor(0, 1);

  // Print out sunrise time to the display
  lcd.write("*");
  lcd.write(byte(0));
  lcd.print(strSunrise);

  // Print out sunset time to the display
  lcd.write(" *");
  lcd.write(byte(1));
  lcd.print(strSunset);
}

bool isPastSunrise(int t)
{
  int intHour = hour();
  int intMin = minute();

  int intMinutes = (intHour * 60) + intMin;

  if(t < intMinutes)
  { 
    return true;
  }  
  return false;    
}

bool isPastSunset(int t)
{
  int intHour = hour();
  int intMin = minute();
  
  int intMinutes = (intHour * 60) + intMin;
  
  if(t < intMinutes)
  {
    return true;
  }
  return false;

}

