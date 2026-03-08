#ifndef __MOTOR_H__
#define __MOTOR_H__

//电机，PWM控制文件
//PWM控制
//电机控制

//***********************************//
//**
//**			  头文件
//**
#include "system.h"



//***********************************//
//**
//**		电机基本参数：用于计算
//**
//编码器电机参数配置
#define RESOLUTION_RATIO 		13                    // 编码器的分辨率 即编码器的线数 13线
#define RESOULTION_TOTAL_RATIO (RESOLUTION_RATIO * 2) // 经过备频后的分辨率  中断读取编码器脉冲 二倍频
#define REDUCTION_RATIO 		20                    // 电机的减速比     MG370减速比 1：34  MG310减速比  1：20     电机转一圈脉冲为520个
#define PID_PERIOD 				5                     /* 定时器周期5ms */
#define PULSE_PRE_ROUND			260					  //一圈的脉冲值

//其他相关参数在control.h

//***********************************//
//**
//**		初始化函数
//**
/**
 * @brief 电机初始化
 * @param None
 * @retval None
 * @note 左轮：PA7(+) PA3(-)；右轮：PD2(+) PD3(-)
 */
void motor_Init(void);

/**
 * @brief 电机PWM初始化（10KHZ）
 * @param None
 * @retval None
 */
void motor_pwm0_Init(void);

//***********************************//
//**
//**		控制函数
//**
/**
 * @brief 双轮控制（加载）
 * @param A A电机值(左)，>0正转，<0反转
 * @param B B电机值(右)，>0正转，<0反转
 * @retval None
 */
void car_control(int A, int B);
//还有个百分比控制占空比的，感觉不精确就没用

/**
 * @brief 电机失能（运动停止）
 * @param  None
 * @retval  None
 **/
void car_stop(void);


#endif
