// include the library code:
#include <LiquidCrystal.h>
#include <NTPClient.h>
#include <TimeLib.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
#define rs (D0)
#define en (D8)
#define d4 (D2)
#define d5 (D5)
#define d6 (D6)
#define d7 (D7)
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#define CONTROL (false)
#define DATA (true)

elapsedMillis LCD_Update;

void LCD_setup() {
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    LCD_Update = 0;
    lcd.setCursor(0,0);
    lcd.print(" Internet Clock ");
    lcd.setCursor(0,1);
    lcd.print("WPS to connect");
    while (LCD_Update < 2000);
    lcd.clear();
}

void LCD_loop() {
    if (LCD_Update < 1000) return;      //update once per second. Actually likely to be just over 1 second
                                        //should probably check more often and only print changes
    LCD_Update = 0;
    time_t tnow;
    char dstr[] = "--/-- ";         //space on the end in case day changes "31"->"1"
    char tstr[] = "--:--:--";       //these could be all spaces, but this shows when the time hasn't been received
    if (timeStatus() == timeSet){
        tnow = now();
        sprintf(dstr,"%d/%s", day(tnow), monthShortStr(month(tnow)));
        sprintf(tstr,"%2.2d:%2.2d:%2.2d", hour(tnow), minute(tnow), second(tnow));
    }
    lcd.setCursor(0,0);
    lcd.print(dstr);
    lcd.setCursor(8,0);
    lcd.print(tstr);
    char tempstr[] = "--.-\xdf" "C";       //gap to stop c being interpreted as hex
    char humstr[] = " --%";     //space for 100%
    if (lastValues.humidity != 0.0){
        sprintf(tempstr, "%.1f\xdf" "C", lastValues.temperature);
        sprintf(humstr, "%3.0f%%", lastValues.humidity);
    }
    lcd.setCursor(1,1);
    lcd.print(tempstr);
    lcd.setCursor(11,1);
    lcd.print(humstr);
  
}
