#include <LiquidCrystal.h>
#include <Sunrise.h>
#include <DS3232RTC.h>
#include <RCSwitch.h>
#include <Time.h>

/* INITIALIZE LIBRARIES */

// Initialize the sunrise lib with longitude, latitude and timezone (UTC+1/UTC+2)
Sunrise sunrise(59.3293235, 18.0685808, +2);

// Initialize RCSwitch 433Mhz TX/RX library
RCSwitch rcSwitch = RCSwitch();

// Initialize the LCD Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/* DECLARE GLOBAL VARIABLES */
bool blinkColon = true;

/* DECLARE CONSTANTS */


// Set lights-on & -off hour
const int INT_LIGHTS_OFF_HOUR = 23;
const int INT_LIGHTS_ON_HOUR = 6;

// Assign switch codes based on sniffed data from remote control
const int INT_SWITCH3_ON = 1397079;
const int INT_SWITCH3_OFF = 1397076;

// Set loop delay
const int INT_LOOP_DELAY = 500;

void setup()
{
  // Configure port for serial communication - logging
  Serial.begin(9600);

  // Configure RC Switch
  rcSwitch.enableTransmit(10);
  rcSwitch.setPulseLength(437);

  // If necessary - set date & time
  
//  RTC.setDOW(FRIDAY);
//  RTC.setTime(20,32,0);
//  RTC.setDate(28, 10, 2016);

  // Get time from RTC
  setSyncProvider(RTC.get);   
  
  if(timeStatus() != timeSet) 
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time"); 

  // Set type of time for sunrise library
  sunrise.Actual(); //Available: Actual, Civil, Nautical, Astronomical
  
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

  // Print some fancy chars
  lcd.createChar(0, charUp);
  lcd.createChar(1, charDown);
}

/*
   Round we go...
*/
void loop()
{
  // Variables for minutes past midnigt to sunrise & sunset
  int intSunriseMinutes= 0, intSunsetMinutes = 0;
  
  // Get sunrise data based on date
  intSunriseMinutes= sunrise.Rise((char)month(), (char)day());
  String strSunrise = getTimeFormattedAsString(intSunriseMinutes, sunrise.Hour(), sunrise.Minute());

  // Get sunset data based on date
  intSunsetMinutes = sunrise.Set((char)month(), (char)day());
  String strSunset = getTimeFormattedAsString(intSunsetMinutes, sunrise.Hour(), sunrise.Minute());

  if(hour() < INT_LIGHTS_ON_HOUR)
  {
    Serial.println("Before lights-on hour in the morning - switch off");
    rcSwitch.send(INT_SWITCH3_OFF, 24);
  }
  else if(!isPastSunrise(intSunriseMinutes))
  {
     // If it's not past sunrise - set switch to 'On'  
     Serial.println("Before sunrise - switch on");
     rcSwitch.send(INT_SWITCH3_ON, 24);
  }
  else if(isPastSunrise(intSunriseMinutes) && !isPastSunset(intSunsetMinutes))
  {
    // If it's past sunrise but not after sunset - set switch to 'Off'  
    Serial.println("After sunrise, before sunset - switch off");
    rcSwitch.send(INT_SWITCH3_OFF, 24);
  }
  else if(isPastSunset(intSunsetMinutes) && (hour() < INT_LIGHTS_OFF_HOUR))
  {
    // If it's past sunrise but not after sunset - set switch to 'On'
    Serial.println("After sunset - switch on");
    rcSwitch.send(INT_SWITCH3_ON, 24);
  }
  else
  {
    Serial.println("After 10 in the evenining - switch off");
    rcSwitch.send(INT_SWITCH3_OFF, 24);
  }
  
  updateLcdScreen(strSunrise, strSunset);

  // Refresh after x minutes...
  delay(INT_LOOP_DELAY);
}

/*
   Returns time formatted as string in 24-h hh:mm format
*/
String getTimeFormattedAsString(int intMinutes, int intHour, int intMinute)
{
  if(intMinutes > 0)
  {
    String strHour = "", strMinute = "";
    
    if(intHour < 10)
      strHour = "0" + (String)intHour;
    else
      strHour = (String)intHour;

    if(intMinute < 10)
      strMinute = "0" + (String)intMinute;
    else
      strMinute = (String)intMinute;
      
    return strHour + ":" + strMinute;
  }
}

void updateLcdScreen(String strSunrise, String strSunset)
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

  // Print out current date & time
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

