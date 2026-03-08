#include "motor.h"

//定义电机结构体变量【在control中定义了】
//struct _Motor left_Motor,right_Motor = {0};

//***********************************//
//**
//**		控制函数：电机控制，电机限幅，电机失能
//**

/**
 * @brief 双轮控制（加载）【通过占空比控制】
 * @param 左轮PWM占空比：A A电机值(左)，>0正转，<0反转
 * @param 右轮PWM占空比：B B电机值(右)，>0正转，<0反转
 * @retval None
 * @note A,B的取值范围[0,2000]，这里A,B代表的占空比
 * 使用了float，通过int取整来控制百分百更加精确
 */
void car_control(int A, int B)
{
    //左轮
	if (A > 0)	//正转
	{

		DL_GPIO_setPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN1_PIN);
        DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN2_PIN);
	}              
	else if (A < 0)		//反转
	{
		A = -A;
		DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN1_PIN);
		DL_GPIO_setPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN2_PIN);
	}
    else 
    {
        DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN1_PIN);
		DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN2_PIN);
    }    
	
    //右轮
	if (B > 0)
	{

		DL_GPIO_setPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_right_IN1_PIN);
        DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_right_IN2_PIN);
	}
	else if (B < 0)
	{
		B = -B;
        DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_right_IN1_PIN);
		DL_GPIO_setPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_right_IN2_PIN);

	}
	else 
    {
        DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_right_IN1_PIN);
		DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_right_IN2_PIN);
    }
    
	//改变占空比输出 [0, 2000]
    DL_TimerG_setCaptureCompareValue(Motor_PWM_INST, 2000 - A, DL_TIMER_CC_0_INDEX);    //这个2000是PWM周期
    DL_TimerG_setCaptureCompareValue(Motor_PWM_INST, 2000 - B, DL_TIMER_CC_1_INDEX);
}

/**
 * @brief 电机失能（运动停止）
 * @param  None
 * @retval  None
 **/
void car_stop(void)
{
	DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN1_PIN);
	DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN2_PIN);
	
	DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_right_IN1_PIN);
	DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_right_IN2_PIN);
}
