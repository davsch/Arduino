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

#define DISPLAY_INACTIVE_MODE 60000
#define DISPLAY_REFRESH_OPERATING 1000
#define DISPLAY_REFRESH_SETUP 500
#define SUNRISE_READ_INTERVAL 60000

unsigned long displayLastRefresh;
unsigned long sunriseLastRead;
unsigned long buttonLastRead;

int lcd_key = 0;
int adc_key_in = 0;
int count = 0;
// Define UTC + Daylight Save Time offset (1h + 0/1h)
int intTimeZoneAndDstOffset = 1;

// Init the sunrise lib with longitude, latitude and timezone (UTC+1/UTC+2)
Sunrise sunrise(59.3293235, 18.0685808, intTimeZoneAndDstOffset);

// Init the LCD Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  

// Define display toggle
bool displayToggle = true;
bool displayOff = false;

String colonStr = ":";

// Define date/time set indicator
bool dateTimeSet = false;

// Define set-up mode
bool setUpMode = false;
int setUpType = 0; // 0 = year, 1 = day, 2 = month, 3 = hour, 4 = minute
void setup()
{
  Serial.begin(9600);
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

  
  buttonLastRead = 0;
  sunriseLastRead = 59900;
  displayLastRefresh = 0;

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
  Serial.println("Init done");
}

void loop()
{
  //Serial.println(String(millis() - btnPressInterval));
  if((millis() - buttonLastRead) >= BUTTON_DELAY)
  {
    
    lcd_key = read_LCD_buttons();
    
    if(lcd_key == btnSELECT)
    {
      Serial.println("Select pressed");
    }
    else if(lcd_key == btnLEFT)
    {
      Serial.println("Left pressed");
    }
    else if(lcd_key == btnRIGHT)
    {
      Serial.println("Right pressed");
    }
    else if(lcd_key == btnUP)
    {
      Serial.println("Up pressed");
    }
    else if(lcd_key == btnDOWN)
    {
      Serial.println("Down pressed");
    }
    buttonLastRead = millis();
    
  }


    
  

  if((millis() - displayLastRefresh) >= DISPLAY_REFRESH_OPERATING)
  {
    displayToggle = !displayToggle;

    if(!setUpMode && !displayToggle)
    {
      colonStr = " ";      
    }
    else
    {
      colonStr = ":";
    }
    
    Serial.println("Refreshing LCD");
    lcd.setCursor(0, 0);
    lcd.print("                ");
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
    
    if((millis() - sunriseLastRead) >= SUNRISE_READ_INTERVAL)
    {
    
      String strSunrise = getSunrise(false);
      
      lcd.setCursor(0, 1);  
      lcd.write(byte(0));
      lcd.print(" " + strSunrise);
    
      String strSunset = getSunrise(true);
      lcd.setCursor(9, 1);  
      lcd.write(byte(1));
      lcd.print(" " + strSunset);
    
      sunriseLastRead = millis();
    }

    displayLastRefresh = millis();
  }
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
  count = count + 1;
  Serial.println(String(count));
  Serial.println(String(adc_key_in));

  if(adc_key_in > 1000) 
    return btnNONE;
  else if (adc_key_in >= 0 && adc_key_in <= 10)   
    return btnRIGHT;  
  else if (adc_key_in >= 90 && adc_key_in <= 110)  
    return btnUP; 
  else if (adc_key_in >= 250 && adc_key_in <= 260)  
    return btnDOWN; 
  else if (adc_key_in >= 400 && adc_key_in <= 420)  
    return btnLEFT; 
  else if (adc_key_in >=  630 && adc_key_in <= 650)  
    return btnSELECT;   
  else
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



