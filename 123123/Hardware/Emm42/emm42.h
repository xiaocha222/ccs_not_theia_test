#ifndef __EMM42_H__
#define __EMM42_H__

#include <stdint.h>
#include "system.h"

// 基础串口函数 (需用户实现)
void uart_write_byte(uint8_t data);
uint8_t uart_read_byte(void);

// 电机控制命令
#define CMD_ENABLE    0xF3  //使能/失能
#define CMD_SPEED     0xF6  //速度模式
#define CMD_POSITION  0xFD  //位置模式

// 方向定义
#define DIR_CW        0x01  // 顺时针
#define DIR_CCW       0x00  // 逆时针

// 使能状态
#define ENABLE_ON     0x01  //ON
#define ENABLE_OFF    0x00  //OFF

// 协议常量
#define SYNC_FLAG     0x00  //多机同步标志（一般关闭）
#define CHECK_BYTE    0x6B  //校验位

// 函数声明
void send_enable_cmd(uint8_t id, uint8_t enable_status);
void send_speed_cmd(uint8_t id, uint8_t direction, uint16_t speed, uint8_t acceleration);
void send_position_cmd(uint8_t id, uint8_t direction, uint16_t speed, uint8_t acceleration, uint32_t pulses, uint8_t mod);
void Emm42_EN_init(void);
void Emm42_DEN(void);
void CMD_task(void);
void origin_set(uint8_t id);
void origin_run(uint8_t id);
void origin_quit(uint8_t id);
#endif // EMM_SERVO_H