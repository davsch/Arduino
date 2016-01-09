#include <LiquidCrystal.h>
#include <Sunrise.h>
#include <DS3231.h>

// Define LCD keyboard buttons
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5


// Define UTC + Daylight Save Time offset (1h + 0/1h)
int intTimeZoneAndDstOffset = 1;

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Init the sunrise lib with longitude, latitude and timezone (UTC+1/UTC+2)
Sunrise sunrise(59.3293235, 18.0685808, intTimeZoneAndDstOffset);

// Init the LCD Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  

// Set flag for time blink indicator
bool blinkColon = true;

// Set flag for date/time set indicator
bool dateTimeSet = false;


void setup()
{
  // Do we need to reinitialize Sunrise class due to summer-time?

  
  // Define LCD size and clear screen 
  lcd.begin(16,2);
  
  lcd.clear();

  // Create some fancy icons for sunrise & sunset
  byte charUp[8] = {
    B00100,
    B01110,
    B11111,
    B00000,
    B00000,
    B10101,
    B01110,
    B11111,
  };

  byte charDown[8] = {
    B11111,
    B01110,
    B00100,
    B00000,
    B00000,
    B10101,
    B01110,
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
  lcd.print(dateStr);
  lcd.setCursor(11, 0);
  lcd.print(timeStr.substring(0, 2) + strColon + timeStr.substring(3, 5));
    
  String strSunrise = getSunrise(false, dateStr);

  lcd.setCursor(0, 1);  
  lcd.write(byte(0));
  lcd.print(" " + strSunrise);

  String strSunset = getSunrise(true, dateStr);
  lcd.setCursor(9, 1);  
  lcd.write(byte(1));
  lcd.print(" " + strSunset);
  
  delay(1000);
}

String getSunrise(bool sunset, String dateStr)
{
  byte hoursByte, minutesByte;
  int minutesInt;
  String hoursStr, minutesStr;

  // Extract month and day from date string
  int intMonth = dateStr.substring(3, 5).toInt();
  int intDay = dateStr.substring(6, 8).toInt();

  // What do we ask for?
  if(sunset)
  {
    minutesInt = sunrise.Set((char)intMonth, (char)intDay);
  }
  else
  {
    minutesInt = sunrise.Rise((char)intMonth, (char)intDay);
  }

  // Check amount of minutes after midnigt sunrise/sunset occurs and convert to h:m
  if(minutesInt > 0){
    hoursByte = sunrise.Hour();
    minutesByte = sunrise.Minute();
  }

  // Fix the output format
  if(hoursByte < 10){
    hoursStr = "0" + String(hoursByte, DEC);
  }
  else{
    hoursStr = String(hoursByte, DEC);
  }
  if(minutesByte < 10){
    minutesStr = "0" + String(minutesByte, DEC);
  }
  else{
    minutesStr = String(minutesByte, DEC);
  }

  // Return sunrise/sunset as hour:minute string
  return hoursStr + ":" + minutesStr;
}

int getTimeOffset()
{
  // Return Time zone & DST offset
  
  // Swedish DST 
  // Start: last sunday of march 
  // End: last sunday of october

  // TODO: Fix time offset check

  return 1;
}



