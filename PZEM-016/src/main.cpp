    #include <stdio.h>
    #include <stdint.h>
    #include "ModbusMaster.h"
    #include "LiquidCrystal_I2C.h"
    #include "PZEM_016.h"
    #include <WiFi.h>
    #include "time.h"
    #include <AsyncTCP.h>
    #include <ESPAsyncWebServer.h>
    #include <LittleFS.h>

    LiquidCrystal_I2C lcd(0x27, 16, 2);

    float U, I;
    float current_P = 0.0;
    unsigned long tmr = 0;

    const char* ssid = "Redmi Note 12 Pro Speed";
    const char* password = "1111111i";

    AsyncWebServer server(80);

    // Khai báo đường dẫn file CSV
    const char* dataFilePath = "/data.csv";

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
        WiFi.mode(WIFI_STA);

        if (!LittleFS.begin(true)) { // true: format nếu chưa được khởi tạo
            Serial.println("Lỗi: Không thể mount LittleFS!");
            return;
        }
        Serial.println("Khởi tạo LittleFS thành công.");

        if (!LittleFS.exists(dataFilePath)) {
            File file = LittleFS.open(dataFilePath, "w");
            if(file) {
                file.println("Date,Time,Power(W)");
                file.close();
                Serial.println("Đã tạo file data.csv mới.");
            }
        }

        WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
            lcd.setCursor(0,0);
            lcd.print("Connecting...");
        }

        Serial.println("\nWiFi connected");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP()); // In ra Serial Monitor
        lcd.setCursor(0,0);
        lcd.print("Connected!");

        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            Serial.println("Client truy cập trang chủ");
            // Gửi file HTML trực tiếp từ LittleFS cực kỳ đơn giản
            request->send(LittleFS, "/index.html", "text/html");
        });

        server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request){
            struct tm timeinfo;
            String tStr = "--:--:--";
            if (getLocalTime(&timeinfo)) {
                char timeBuf[10];
                strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &timeinfo);
                tStr = String(timeBuf);
            }
            
            // current_P là biến toàn cục lưu công suất đo được ở hàm loop()
            String json = "{\"power\":" + String(current_P) + ", \"time\":\"" + tStr + "\"}";
            request->send(200, "application/json", json);
        });

        server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
            Serial.println("Client yêu cầu tải file data.csv");
            if (LittleFS.exists(dataFilePath)) {
                // Tham số 'true' ở cuối sẽ ép trình duyệt tải file về thay vì hiển thị
                request->send(LittleFS, dataFilePath, "text/csv", true);
            } else {
                request->send(404, "text/plain", "File khong ton tai! Chua co du lieu.");
            }
        });

        server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request){
            Serial.println("Client yêu cầu xóa file data.csv");
            if (LittleFS.exists(dataFilePath)) {
                LittleFS.remove(dataFilePath);
                // Ghi lại tiêu đề cột cho file CSV mới
                File file = LittleFS.open(dataFilePath, "w");
                file.println("Date,Time,Power(W)");
                file.close();
                
                // Trả về mã script nhỏ để báo thành công và chuyển hướng về trang chủ
                request->send(200, "text/html", "<script>alert('Da xoa du lieu!'); window.location.href='/';</script>");
            } else {
                request->send(404, "text/plain", "Khong tim thay file de xoa.");
            }
        });

        server.onNotFound([](AsyncWebServerRequest *request){
            request->send(404, "text/plain", "404: Not Found");
        });

        server.begin();
        Serial.println("HTTP server đã khởi chạy!");


        //Config server định thời
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
            return;
        }

        // Clear LCD
        lcd.clear();
    }


    void loop(){
        // Chu kỳ đọc data
        if (millis() - tmr >= 5000){
            tmr = millis();

            float P = PZEM_Read_Power();

            //Nếu kết quả đọc công suất trả về là NAN
            if (isnan(P)) {
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("PZEM ERROR");
                return;
            }

            //Khởi tạo biến để xuất ra chart
            current_P = P;

            //Khởi tạo biến in ra LCD1602
            char bufP[16];
            snprintf(bufP, sizeof(bufP), "%4.2f", P);

            lcd.clear();

            lcd.setCursor(0,1);
            lcd.print("P:");
            lcd.print(bufP);
            lcd.print("W ");

            struct tm timeinfo;
            if (getLocalTime(&timeinfo)) {
                char dateBuf[15];
                char timeBuf[10];
                strftime(dateBuf, sizeof(dateBuf), "%d/%m/%Y", &timeinfo);
                strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &timeinfo);
                
                strftime(timeStr, sizeof(timeStr), "%d/%m %H:%M", &timeinfo);
                lcd.setCursor(0, 0);
                lcd.print(timeStr);

                // Ghi data sau khi đọc được thời gian
                File file = LittleFS.open(dataFilePath, "a");
                if (file) {
                    file.printf("%s,%s,%.2f\n", dateBuf, timeBuf, P);
                    file.close();
                    Serial.println("Đã ghi 1 dòng vào data.csv");
                } else {
                    Serial.println("Lỗi: Không mở được file CSV để ghi!");
                }

            } else {
                // Nếu không lấy được thời gian
                lcd.setCursor(0, 0);
                lcd.print("--/-- --:--");
            }
        }

    }
