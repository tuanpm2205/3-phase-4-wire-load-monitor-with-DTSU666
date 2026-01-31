#include <stdio.h>
#include <stdint.h>
#include "dtsu666.h"
#include "ModbusMaster.h"
#include "LiquidCrystal_I2C.h"
#include "PZEM_016.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

float U, I;
unsigned long tmr = 0;

void setup(){
    Serial.begin(9600);
    //Init_dtsu666();
    Init_Pzem();
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Xin chao!");
    delay(1000);
    lcd.clear();
}


void loop(){
    if (millis() - tmr < 1000) return;
    tmr = millis();

    float U = PZEM_Read_Voltage();
    float I = PZEM_Read_Current();

    if (isnan(U) || isnan(I)) {
        lcd.setCursor(0,0);
        lcd.print("PZEM ERROR     ");
        return;
    }

    char bufV[16];
    char bufI[16];

    snprintf(bufV, sizeof(bufV), "%6.1f", U);
    snprintf(bufI, sizeof(bufI), "%6.2f", I);


    lcd.setCursor(0,0);
    lcd.print("V:");
    lcd.print(bufV);
    lcd.print("V ");

    lcd.setCursor(0,1);
    lcd.print("I:");
    lcd.print(bufI);
    lcd.print("A ");
}
