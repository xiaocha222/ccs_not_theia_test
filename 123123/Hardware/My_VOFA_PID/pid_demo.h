/*******************************************************************************
 * @file        
 * @author      eternal_fu
 * @version     V0.0.1
 * @brief       
 * @Date 2024-12-11 15:25:18
 * @LastEditTime 2024-12-11 16:37:29
 * @**************************************************************************** 
 * @attention   修改说明
 * @    版本号      日期         作者      说明
 *******************************************************************************/
#ifndef __PID_DEMO_H__
#define __PID_DEMO_H__

#include "system.h"
/* pid结构体 */
typedef struct
{
    float kp;					/* 比例 */
    float ki;					/* 积分 */
    float kd;					/* 微分 */
    float target;				/* 目标值 */
    float current;				/* 当前值 */
    float out;					/* 执行量 */
    float limit;                /* PID(out)输出限幅值 */

    float error;				/* 当前误差 */
    float last_error;			/* 上一次误差 */
    float last2_error;			/* 上上次误差 */
    float last_out;             /* 上一次执行量 */
	float integral;				/* 积分（累加） */
	float p_out,i_out,d_out;	/* 各控制量输出值 */
}PID_T;
/* —————————————————————————————缓冲区————————————————————————————*/
extern float gLeftTargetBuff;
extern float gRightTargetBuff;
extern float gAngleTargetBuff;
extern float gDistanceTargetBuff;
extern float gBaseSpeed;
extern float gBaseSpeed_Angle;

extern float g_TrnTracTrast_Kp ;
extern float g_TrnTracTrast_Kd ;

extern float g_TrackKp_Buff;
extern float g_TrackKd_Buff;
extern float g_BaseSpeed_Buff;

extern float gSetpper_X_TargetBuff;
extern float gSetpper_Y_TargetBuff;


/* ——————————————————————————要调的PID对象————————————————————————*/
/* 角度环 */
extern PID_T g_tAnglePID;
/* 左轮速度环 */
extern PID_T g_tLeftMotorPID;
/* 右轮速度环 */
extern PID_T g_tRightMotorPID;
/* 灰度转向环 */
extern PID_T g_tTrackPID;	
/*距离环*/
extern PID_T g_tDistancePID;
/*云台X轴距离环*/
extern PID_T g_tSetpper_X_PID;
/*云台Y轴距离环*/
extern PID_T g_tSetpper_Y_PID;

/*
    供用户调用的API
*/
void pid_Init(void);
void pid_closeloop_angle(float _angle_target);
void pid_closeloop_motor(float _left_target, float _right_target);
void pid_closeloop_track(float _track_target);
void pid_closeloop_distance(float _distance_target);
void pid_Setpper_X_distance(float _distance_target);
void pid_Setpper_Y_distance(float _distance_target);
#if 0 /* 用户要用的值只有out */
    /* 在pid_closeloop_angle后，用户使用 */
    多环控制
		只用一个应用环：执行单元的PID输入(速度环input/目标值) = g_tAnglePID.out
	
		只用N个应用环：(边巡线、边角度) 执行单元的PID输入(速度环input/目标值) = g_tAnglePID.out + 巡线pid.out`
	
	car_run_closeloop();
    car_run(左轮速度环PID输出值, 同理)
	
while()
	pid_closeloop_angle(90) => g_tAnglePID.out

	pid_closeloop_motor(初速度 + _g_tAnglePID.out, 右轮初速度 - g_tAnglePID.out) // 要算两个轮子的PID

	car_run(leftpid.out rightpid.out)
#endif

/*
    PID相关的功能函数
    - 增量式
    - 位置式
    - 滤波算法
        - 卡尔曼滤波
        - ……
*/


#endif

