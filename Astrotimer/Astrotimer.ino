#include <LiquidCrystal.h>
#include <Sunrise.h>
#include <DS3232RTC.h>
#include <Wire.h>

// Define LCD keyboard buttons
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

// Define delay for button press and inactive timeout
#define BUTTON_DELAY 500
#define BUTTON_TIMEOUT 5000


int lcd_key = 0;
int adc_key_in = 0;

// Define UTC + Daylight Save Time offset (1h + 0/1h)
int intTimeZoneAndDstOffset = 1;

// Init the sunrise lib with longitude, latitude and timezone (UTC+1/UTC+2)
Sunrise sunrise(59.3293235, 18.0685808, intTimeZoneAndDstOffset);

// Init the LCD Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  

// Define display toggle
bool displayToggle = true;

// Define date/time set indicator
bool dateTimeSet = false;

// Define set-up mode
bool setUpMode = false;
int setUpType = 0; // 0 = year, 1 = day, 2 = month, 3 = hour, 4 = minute
void setup()
{
  // Do we need to reinitialize Sunrise class due to summer-time?

  // Get time from RTC
  setSyncProvider(RTC.get);
  
  if(timeStatus() != timeSet) 
  {    
    Serial.println("Unable to sync with the RTC");
  }
  else
  {
      Serial.println("RTC has set the system time");      
  }

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

  // If necessary - set date & time
  //rtc.setDOW(FRIDAY);
  //rtc.setTime(20,24,0);
  //rtc.setDate(9, 1, 2016);

  sunrise.Actual(); //Actual, Civil, Nautical, Astronomical

}

void loop()
{
  lcd_key = read_LCD_buttons();
  
  switch (lcd_key)
  {
    case btnUP:
    {
      if(setUpMode)
      {
        switch (setUpType)
        {
          case 0:
          {
            // Years
          }
          case 1:
          {
            // Months
          }
          case 2:
          {
            // Days
          }
          case 3:
          {
            // Hours
            adjustTime(3600);
          }
          case 4:
          {
            // Minutes
            adjustTime(60);
          }
        }
      }
    }
    case btnDOWN:
    {
      if(setUpMode)
      {
        switch (setUpType)
        {
          case 0:
          {
            // Years
          }
          case 1:
          {
            // Months
          }
          case 2:
          {
            // Days
          }
          case 3:
          {
            // Hours
            adjustTime(-3600);
          }
          case 4:
          {
            // Minutes
            adjustTime(-60);
          }
        }
      }
    }
    case btnLEFT:
    {
      if(setUpMode)
      {
        if(setUpType == 0)
        {
          setUpType = 4;
        }
        else
        {
          setUpType = setUpType - 1;
        }
      }
    }
    case btnRIGHT:
    {
      if(setUpMode)
      {
        if(setUpType == 4)
        {
          setUpType = 0;
        }
        else
        {
          setUpType = setUpType + 1;
        }
      }
    }
    case btnSELECT:
    {
      setUpMode = !setUpMode;
    }
  }
  
  delay(BUTTON_DELAY);
  
  String colonStr = ":";
  
  if(!setUpMode && !displayToggle)
  {
    colonStr = " ";      
  }
    
  displayToggle = !displayToggle;
  
  lcd.clear();
  lcd.setCursor(0, 0);

  if(setUpMode && setUpType < 3 && displayToggle)
  {    
    String strYear = String(year());
    String strMonth = getDoubleDigit(month());
    String strDay = getDoubleDigit(day());

    if(setUpType == 0)
      strYear = "    ";
    else if(setUpType == 1)
      strMonth = "  ";
    else if(setUpType == 2)
      strDay = "  ";

    lcd.print(strYear + ":" + strMonth + ":" + strDay);
  }
  else 
    lcd.print(String(year()) + ":" + getDoubleDigit(month()) + ":" + getDoubleDigit(day()));
 
  
  lcd.setCursor(11, 0);
  if(setUpMode && setUpType > 2 && displayToggle)
  {
    if(setUpMode != 3)
      lcd.print(getDoubleDigit(hour()) + ":  ");
    else
      lcd.print(String("  :" + getDoubleDigit(minute())));
    
  }
  else
    lcd.print(getDoubleDigit(hour()) + colonStr + getDoubleDigit(minute()));
  
  String strSunrise = getSunrise(false);

  lcd.setCursor(0, 1);  
  lcd.write(byte(0));
  lcd.print(" " + strSunrise);

  String strSunset = getSunrise(true);
  lcd.setCursor(9, 1);  
  lcd.write(byte(1));
  lcd.print(" " + strSunset);

  delay(500);
}

String getDoubleDigit(int digit)
{
  if(digit < 10)
    return "0" + String(digit);
  else
    return String(digit);
}

int read_LCD_buttons()
{
  adc_key_in = analogRead(0);

  if(adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;  
  if (adc_key_in < 250)  return btnUP; 
  if (adc_key_in < 400)  return btnDOWN; 
  if (adc_key_in < 635)  return btnLEFT; 
  if (adc_key_in < 890)  return btnSELECT;   

  return btnNONE;

}

String getSunrise(bool sunset)
{
  byte hoursByte, minutesByte;
  int intMinutesAfterMidnight;
  String hoursStr, minutesStr;

  
  // What do we ask for?
  if(sunset)
  {
    intMinutesAfterMidnight = sunrise.Set((char)month(), (char)day());
  }
  else
  {
    intMinutesAfterMidnight = sunrise.Rise((char)month(), (char)day());
  }

  // Check amount of minutes after midnigt sunrise/sunset occurs and convert to h:m
  if(intMinutesAfterMidnight > 0){
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



