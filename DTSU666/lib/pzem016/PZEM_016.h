#ifndef PZEM016_REGISTERS_H
#define PZEM016_REGISTERS_H

#include <stdint.h>
#include <ModbusMaster.h>

// --------------------------------------------------------------------------
// 1. MEASUREMENT RESULT REGISTERS (Thanh ghi kết quả đo)
// Use Function Code: 0x04 (Read Input Register)
// [Tài liệu trang 3 - Source: 258]
// --------------------------------------------------------------------------
enum PZEM_Input_Registers : uint16_t {
    // Voltage (Điện áp)
    // Resolution: 0.1V (Ví dụ: đọc được 2200 => 220.0V)
    REG_VOLTAGE       = 0x0000, 

    // Current (Dòng điện) - 32 bit (2 thanh ghi)
    // Resolution: 0.001A (Ví dụ: đọc được 1000 => 1.000A)
    // Low word ở trước, High word ở sau
    REG_CURRENT_L     = 0x0001, 
    REG_CURRENT_H     = 0x0002, 

    // Active Power (Công suất tác dụng) - 32 bit (2 thanh ghi)
    // Resolution: 0.1W (Ví dụ: đọc được 2200 => 220.0W)
    REG_POWER_L       = 0x0003, 
    REG_POWER_H       = 0x0004, 

    // Active Energy (Điện năng tiêu thụ) - 32 bit (2 thanh ghi)
    // Resolution: 1Wh (Ví dụ: đọc được 123 => 123Wh)
    REG_ENERGY_L      = 0x0005, 
    REG_ENERGY_H      = 0x0006, 

    // Frequency (Tần số)
    // Resolution: 0.1Hz (Ví dụ: đọc được 500 => 50.0Hz)
    REG_FREQUENCY     = 0x0007, 

    // Power Factor (Hệ số công suất)
    // Resolution: 0.01 (Ví dụ: đọc được 100 => 1.00)
    REG_PF            = 0x0008, 

    // Alarm Status (Trạng thái cảnh báo)
    // 0xFFFF: Alarm (Quá tải), 0x0000: Normal (Bình thường)
    REG_ALARM_STATUS  = 0x0009  
};

// --------------------------------------------------------------------------
// 2. SLAVE PARAMETER REGISTERS (Thanh ghi tham số cài đặt)
// Use Function Code: 0x03 (Read) or 0x06 (Write)
// [Tài liệu trang 4 - Source: 270]
// --------------------------------------------------------------------------
enum PZEM_Holding_Registers : uint16_t {
    // Power Alarm Threshold (Ngưỡng cảnh báo công suất)
    // Resolution: 1W (Ví dụ: Ghi 2300 => Cảnh báo khi vượt 2300W)
    REG_PARAM_POWER_ALARM = 0x0001, 

    // Modbus RTU Address (Địa chỉ thiết bị)
    // Range: 0x0001 ~ 0x00F7 (1-247)
    // Mặc định thường là 0x01
    REG_PARAM_ADDRESS     = 0x0002  
};


uint16_t Read_Reg(uint16_t reg);


#endif // PZEM016_REGISTERS_H