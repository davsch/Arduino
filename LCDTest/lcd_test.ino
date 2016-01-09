
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int lcd_key    = 0;
int adc_key_in = 0;

int menu_id = 0;
int sub_menu_id = 0;

bool astro_always_on = false;

String menu_array[10][10];

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

#define BUTTON_DELAY 500
#define BUTTON_TIMEOUT 5000

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

void setup() {

  lcd.begin(16,2);
  lcd.setCursor(0,0);

  menu_array[0][0] = "Astro clock";
  menu_array[0][1] = "Always On";
  menu_array[0][2] = "Morning On";
  menu_array[0][3] = "Night Off";
  menu_array[1][0] = "Beer Fridge Timer";
  menu_array[1][1] = "Always On";
  menu_array[1][2] = "Date & Time On";
  menu_array[1][3] = "Date & Time Off";
  menu_array[2][0] = "Fan Timer";
  menu_array[2][1] = "Run Time";
  menu_array[2][2] = "Delay Interval";
  menu_array[3][0] = "Set Date & Time";
}

void loop() {

    lcd.setCursor(0,1);
    lcd.setCursor(0, 1);
    lcd_key = read_LCD_buttons();

    switch (lcd_key)
    {
      case btnRIGHT:
      {
        if(menu_array[menu_id][sub_menu_id+1] != NULL)
        {
          sub_menu_id = sub_menu_id + 1;
          lcd.clear();
          lcd.print(menu_array[menu_id][sub_menu_id]);
          if(menu_id == 0 && sub_menu_id == 1)
          {
            lcd.setCursor(0,1);
            if(astro_always_on)
            {
              lcd.print("Yes");            
            }
            else
            {
              lcd.print("No");
            }
          }
          delay(BUTTON_DELAY);
        }
        break;
      }
      case btnLEFT:
      {
        if(sub_menu_id != 0)
        {
          sub_menu_id = sub_menu_id - 1;
          lcd.clear();
          lcd.print(menu_array[menu_id][sub_menu_id]);          
          delay(BUTTON_DELAY);
        }
        break;
      }
      case btnUP:
      {
        lcd.setCursor(0,1);
        if(menu_id != 0)
        {
          menu_id = menu_id - 1;
          sub_menu_id = 0;
          lcd.clear();
          lcd.print(menu_array[menu_id][sub_menu_id]);
          delay(BUTTON_DELAY);
        }
        break;
      }
      case btnDOWN:
      {
        lcd.setCursor(0,1);
        if(menu_id < 3)
        {
          menu_id = menu_id + 1;
          sub_menu_id = 0;
          lcd.clear();
          lcd.print(menu_array[menu_id][sub_menu_id]);
          delay(BUTTON_DELAY);
        }
        
        break;
      }
      case btnSELECT:
      {
        if(menu_id == 0 && sub_menu_id == 1)
        {
            astro_always_on = !astro_always_on;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(menu_array[menu_id][sub_menu_id]);
            lcd.setCursor(0,1);
            if(astro_always_on)
            {
              lcd.print("Yes");            
            }
            else
            {
              lcd.print("No");
            }
        }

        delay(BUTTON_DELAY);
        break;
      }
      case btnNONE:
      {
        lcd.setCursor(0,1);
        //lcd.print("                ");
        break;
      }
    }
  }



  /*
   *  1.      Tid
   *  1.1       Ställ in tid
   *  1.2       Ställ in datum
   *  2.      Astrour
   *  2.1       Ljus på
   *  2.2       Starttid morgon
   *  2.3       Sluttid kväll
   *  3.      Öltimer
   *  3.1       Starttid
   *  3.2       Sluttid
   * 
   * 
   */
   
