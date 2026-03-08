#ifndef __APP_H_
#define __APP_H_
#include "system.h"

/*===========================================重定义=======================================*/
#define DISTANCE_3_4    2300
#define DISTANCE_1      1750
#define DISTANCE_2      1750
#define DISTANCE_A_C    1500
#define DISTANCE_B_D    1550
#define ANGLE           0
#define ANGLE_1         0
#define ANGLE_2         180
#define ANGLE_A         52
#define ANGLE_B         127



#define stop  0
#define walkFir 1
#define walkSec 2
#define walkThr 3
#define turnleft        4
#define turnright		5
#define turnaround		6


/*===========================================变量声明=======================================*/
extern uint8_t enter_flag;
extern float _distance_target;
extern uint8_t cho;
extern float start_yaw;                    // 上电时的角度
extern uint8_t g_LevelFlag;
extern uint8_t _NCycle ;

extern uint8_t g_ChosFlag;



extern float _left_target;
extern float _right_target;
extern float _distance_target;
// extern float _all_go;
extern float _track_target;
extern int8_t _cycles;
extern uint8_t _find_target_flag;

/*===========================================函数声明=======================================*/
void debug_task(void);
void enter_inner_task(void);
void Choose_Topic(void);
void Gray_Test();


void pid_closeloop_motor_task(void);
void straight_task(float _distancetarget);
void turn_task(float _target);
void stop_task(void);
void Topic1(void);
void Walk(void);
void Topic1_TEST(void);
void Walk_Basic(void);

void extra1(void);

void topic_4_task(void);
void topic_choose(void);
void track_enter_inner_task(void);

void uart_VofaEncoder_Test(void);
void AngleCal_Task(void);

//云台
void protect_time_out(void);
void basics2(void);
void basics3(void);
void error_stop(void);
void Emm42_Test(void);
#endif