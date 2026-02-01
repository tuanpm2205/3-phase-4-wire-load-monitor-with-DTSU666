#include <stdio.h>
#include <stdint.h>
#include "dtsu666.h"
#include "ModbusMaster.h"
#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

float U, I;
unsigned long tmr = 0;

void setup(){
    Serial.begin(9600);
    Init_dtsu666();
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Xin chao!");
    delay(1000);
    lcd.clear();
}


void loop(){

}
