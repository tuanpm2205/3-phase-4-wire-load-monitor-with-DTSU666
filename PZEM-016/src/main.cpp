#include <stdio.h>
#include <stdint.h>
#include "ModbusMaster.h"
#include "LiquidCrystal_I2C.h"
#include "PZEM_016.h"
#include <WiFi.h>
#include "time.h"
#include <WebServer.h>
#include <LittleFS.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

float U, I;
unsigned long tmr = 0;

const char* ssid = "Redmi Note 12 Pro Speed";
const char* password = "1111111i";

const char* ap_ssid = "ESP32_Local_Network";
const char* ap_password = "11111111";

WebServer server(80);

// NTP server
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;   // UTC+7
const int   daylightOffset_sec = 0;
char timeStr[16]; // "dd/mm hh:mm"

void setup(){

    Serial.begin(9600);
    Init_Pzem();
    lcd.init();
    lcd.backlight();
    WiFi.mode(WIFI_AP_STA);

    if (!LittleFS.begin(true)) { // true: format nếu chưa được khởi tạo
        Serial.println("Lỗi: Không thể mount LittleFS!");
        return;
    }
    Serial.println("Khởi tạo LittleFS thành công.");

    WiFi.softAP(ap_ssid, ap_password);
    Serial.print("Đã phát Wi-Fi (AP). IP của ESP32 là: ");
    Serial.println(WiFi.softAPIP());

    WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        lcd.setCursor(0,0);
        lcd.print("Connecting...");
    }

    Serial.println("\nWiFi connected");
    lcd.setCursor(0,0);
    lcd.print("Connected!");

    server.on("/", HTTP_GET, []() {
        Serial.println("Có client truy cập vào trang chủ.");
        
        // Mở file index.html từ LittleFS
        File file = LittleFS.open("/index.html", "r");
        
        if (!file) {
        server.send(404, "text/plain", "Loi: Khong tim thay file index.html trong LittleFS!");
        return;
        }

        // Stream nội dung file HTML cho trình duyệt
        server.streamFile(file, "text/html");
        file.close();
    });

    server.onNotFound([]() {
    server.send(404, "text/plain", "404: Not Found");
    });

    server.begin();
    Serial.println("HTTP server đã khởi chạy!");

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }

    lcd.clear();
}


void loop(){

    server.handleClient();

    if (millis() - tmr >= 5000){
        tmr = millis();

        float P = PZEM_Read_Power();

        if (isnan(P)) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("PZEM ERROR");
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

        strftime(timeStr, sizeof(timeStr), "%d/%m %H:%M", &timeinfo);

        lcd.setCursor(0, 0);
        lcd.print(timeStr);
        } else {
            // Nếu mất mạng hoặc chưa đồng bộ được giờ
            lcd.setCursor(0, 0);
            lcd.print("--/-- --:--");
        }
    }

}
