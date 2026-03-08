#include "emm42.h"

// int acc = 200;
// int spd = 60;

// void Position_CMD_run(void)
// {
//     if(g_tSetpper_Y_PID.out>=0)
//     {
//         send_speed_cmd(1, 1, (uint32_t)g_tSetpper_Y_PID.out, acc);
//     }
//     else if (g_tSetpper_Y_PID.out<0) {
//         send_speed_cmd(1, 0, -(uint32_t)g_tSetpper_Y_PID.out, acc);
//     }
//     delay_cycles(80000000/1000);
//     if(g_tSetpper_X_PID.out>=0)
//     {
//         send_speed_cmd(2, 1, (uint32_t)g_tSetpper_X_PID.out, acc);
//     }
//     else if (g_tSetpper_X_PID.out<0) {
//         send_speed_cmd(2, 0, -(uint32_t)g_tSetpper_X_PID.out, acc);
//     }
//     uart_time = 1;
//     protect_time_out();
// }

int acc = 0;
int spd = 30;

void CMD_task(void)
{
    if(g_tSetpper_Y_PID.out>=0)
    {
        send_position_cmd(1, 1, spd, acc, (uint32_t)g_tSetpper_Y_PID.out, 0);
    }
    else if (g_tSetpper_Y_PID.out<0) {
        send_position_cmd(1, 0, spd, acc, -(uint32_t)g_tSetpper_Y_PID.out, 0);
    }
    mspm0_delay_ms(1);
    if(g_tSetpper_X_PID.out>=0)
    {
        send_position_cmd(2, 1, spd, acc, (uint32_t)g_tSetpper_X_PID.out, 0);
    }
    else if (g_tSetpper_X_PID.out<0) {
        send_position_cmd(2, 0, spd, acc, -(uint32_t)g_tSetpper_X_PID.out, 0);
    }
    uart_time = 1;
    protect_time_out();
}

void uart_write_byte(uint8_t data) {
    DL_UART_Main_transmitDataBlocking(UART_StepperCMD_INST, data);//按照库函数自由调用 
}

uint8_t uart_read_byte(void) {
    // 用户需在此实现具体平台的字节读取逻辑
    // 示例伪代码: HAL_UART_Receive(&huart, &data, 1, timeout);
    return 0; // 返回实际读取的字节
}

// 电机使能控制
void send_enable_cmd(uint8_t id, uint8_t enable_status) {
    uint8_t cmd[] = {
        id,           // 地址
        CMD_ENABLE,   // 使能命令
        0xAB,         // 固定标识
        enable_status,// 使能状态
        SYNC_FLAG,    // 多机同步标志
        CHECK_BYTE    // 校验字
    };
    
    for (int i = 0; i < sizeof(cmd); i++) {
        uart_write_byte(cmd[i]);
    }
}

// 速度模式控制
void send_speed_cmd(uint8_t id, uint8_t direction, uint16_t speed, uint8_t acceleration) {
    uint8_t cmd[] = {
        id,                   // 地址
        CMD_SPEED,            // 速度命令
        direction,            // 方向
        (uint8_t)(speed >> 8), // 速度高字节
        (uint8_t)(speed),      // 速度低字节
        acceleration,         // 加速度
        SYNC_FLAG,            // 多机同步标志
        CHECK_BYTE            // 校验字
    };
    
    for (int i = 0; i < sizeof(cmd); i++) {
        uart_write_byte(cmd[i]);
    }
}

// 位置模式控制
void send_position_cmd(uint8_t id, uint8_t direction, uint16_t speed, 
                      uint8_t acceleration, uint32_t pulses, uint8_t mod) {
    uint8_t cmd[] = {
        id,                    // 地址
        CMD_POSITION,          // 位置命令
        direction,             // 方向
        (uint8_t)(speed >> 8),  // 速度高字节
        (uint8_t)(speed),       // 速度低字节
        acceleration,          // 加速度
        (uint8_t)(pulses >> 24), // 脉冲数最高字节
        (uint8_t)(pulses >> 16), // 
        (uint8_t)(pulses >> 8),  // 
        (uint8_t)(pulses),       // 脉冲数最低字节
        (uint8_t)(mod),        // 位置模式（绝对/相对）     
        SYNC_FLAG,             // 多机同步标志
        CHECK_BYTE             // 校验字
    };
    
    for (int i = 0; i < sizeof(cmd); i++) {
        uart_write_byte(cmd[i]);
    }
}

void Emm42_EN_init(void)
{
    mspm0_delay_ms(1);
    send_enable_cmd(1,1);
    mspm0_delay_ms(1);
    send_enable_cmd(2,1);
}

void Emm42_DEN(void)
{
    mspm0_delay_ms(1);
    send_enable_cmd(1,0);
    mspm0_delay_ms(1);
    send_enable_cmd(2,0);
}


/* 发送设置原点命令 */
void origin_set(uint8_t id)
{
    // 设置原点命令帧（5字节协议）
    uint8_t cmd[] = 
    {
        id,        // 设备地址
        0x93,      // 命令码（设置原点位置）
        0x88,      // 固定标识1
        0x01,      // 固定标识2
        0x6B       // 校验码
    };
    
    // 通过串口发送完整命令帧
    for (int i = 0; i < sizeof(cmd); i++) {
        uart_write_byte(cmd[i]);  // 逐字节发送命令数据
    }
}

/* 发送运行原点命令 */
void origin_run(uint8_t id)
{
    uint8_t cmd[] = 
    {
        id,        // 设备地址
        0x9A,      // 命令码（设置原点位置）
        0x00,      // 固定标识1
        0x00,      // 固定标识2
        0x6B       // 校验码
    };
    
    // 通过串口发送完整命令帧
    for (int i = 0; i < sizeof(cmd); i++) {
        uart_write_byte(cmd[i]);  // 逐字节发送命令数据
    }
}

/* 发送退出原点命令 */
void origin_quit(uint8_t id)
{
    uint8_t cmd[] = 
    {
        id,        // 设备地址
        0x9C,      // 命令码（设置原点位置）
        0x48,      // 固定标识1
        0x6B       // 校验码
    };
    
    // 通过串口发送完整命令帧
    for (int i = 0; i < sizeof(cmd); i++) {
    }
}