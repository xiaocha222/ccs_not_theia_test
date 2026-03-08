#ifndef __SYSTEM_H__
#define __SYSTEM_H__

//***********************************//
//**
//**			  头文件
//**
//C库
#include <stdint.h>					//uint32之类的	
#include <stdbool.h>				//true/bool
#include <stdio.h>					//printf重定义
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

#include "ti/driverlib/dl_timerg.h"
// #include "ti/driverlib/dl_uart.h"
#include "ti/driverlib/m0p/dl_core.h"
#include "ti_msp_dl_config.h"


//bsp（自定义文件）
#include "motor.h"
#include "app.h"
#include "my_timer.h"
#include "encoder.h"
#include "uart_vofa.h"
#include "pid_demo.h"
#include "my_uart.h"

#include "clock.h"
#include "interrupt.h"
#include "oled_hardware_i2c.h"
#include "mpu6050.h"
#include "key.h"
#include "graysensor.h"
#include "emm42.h"


/* =============================状态机============================= */
/* 状态机标志位 */
typedef enum // 数字编号从第一个枚举变量往下依次+1
{
    SYS_STA_INIT = 0,       // 初始化  =0
    SYS_STA_DEBUG,          // 调试    =0+1=1
    SYS_STA_CHOOSE,         // 选题
    DEBUG_Q1,               // 基本部分第一问
    DEBUG_Q2,               // 基本部分第二问
    DEBUG_Q3,               // 基本部分第三问
    BASIC_Q1,               // 基本部分第一问
    BASIC_Q2,               // 基本部分第二问
    BASIC_Q3,               // 基本部分第三问
    EXTRA_Q1,               // 发挥部分1
    EXTRA_Q2,               // 发挥部分1
    EXTRA_Q3,               // 发挥部分1
    SYS_STA_STOP,           // 停止
    // --------
    SYS_STA_VelocityLoopTest,

    // --------
    SYS_STA_MAX_NUM // 状态机最大状态数（从0开始，最后一位，刚好是最大状态数）
}    
SYS_FLAG_STA_E;

extern SYS_FLAG_STA_E g_eSysFlagManage;		// 状态机标志位

/* 状态机对应的任务 */
typedef struct
{
	uint8_t	 SysStaID;			    // 当前任务所属的状态机
    uint32_t Run;					// 任务定时时间到了，可以开始执行
    uint32_t TimCount;				// 任务计时器初始值
    uint32_t TimRload;				// 任务计时器重载值
    void (*pTaskFunc)(void);		// 该任务对应的函数
} TaskComps_T;

//***********************************//
//**
//**			  变量
//**


/* =============================系统函数============================= */
extern void SysInit(void);
extern void SysRun(void);
extern void TaskTimeSliceManage(void);



#endif