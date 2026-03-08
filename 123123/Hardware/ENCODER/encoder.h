#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "system.h"

extern int EncoderLeftBuff;

//****** 电机部分 ******//
typedef struct _Motor
{
	int8_t direct;      	//旋转方向
	int Time_counter;		//一定周期内的脉冲计数	：测速【2倍频，未处理】
	int Alway_counter;		//累计计数				：计步【2倍频，已处理】
	
	float n_rpm_Speed;      //电机转速(每秒多少转)
	float v_Speed;			//电机线速度(每秒多少毫米)
}Motor;


//使用了外部中断来测速/编码器控制

//***********************************//
//**
//**		使用函数
//**
/**
 * @brief GPIO外部中断，读取编码器速度
 * @param None
 * @param None
 * @retval GROUP1中断组的中断服务函数（GPIOA，GPIOB都在这个组）——用于编码器的外部中断测速
 */
void GROUP1_IRQHandler(void);

//***********************************//
//**
//**		变量
//**
extern Motor left_Motor;
extern Motor right_Motor; 

#endif
