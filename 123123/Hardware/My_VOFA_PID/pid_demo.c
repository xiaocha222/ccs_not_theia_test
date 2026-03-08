/*******************************************************************************
 * @file        pid_demo
 * @author      eternal_fu
 * @version     V0.0.1
 * @brief       实现了PID公式，各种环的闭环控制，部分PID功能函数
 * @Date 2024-12-11 15:25:07
 * @LastEditTime 2024-12-11 16:35:47
 * @**************************************************************************** 
 * @attention   修改说明
 * @    版本号      日期         作者      说明
 *******************************************************************************/
#include "pid_demo.h"

/* 声明功能函数 */
static void pid_formula_incremental(PID_T * _tpPID);
static void pid_formula_positional(PID_T * _tpPID);
static void pid_out_limit(PID_T * _tpPID);

/* 当前系统的PID */
PID_T g_tAnglePID = {0};        /* 角度环 */
PID_T g_tLeftMotorPID = {0};    /* 左轮速度环 */
PID_T g_tRightMotorPID = {0};   /* 右轮速度环 */
PID_T g_tTrackPID={0};			/*灰度转向环*/
PID_T g_tDistancePID={0};		/*距离环*/
//...【2】
PID_T g_tSetpper_X_PID={0};		/*云台X轴距离环*/
PID_T g_tSetpper_Y_PID={0};		/*云台Y轴距离环*/
//...【2】
float gSetpper_X_TargetBuff = 0;
float gSetpper_Y_TargetBuff = 0;
/* 储存缓冲区变量 */
float gLeftTargetBuff = 0;
float gRightTargetBuff = 0; 
float gAngleTargetBuff = 0;
float gDistanceTargetBuff = 0;
float gTrackCurrentBuff = 0;

float gBaseSpeed = 1.5;
float gBaseSpeed_Angle = 0.8;

float g_TrnTracTrast_Kp = 0.04;
float g_TrnTracTrast_Kd = 0.13;

float g_TrackKp_Buff = 0;
float g_TrackKd_Buff = 0;
float g_BaseSpeed_Buff = 0;

/*******************************************************************************
 * @brief   
 * @return {*}
 * @note    none
 *******************************************************************************/
static void pid_InitVal(void)
{
	/* 系统 - 角度环 */
	g_tAnglePID.kp = 0.06;		        	// 比例
	g_tAnglePID.ki = 0;			    		// 积分
	g_tAnglePID.kd = 0.121 ;			    // 微分
	g_tAnglePID.target = 0;			    	// 目标值[可以在外部调用]
	g_tAnglePID.limit = 0.5;		        	// 限幅值

    /* 系统 - 左轮速度环 */
	g_tLeftMotorPID.kp = 337;		    	// 比例
	g_tLeftMotorPID.ki = 66;				// 积分
	g_tLeftMotorPID.kd = 0.0;				// 微分
	g_tLeftMotorPID.target = 0;				// 目标值[可以在外部调用]
	g_tLeftMotorPID.limit = 2000;		    // 限幅值

	/* 系统 - 右轮速度环 */
	g_tRightMotorPID.kp = 337;		    // 比例
	g_tRightMotorPID.ki = 66;				// 积分
	g_tRightMotorPID.kd = 0.0;				// 微分
	g_tRightMotorPID.target = 0;			// 目标值[可以在外部调用]
	g_tRightMotorPID.limit = 2000;		    // 限幅值
	
	/* 系统 - 转向环 */
	g_tTrackPID.kp=0.101;					// 比例
	g_tTrackPID.ki=0;                   	// 积分
	g_tTrackPID.kd=0.50132;                   	// 微分
	g_tTrackPID.target=0;               	// 目标值[可以在外部调用]
	g_tTrackPID.limit=0.3;                    // 限幅值,转速
	
	/* 系统 - 距离向环 */
	g_tDistancePID.kp=0.01;					// 比例
	g_tDistancePID.ki=0;                   	// 积分
	g_tDistancePID.kd=0;                   	// 微分
	g_tDistancePID.target=0;               	// 目标值[可以在外部调用]
	g_tDistancePID.limit=2;                 // 限幅值,转速

	/* 系统 - 云台X轴距离环 */
	g_tSetpper_X_PID.kp = 0.224;		        	// 比例
	g_tSetpper_X_PID.ki = 0.0;			    	// 积分
	g_tSetpper_X_PID.kd = 0.0068;			    	// 微分
	g_tSetpper_X_PID.target = 0;			    // 目标值[可以在外部调用]
	g_tSetpper_X_PID.limit = 3200;		    	// 限幅值

	/* 系统 - 云台Y轴距离环 */
	g_tSetpper_Y_PID.kp = 0.0;		        	// 比例
	g_tSetpper_Y_PID.ki = 0.0;			    	// 积分
	g_tSetpper_Y_PID.kd = 0;			    	// 微分
	g_tSetpper_Y_PID.target = 0;			    // 目标值[可以在外部调用]
	g_tSetpper_Y_PID.limit = 3200;		   	 	// 限幅值
}




/* —————————————————————————— 供用户调用的API —————————————————————————— */
/*******************************************************************************
 * @brief   
 * @return {*}
 * @note    none
 *******************************************************************************/
void pid_Init(void)
{
    pid_InitVal();
}

/* —————————————————————————— 闭环控制[仅计算出out值] —————————————————————————— */
/*******************************************************************************
 * @brief   角度闭环控制函数[角度环]
 * @return {*}
 * @note    在位置式中，P-快速性，I-准确性，D-稳定性【一般按顺序给P,D】
 *******************************************************************************/
void pid_closeloop_angle(float _angle_target)
{
	if (_angle_target > 360) 
	{
		_angle_target-=360;
	}
	else if (_angle_target < 0) 
	{
		_angle_target+=360;
	}
    /* [更新目标值] */
    g_tAnglePID.target = _angle_target;

    /* [获取反馈值/当前值] - 获取当前角度 */
    // g_tAnglePID.current = current;

    if (g_tAnglePID.target-g_tAnglePID.current > 180.0) {
        g_tAnglePID.current += 360.0;
    } else if (g_tAnglePID.target-g_tAnglePID.current < -180.0) {
        g_tAnglePID.current -= 360.0;
    }
    /* [通过PID公式计算PID输出值] */
    pid_formula_positional(&g_tAnglePID);
    
    /* [限幅PID输出] */
    pid_out_limit(&g_tAnglePID);
}

/*******************************************************************************
 * @brief   电机闭环控制函数[速度环]
 * @return {*}
 * @note    在增量式中，P-稳定性，I-快速性，D-准确性【一般按顺序给I,P】
 *			计算出out值后，在其他地方car_run(out1, out2);
 *******************************************************************************/
void pid_closeloop_motor(float _left_target, float _right_target)
{
    /* [更新目标值] */
    g_tLeftMotorPID.target = _left_target;
    g_tRightMotorPID.target = _right_target;

    /* [获取反馈值/当前值] - 获取当前双轮速度(current) */
    g_tLeftMotorPID.current = left_Motor.n_rpm_Speed;
    g_tRightMotorPID.current = right_Motor.n_rpm_Speed;

    /* [通过PID公式计算PID输出值] */
    pid_formula_incremental(&g_tLeftMotorPID);
    pid_formula_incremental(&g_tRightMotorPID);
    
    /* [限幅PID输出] */
    pid_out_limit(&g_tLeftMotorPID);
    pid_out_limit(&g_tRightMotorPID);
}


/*******************************************************************************
 * @brief   转向闭环控制函数[转向环]
 * @return {*}
 * @note    在位置式中，P-快速性，I-准确性，D-稳定性【一般按顺序给P,D】
 *******************************************************************************/
void pid_closeloop_track(float _track_target)
{
    /* [更新目标值] */
    g_tTrackPID.target = _track_target;

    /* [获取反馈值/当前值] - 获取当前灰度 */
    //  g_tTrackPID.current = (example.DataBuff[2]-128)/10;
	g_tTrackPID.current = gray_status;
	
    /* [通过PID公式计算PID输出值] */
    pid_formula_positional(&g_tTrackPID);
    
    /* [限幅PID输出] */
    pid_out_limit(&g_tTrackPID);
}

/*******************************************************************************
 * @brief   距离闭环控制函数
 * @return {*}
 * @note    在增量式中，P-稳定性，I-快速性，D-准确性【一般按顺序给I,P】
 *			计算出out值后，在其他地方car_run(out1, out2);
 *******************************************************************************/
void pid_closeloop_distance(float _distance_target)
{
    /* [更新目标值] */
    g_tDistancePID.target = _distance_target;

    /* [获取反馈值/当前值] - 获取当前距离 */
    // g_tDistancePID.current =distance;
	
    /* [通过PID公式计算PID输出值] */
    pid_formula_positional(&g_tDistancePID);
    
    /* [限幅PID输出] */
    pid_out_limit(&g_tDistancePID);
}
//【4】添加自己闭环控制
/*******************************************************************************
 * @brief   云台X轴距离环控制函数
 * @return {*}
 * @note    在增量式中，P-稳定性，I-快速性，D-准确性【一般按顺序给I,P】
 *			计算出out值后，在其他地方car_run(out1, out2);
 *******************************************************************************/
void pid_Setpper_X_distance(float _distance_target)
{
    /* [更新目标值] */
    g_tSetpper_X_PID.target = _distance_target;

    /* [获取反馈值/当前值] - 获取当前距离 */
    g_tSetpper_X_PID.current = X_Current;
	
    /* [通过PID公式计算PID输出值] */
    pid_formula_positional(&g_tSetpper_X_PID);
    
    /* [限幅PID输出] */
    pid_out_limit(&g_tSetpper_X_PID);
}

/*******************************************************************************
 * @brief   云台Y轴距离环控制函数
 * @return {*}
 * @note    在增量式中，P-稳定性，I-快速性，D-准确性【一般按顺序给I,P】
 *			计算出out值后，在其他地方car_run(out1, out2);
 *******************************************************************************/
void pid_Setpper_Y_distance(float _distance_target)
{
    /* [更新目标值] */
    g_tSetpper_Y_PID.target = _distance_target;

    /* [获取反馈值/当前值] - 获取当前距离 */
    g_tSetpper_Y_PID.current = Y_Current;
	
    /* [通过PID公式计算PID输出值] */
    pid_formula_positional(&g_tSetpper_Y_PID);
    
    /* [限幅PID输出] */
    pid_out_limit(&g_tSetpper_Y_PID);
}

/* —————————————————————————— PID相关的功能函数 —————————————————————————— */
/*******************************************************************************
 * @brief   
 * @return {*}
 * @note    none
 * @param {PID_T *} _tpPID
 *******************************************************************************/
static void pid_out_limit(PID_T * _tpPID)
{
    if(_tpPID->out > _tpPID->limit)
		_tpPID->out = _tpPID->limit;
    else if(_tpPID->out < -_tpPID->limit)
	    _tpPID->out = -_tpPID->limit;
}

/*******************************************************************************
 * @brief   增量式PID公式
 * @return {*}
 * @note    在增量式中，P-稳定性，I-快速性，D-准确性【一般按顺序给I,P】
 * @param {PID_T *} _tpPID  传入要运算的PID对象的指针
 *******************************************************************************/
static void pid_formula_incremental(PID_T * _tpPID)
{
    _tpPID->error = _tpPID->target - _tpPID->current;															
    
	_tpPID->p_out = _tpPID->kp * (_tpPID->error - _tpPID->last_error);
	_tpPID->i_out = _tpPID->ki * _tpPID->error;
	_tpPID->d_out = _tpPID->kd * (_tpPID->error - 2 * _tpPID->last_error + _tpPID->last2_error);
	
	_tpPID->out += _tpPID->p_out + _tpPID->i_out + _tpPID->d_out;
	
	_tpPID->last2_error = _tpPID->last_error;																
	_tpPID->last_error = _tpPID->error;
}

/*******************************************************************************
 * @brief   位置式PID公式
 * @return {*}
 * @note    在位置式中，P-快速性，I-准确性，D-稳定性【一般按顺序给P,D】
 * @param {PID_T *} _tpPID  传入要运算的PID对象的指针
 *******************************************************************************/
static void pid_formula_positional(PID_T * _tpPID)
{
    _tpPID->error = _tpPID->target - _tpPID->current;
	_tpPID->integral += _tpPID->error;
	
	_tpPID->p_out = _tpPID->kp * _tpPID->error;
	_tpPID->i_out = _tpPID->ki * _tpPID->integral;
	_tpPID->d_out = _tpPID->kd * (_tpPID->error - _tpPID->last_error);
	
	_tpPID->out =  _tpPID->p_out + _tpPID->i_out + _tpPID->d_out;		
	
	_tpPID->last_error = _tpPID->error;
}
