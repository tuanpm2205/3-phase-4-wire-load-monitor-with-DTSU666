#include <stdio.h>
#include <stdint.h>
#include "ModbusMaster.h"
#include "LiquidCrystal_I2C.h"
#include "PZEM_016.h"
#include <WiFi.h>
#include "time.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

float U, I;
unsigned long tmr = 0;

const char* ssid = "Redmi Note 12 Pro Speed";
const char* password = "1111111i";

// NTP server
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;   // UTC+7
const int   daylightOffset_sec = 0;
char timeStr[16]; // "dd/mm hh:mm"

void setup(){
    Serial.begin(9600);
    //Init_dtsu666();

    WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    Init_Pzem();
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0,0);
    lcd.print("Connected!");

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }

    lcd.clear();
}


void loop(){
    if (millis() - tmr < 1000) return;
    tmr = millis();

    float P = PZEM_Read_Power();


    if (isnan(P)) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("PZEM ERROR     ");
        return;
    }

    char bufP[16];

    snprintf(bufP, sizeof(bufP), "%4.2f", P);

    lcd.clear();

    lcd.setCursor(0,1);
    lcd.print("P:");
    lcd.print(bufP);
    lcd.print("W ");

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.println(&timeinfo, "%d/%m/%Y %H:%M:%S");
    }

    strftime(timeStr, sizeof(timeStr),
            "%d/%m %H:%M",
            &timeinfo);

    lcd.setCursor(0, 0);
    lcd.print(timeStr);


}
