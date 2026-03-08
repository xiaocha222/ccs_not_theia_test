/*******************************************************************************
 * @file        VOFA+的串口收发
 * @author      eternal_fu
 * @version     V0.0.1
 * @brief       
 * @Date 2024-12-11 01:05:50
 * @LastEditTime 2024-12-11 04:28:17
 * @**************************************************************************** 
 * @attention   修改说明
 * @    版本号      日期                作者      说明
 *      0.0.1       2024年12月11日      fu      仅支持FireWater数据引擎、收发数据
 *******************************************************************************/
#include "uart_vofa.h"



/* VOFA命令处理缓冲区 */
uint8_t g_ucVofaBuf[VOFA_DATAPACK_MAXLEN];

static float vofa_get_data(uint8_t, uint8_t);
static void vofa_set_sram_data(uint8_t, float);
static void vofa_set_flash_data(float);

/* 声明传输变量 */

/*******************************************************************************
 * @brief   通过串口向VOFA+发送数据，输出波形
 * @return {*}
 * @note    none
 * @param {uint8_t} _dataflag   当前打印的数据类别[标志位]
 *******************************************************************************/
void vofa_draw_graphical(uint8_t _dataflag)
{
/*
    FireWater协议格式:
        "<any>:ch0,ch1,ch2,...,chN\n"
        - any和冒号可以为空，但换行(\n)不可省略；
        - any不可以为"image"，这个前缀用于解析图片数据；
        - 此处\n为换行，并非指字符斜杠+字符n；
        - \n也可以为\n\r，或\r\n。
    案例:
        VOFA_printf("%.0f,"		
					"%.0f,"
					"%.4f,"
					"%.4f,"
                    "%.4f\r\n"
					,dis_pid.target, dis_pid.current, dis_pid.kp, dis_pid.kd,  dis_pid.out);
        发送4个曲线:
        VOFA_printf("channels: 1.386578,0.977929,-0.628913,-0.942729\n");
        VOFA_printf("1.386578,0.977929,-0.628913,-0.942729\n")
*/

	/* 可以写一个枚举，通过枚举选择打印指定数据 */
    switch(_dataflag)
	{
        /* 左轮-速度环 */
		case PID_LEFT_MOTOR:
            VOFA_printf("%.2f,%.2f,%.2f,%2f\r\n", 
						g_tLeftMotorPID.current, g_tLeftMotorPID.target, g_tLeftMotorPID.kp, g_tLeftMotorPID.ki);
			break;
		case PID_RIGHT_MOTOR:
			VOFA_printf("%.2f,%.2f,%.2f,%2f\r\n", 
					g_tRightMotorPID.current, g_tRightMotorPID.target, g_tRightMotorPID.kp, g_tRightMotorPID.ki);
		break;
		/* 角度环 */
		case PID_ANGLE:
            VOFA_printf("%.2f,%.2f,%.2f,%2f\r\n",
						g_tAnglePID.current, g_tAnglePID.target, g_tAnglePID.kp, g_tAnglePID.kd);
			break;
        /* 转向环 */
        case PID_TRACK:
            VOFA_printf("%.2f,%.2f,%.2f,%.2f\r\n",
                        g_tTrackPID.current, gBaseSpeed, g_tTrackPID.kp, g_tTrackPID.kd);
        break;
        /* 距离环 */
        case PID_DISTANCE:
            VOFA_printf("%.2f,%.2f,%.2f,%.2f\r\n",
						g_tDistancePID.current, g_tDistancePID.target , yaw, g_tLeftMotorPID.out);
		break;
/*		 左轮-速度环 
		case PID_LEFT_MOTOR:
            VOFA_printf("%.2f,%.2f,%.2f,%2f\r\n", car_pid_kp, turn_pid_kp, now_speed, aim_speed);
			now_speed+=0.02;
			break;*/
        default:
            break;
	}
#if 0 /* 这里仅作参考(可删除) */
    switch(_dataflag)
	{
		case _left_motor_pid:
			//左轮速度环：【发送和接收】
			VOFA_printf("%.2f,"		
						"%.2f,"
						"%.2f,"
						"%.2f\r\n"
						,left_Motor.pid.target, left_Motor.pid.current, left_Motor.pid.kp, left_Motor.pid.ki);
			break;
		case _right_motor_pid:
			//右轮速度环：【发送和接收】
			VOFA_printf("%f,"
						"%f,"
						"%f,"
						"%f\r\n"
						,right_Motor.pid.target, right_Motor.pid.current, right_Motor.pid.kp, right_Motor.pid.ki);
			break;
		case _turn_pid:
			//巡线转向环(摄像头)：
			VOFA_printf("%.2f,"		
						"%.2f,"
						"%.2f\r\n"
						,turn_pid.target, turn_pid.current, turn_pid.kp);
			break;
		case _angle_pid:
			//陀螺仪转向环：
			VOFA_printf("%.0f,"		
						"%.0f,"
						"%.2f,"
						"%.0f\r\n"
						,angle_pid.target, angle_pid.current, angle_pid.kp, angle_pid.kd);
			break;
		case _track_pid:
			//巡线（灰度）转向环：
			VOFA_printf("%.0f,"		
						"%.0f,"
						"%.5f,"
						"%.0f\r\n"
						,track_pid.target, track_pid.current, track_pid.kp, track_pid.kd);
			break;
        case _dis_pid:
			//计步环：
			VOFA_printf("%.0f,"		
						"%.0f,"
						"%.4f,"
						"%.4f,"
                        "%.4f\r\n"
						,dis_pid.target, dis_pid.current, dis_pid.kp, dis_pid.kd,  dis_pid.out);
			break;
	}
#endif
}

/*******************************************************************************
 * @brief   将此函数添加串口处理字节流的程序中(如串口中断接收中)
 *          将处理接收到的VOFA+的命令解析，并执行对应操作
 * @return {*}
 * @note    目前仅实现了VOFA发送数据的解析
 *******************************************************************************/
void vofa_set_data(uint8_t _rx_byte)
{
    static uint8_t end_pos = 0;         // 遍历vofa缓冲区、记录尾部位置、数组当前位置
    static uint8_t head_pos = 0;

    /* 保存命令 */
    g_ucVofaBuf[end_pos] = _rx_byte;

    if (_rx_byte == VOFA_DATAPACK_HEAD) {   /* 找到帧头 */
        head_pos = end_pos;                 /* 数组当前位置就是帧头 */
    }
    else if(_rx_byte == VOFA_DATAPACK_END && g_ucVofaBuf[head_pos] == VOFA_DATAPACK_HEAD) { /* 找到帧尾和帧头时 */
        /* 解析VOFA命令 */
        float VofaData = vofa_get_data(head_pos, end_pos);
        
        /* 解析完，写入存储在SRAM的变量中 */
        vofa_set_sram_data(head_pos, VofaData);

        /* 清理处理完的数据 */
        end_pos = 0;
        head_pos = 0;
        memset(g_ucVofaBuf, 0x00, VOFA_DATAPACK_MAXLEN);
    }

    /* 超出缓冲区大小，清空 */
    if(++end_pos > VOFA_DATAPACK_MAXLEN) {
        end_pos = 0;
        memset(g_ucVofaBuf, 0x00, VOFA_DATAPACK_MAXLEN);
    }
}

/*******************************************************************************
 * @brief   
 * @return {*}
 * @note    _head是帧头位置，_end是帧尾位置，数据为起始在_head+1
 *          c标准函数库, 需包含头文件 stdlib.h
 *          - atof()字串转换为浮点数
 *          - atoi()字串转换为整数
 *******************************************************************************/
static float vofa_get_data(uint8_t _head, uint8_t _end)
{
    char _VofaData[_end - (_head + 1)];
	for(uint8_t i = 0; i < _end; i++)
		_VofaData[i] = g_ucVofaBuf[i + _head + 1]; 

    return atof(_VofaData);	/* 字符串转浮点 */
}

/*******************************************************************************
 * @brief   将接收到的命令，直接写入SRAM空间，掉电丢失
 *          （就是把接收到的值赋给需要的变量）
 * @return {*}
 * @note    在此处配置相关的运动参数
 * ————————————————————————————————————————————————————————————————————————————————————————————————————————————————
 * |   数据号(用于分清是哪个干什么的数据,1-2byte)   |   帧头('=',1byte)   |   数据位(n byte)   |   包尾('\n',1byte)   |
 * ————————————————————————————————————————————————————————————————————————————————————————————————————————————————
 *   形如：
 *       "P1=%.2f\n"
 *       "V1=%.2f\n"
 * 
 *  还记得P1/V1【数据号】吗，就是用来现在分辨是用来给谁的
 *  这里我采用
 *      P1 - car_pid_kp
 *      P2 - turn_pid_kp
 *      I1 - car_pid_ki
 *		I2=%.2f\n
 *  以此类推都可以！
 * 
 *******************************************************************************/
static void vofa_set_sram_data(uint8_t _head, float _data)
{
    uint8_t _id_pos1 = _head - 2;   /* 数据号的第1位 - P/I/... */
    uint8_t _id_pos2 = _head - 1;   /* 数据号的第2位 - 1/2/3/4/...*/
	
// 角度环[1] ————————————————————————————————————
    /* P1 - g_tAnglePID.kp */
	// if (g_ucVofaBuf[_id_pos1] == 'P' && g_ucVofaBuf[_id_pos2] == '1') { 
    //     g_tSetpper_X_PID.kp = _data;
    // }
    // /* D1 - g_tAnglePID.kd */
    // else if (g_ucVofaBuf[_id_pos1] == 'D' && g_ucVofaBuf[_id_pos2] == '1') {
    //     g_tSetpper_X_PID.ki = _data;
    // }
	// /* T1 - g_tAnglePID.target */
    // else if (g_ucVofaBuf[_id_pos1] == 'T' && g_ucVofaBuf[_id_pos2] == '1') {
    //     g_tSetpper_X_PID.kd = _data;
    // }
// // 左轮速度环[2] ————————————————————————————————————
//     /* P2 - g_tLeftMotorPID.kp */
// 	else if (g_ucVofaBuf[_id_pos1] == 'P' && g_ucVofaBuf[_id_pos2] == '2') { 
//         g_tLeftMotorPID.kp = _data;
//     }
//     /* I2 - g_tLeftMotorPID.ki */
//     else if (g_ucVofaBuf[_id_pos1] == 'I' && g_ucVofaBuf[_id_pos2] == '2') {
//         g_tLeftMotorPID.ki = _data;
//     }
// 	/* T2 - g_tAnglePID.target */
//     else if (g_ucVofaBuf[_id_pos1] == 'T' && g_ucVofaBuf[_id_pos2] == '2') {
//         gLeftTargetBuff = _data;
//     }
// // 右轮速度环[2] ————————————————————————————————————
//     /* P2 - g_tLeftMotorPID.kp */
// 	else if (g_ucVofaBuf[_id_pos1] == 'P' && g_ucVofaBuf[_id_pos2] == '3') { 
//         g_tRightMotorPID.kp = _data;
//     }
//     /* I2 - g_tLeftMotorPID.ki */
//     else if (g_ucVofaBuf[_id_pos1] == 'I' && g_ucVofaBuf[_id_pos2] == '3') {
//         g_tRightMotorPID.ki = _data;
//     }
// 	/* T2 - g_tAnglePID.target */
//     else if (g_ucVofaBuf[_id_pos1] == 'T' && g_ucVofaBuf[_id_pos2] == '3') {
//         gRightTargetBuff = _data;
//     }
// 巡线环[3] ————————————————————————————————————
    //     /* P4 - g_tTrackPID.kp */
	// if (g_ucVofaBuf[_id_pos1] == 'P' && g_ucVofaBuf[_id_pos2] == '4') { 
    //     g_tTrackPID.kp = _data/1000;
    // }
    // /* D4 - g_tTrackPID.kd */
    // else if (g_ucVofaBuf[_id_pos1] == 'D' && g_ucVofaBuf[_id_pos2] == '4') {
    //     g_tTrackPID.kd = _data/1000;
    // }
	// /* T4 - g_tTrackPID.target */
    // else if (g_ucVofaBuf[_id_pos1] == 'T' && g_ucVofaBuf[_id_pos2] == '4') {
    //     gBaseSpeed = _data;
    // }
// // 距离环[4] ————————————————————————————————————
//     /* P5 - g_tDistancePID.kp */
// 	if (g_ucVofaBuf[_id_pos1] == 'P' && g_ucVofaBuf[_id_pos2] == '5') { 
//         g_tDistancePID.kp = _data;
//     }
//     /* D5 - g_tDistancePID.kd */
//     else if (g_ucVofaBuf[_id_pos1] == 'D' && g_ucVofaBuf[_id_pos2] == '5') {
//         g_tDistancePID.kd = _data;
//     }
// 	/* T5 - g_tDistancePID.target */
//     else if (g_ucVofaBuf[_id_pos1] == 'T' && g_ucVofaBuf[_id_pos2] == '5') {
//         gDistanceTargetBuff = _data;
//     }
	
// // 转向基础速度 ————————————————————————————————————
// 	if (g_ucVofaBuf[_id_pos1] == 'A' && g_ucVofaBuf[_id_pos2] == '1') { 
//         gBaseSpeed_Angle = _data;
//     }

// // 转向限制速度 ————————————————————————————————————
//     else if (g_ucVofaBuf[_id_pos1] == 'A' && g_ucVofaBuf[_id_pos2] == '2') {
//         g_tAnglePID.limit = _data;
//     }
	    
// // 转向循迹过渡kp ————————————————————————————————————
// 	if (g_ucVofaBuf[_id_pos1] == 'G' && g_ucVofaBuf[_id_pos2] == 'p') { 
//         g_TrnTracTrast_Kp = _data;
//     }

// // 转向循迹过渡kd ————————————————————————————————————
// 	if (g_ucVofaBuf[_id_pos1] == 'G' && g_ucVofaBuf[_id_pos2] == 'd') { 
//         g_TrnTracTrast_Kd = _data;
//     }

//     // 转向循迹过渡kd ————————————————————————————————————
// 	if (g_ucVofaBuf[_id_pos1] == 'X' && g_ucVofaBuf[_id_pos2] == '1') { 
//         g_tTrackPID.limit = _data;
//     }

// 云台Y轴距离环[7] ————————————————————————————————————
    /* P5 - g_tDistancePID.kp */
	// if (g_ucVofaBuf[_id_pos1] == 'P' && g_ucVofaBuf[_id_pos2] == '1') { 
    //     g_tSetpper_Y_PID.kp = _data;
    // }
    // /* D5 - g_tDistancePID.kd */
    // else if (g_ucVofaBuf[_id_pos1] == 'D' && g_ucVofaBuf[_id_pos2] == '1') {
    //     g_tSetpper_Y_PID.ki = _data;
    // }
	// /* T5 - g_tDistancePID.target */
    // else if (g_ucVofaBuf[_id_pos1] == 'T' && g_ucVofaBuf[_id_pos2] == '1') {
    //     g_tSetpper_Y_PID.kd = _data;
    // }

    // 云台X轴距离环[7] ————————————————————————————————————
    /* P5 - g_tDistancePID.kp */
	if (g_ucVofaBuf[_id_pos1] == 'P' && g_ucVofaBuf[_id_pos2] == 'X') { 
        g_tSetpper_X_PID.kp = _data;
    }
    /* D5 - g_tDistancePID.kd */
    else if (g_ucVofaBuf[_id_pos1] == 'I' && g_ucVofaBuf[_id_pos2] == 'X') {
        g_tSetpper_X_PID.ki = _data;
    }
	/* T5 - g_tDistancePID.target */
    else if (g_ucVofaBuf[_id_pos1] == 'D' && g_ucVofaBuf[_id_pos2] == 'X') {
        g_tSetpper_X_PID.kd = _data;
    }
    /* 以此类推 */
    /* 注意假如是int数据-记得用数据类型转换(int) */
}

/*******************************************************************************
 * @brief   将接收到的命令，直接写入FLASH空间，掉电不丢失
 *          （提前在FLASH空间开辟了参数空间，实现了内部FLASH的API）
 * @return {*}
 * @note    none
 *******************************************************************************/
static void vofa_set_flash_data(float _data)
{
    
}
