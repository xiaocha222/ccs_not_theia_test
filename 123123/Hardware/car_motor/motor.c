#include "motor.h"
#include "hc_hal_motor.h"

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
    // 左轮方向控制
	if (A > 0)	// 正转
	{
        hc_hal_motor_set_dir(HC_MOTOR_ID_LEFT, HC_MOTOR_DIR_FORWARD);
	}              
	else if (A < 0)		// 反转
	{
		A = -A;
        hc_hal_motor_set_dir(HC_MOTOR_ID_LEFT, HC_MOTOR_DIR_REVERSE);
	}
    else 
    {
        hc_hal_motor_set_dir(HC_MOTOR_ID_LEFT, HC_MOTOR_DIR_STOP);
    }    
	
    // 右轮方向控制
	if (B > 0)
	{
        hc_hal_motor_set_dir(HC_MOTOR_ID_RIGHT, HC_MOTOR_DIR_FORWARD);
	}
	else if (B < 0)
	{
		B = -B;
        hc_hal_motor_set_dir(HC_MOTOR_ID_RIGHT, HC_MOTOR_DIR_REVERSE);
	}
	else 
    {
        hc_hal_motor_set_dir(HC_MOTOR_ID_RIGHT, HC_MOTOR_DIR_STOP);
    }
    
	// 改变占空比输出 [0, 2000]
    hc_hal_motor_set_pwm(HC_MOTOR_ID_LEFT, A);
    hc_hal_motor_set_pwm(HC_MOTOR_ID_RIGHT, B);
}

/**
 * @brief 电机失能（运动停止）
 * @param  None
 * @retval  None
 **/
void car_stop(void)
{
    hc_hal_motor_set_dir(HC_MOTOR_ID_LEFT, HC_MOTOR_DIR_STOP);
    hc_hal_motor_set_dir(HC_MOTOR_ID_RIGHT, HC_MOTOR_DIR_STOP);
}
