#include "dtsu666.h"


ModbusMaster node1;

void Init_dtsu666(){
    Serial2.begin(default_baudrate_dtsu666, SERIAL_8N1, RX_Pin_dtsu666, TX_Pin_dtsu666);
    node1.begin(dtsu_slaveid, Serial2);
}


float Read_Voltage_A(){
    return ReadFloat_Value(UA_H);
}

float Read_Voltage_B(){
    return ReadFloat_Value(UB_H);
}

float Read_Voltage_C(){
    return ReadFloat_Value(UC_H);
}

float ReadFloat_Value(uint16_t Reg){
    Data_Convert Float;
    uint8_t result = node1.readHoldingRegisters(Reg, 2);
    if (result == node1.ku8MBSuccess) {
        Float.u32 = ((uint32_t)node1.getResponseBuffer(0) << 16)| node1.getResponseBuffer(1);
        return Float.f;
    }
    return READ_ERROR;
}