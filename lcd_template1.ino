#include <LiquidCrystal.h>
#include <string.h>

#define RS	12
#define EN	11
#define D4	5
#define D5	4
#define D6	3
#define D7	2

#define COLS 16
#define ROWS 2

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

#define Input A0

char *text[] =
{
    "LOREM IPSUM",
    "ASCII stupid question, get a stupid ANSI",
    "the quick brown fox jumped over the lazy dog"
};

void setup(){
  pinMode( A0, INPUT);  
  lcd.begin(16, 2);
}

void loop(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print( analogRead( A0));
  lcd.setCursor(0,1);
  float mV = 5.0 * analogRead( A0);
  mV = mV /1024;
  lcd.print( mV);
  delay(100);
}
