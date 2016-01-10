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

// Define delay for button press and inactive timeout
#define BUTTON_DELAY 500
#define BUTTON_TIMEOUT 5000


int lcd_key = 0;
int adc_key_in = 0;

// Define UTC + Daylight Save Time offset (1h + 0/1h)
int intTimeZoneAndDstOffset = 1;

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

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
bool setUpType = 0; // 0 = date, 1 = time
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
  lcd_key = read_LCD_buttons();
  
  switch (lcd_key)
  {
    case btnUP:
    {
      if(setUpMode)
      {
        if(setUpType == 0)
        {
          // Adjust date up
        }
        else if(setUpType == 1)
        {
          // Adjust time up
        }
      }
    }
    case btnDOWN:
    {
      if(setUpMode)
      {
        if(setUpType == 0)
        {
          // Adjust date down
        }
        else if(setUpType == 1)
        {
          // Adjust time down
        }
      }
    }
    case btnLEFT:
    {
      if(setUpMode)
      {
        if(setUpType == 0)
        {
          
        }
        else if(setUpType == 1)
        {
          
        }
      }
    }
    case btnRIGHT:
    {
      if(setUpMode)
      {
        if(setUpType == 0)
        {
          
        }
        else if(setUpType == 1)
        {
          
        }
      }
    }
    case btnSELECT:
    {
      setUpMode = !setUpMode;
    }
  }
  
  delay(BUTTON_DELAY);
  
  String dateStr = rtc.getDateStr(1, 2);
  String timeStr = rtc.getTimeStr();
  String colonStr = ":";
  
  if(!setUpMode && !displayToggle)
  {
    colonStr = " ";      
  }
    
  displayToggle = !displayToggle;
  
  lcd.clear();
  lcd.setCursor(0, 0);

  if(!(setUpMode && setUpType == 0 && displayToggle))
  {
    lcd.print(dateStr);
  }
  
  lcd.setCursor(11, 0);
  if(!(setUpMode && setUpType == 1 && displayToggle))
  {
    lcd.print(timeStr.substring(0, 2) + colonStr + timeStr.substring(3, 5));
  }
    
  String strSunrise = getSunrise(false, dateStr);

  lcd.setCursor(0, 1);  
  lcd.write(byte(0));
  lcd.print(" " + strSunrise);

  String strSunset = getSunrise(true, dateStr);
  lcd.setCursor(9, 1);  
  lcd.write(byte(1));
  lcd.print(" " + strSunset);

  delay(500);
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



