#include <stdio.h>
#include <stdint.h>
#include "dtsu666.h"
#include "ModbusMaster.h"

void setup(){
    Serial.begin(9600);
    Init_dtsu666();
}

void loop(){
    Serial.print("Ua:");
    Serial.println(Read_Voltage_A());
    Serial.print("Ub:");
    Serial.println(Read_Voltage_B());
    Serial.print("Uc:");
    Serial.println(Read_Voltage_C());
    delay(5000);
}