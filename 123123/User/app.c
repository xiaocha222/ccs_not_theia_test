#include "app.h"
/*=======================================全局变量=================================*/

/*=======================================其他变量=================================*/
//给OLED显示的
uint8_t oled_buffer[32];
uint8_t oled_buffer2[32];
uint8_t oled_buffer3[32];
uint8_t oled_buffer4[32];

//刷新标志位
uint8_t Refrash = 100;

//转弯标志位
uint8_t RATurn_Flag = 0;
//跑5圈后停车
uint8_t _5CycleStop1 = 0;
uint8_t _5CycleStop2 = 0;
uint8_t _NCycle = 0;

//选题标志位
uint8_t g_ChosFlag= 0;      //内部选题计数
uint8_t g_LevelFlag= 1;     //菜单级计数
uint8_t g_ModelFlag = 0;    //模式计数
uint8_t ContinueFlag = 1;

//每题的行走状态
uint8_t WalkState = 0;
//转向缓冲区
float WalkAngleBuff = 0;
//转向命令
uint8_t TurnBuff = 0;

uint8_t TurnCompltFlag = 0;

float AngleBuff = 0;


float _left_target = 0;
float _right_target = 0;
float _distance_target = 0;
float _all_go = 0;
float _track_target = 0;
uint8_t _find_target_flag = 0;
uint8_t _find_target_quit;
int8_t _cycles = 0;
static uint8_t origin_over = 1; 
uint8_t protect_flag = 1;


/*******************************************************************************
 * @brief   基本部分的行走函数
 * @return  
 * @note    
 *******************************************************************************/
void Walk_Basic(void)
{
    gray_check();
    if(Write_Check())
    {
    //     //使用过渡参数
    //     g_TrackKp_Buff = g_tTrackPID.kp;
    //     g_TrackKd_Buff = g_tTrackPID.kd;
    //     g_BaseSpeed_Buff = gBaseSpeed;
    //     g_tTrackPID.kp = g_TrnTracTrast_Kp;
    //     g_tTrackPID.kd = g_TrnTracTrast_Kd;
    //     gBaseSpeed = 1.65;
        WalkAngleBuff = yaw;
        RATurn_Flag = 1;
        
    }

    else if(_5CycleStop1==1)
    {
        _5CycleStop2++;
        _5CycleStop1 = 0;
    }

    else if(_5CycleStop2 == _NCycle)
    {
        g_eSysFlagManage = SYS_STA_STOP;
    }



    switch(RATurn_Flag)
    {
        case 0:
        {      
    //         if((_5CycleStop1 == 1)&&(gray_status==0))
    //         {
    //             //换回原来的参数
    //             g_tTrackPID.kp = g_TrackKp_Buff;
    //             g_tTrackPID.kd = g_TrackKd_Buff;
    //             gBaseSpeed = 1.65;
                
    //             _5CycleStop2++;
    //             _5CycleStop1 = 0;
    //         }

    //         // else if(_5CycleStop2 == _NCycle)
    //         // {
    //         //     g_eSysFlagManage = SYS_STA_STOP;
    //         // }

    //         gray_check();
            pid_closeloop_track(0);
            pid_closeloop_motor(gBaseSpeed-g_tTrackPID.out, gBaseSpeed+g_tTrackPID.out);
            car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
        }break;

        case 1:
        {
            pid_closeloop_angle(WalkAngleBuff + 90);
            pid_closeloop_motor(gBaseSpeed_Angle-g_tAnglePID.out,gBaseSpeed_Angle+g_tAnglePID.out);
            car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);


            // /* ***角度归一化处理*** */
            // float angle_diff = g_tAnglePID.target - g_tAnglePID.current;
            // AngleBuff = angle_diff;
		    // if (angle_diff > 180) angle_diff -= 360;
		    // if (angle_diff < -180) angle_diff += 360;
		    // if((angle_diff > -25) && (angle_diff <25 ))
		    // {
            //     RATurn_Flag = 0;
            //     g_eSysFlagManage = SYS_STA_STOP;
		    // }

            if((gray_status >= -6)&&(gray_status <= -3))
            {
                // g_eSysFlagManage = SYS_STA_STOP;
                
                _5CycleStop1=1;

                RATurn_Flag = 0;


                // car_control(0, 0);
                // g_eSysFlagManage = SYS_STA_CHOOSE;////////////////////

            }
        }break;

        default:break;

    }
    }

/*******************************************************************************
 * @brief   额外部分的行走函数
 * @return  
 * @note    
 *******************************************************************************/
void Walk(void)
{
    gray_check();
    if(Write_Check())
    {
    //     //使用过渡参数
    //     g_TrackKp_Buff = g_tTrackPID.kp;
    //     g_TrackKd_Buff = g_tTrackPID.kd;
    //     g_BaseSpeed_Buff = gBaseSpeed;
    //     g_tTrackPID.kp = g_TrnTracTrast_Kp;
    //     g_tTrackPID.kd = g_TrnTracTrast_Kd;
    //     gBaseSpeed = 1.65;
        WalkAngleBuff = yaw;
        RATurn_Flag = 1;
        
    }

    else if(_5CycleStop1==1)
    {
        _5CycleStop2++;
        _5CycleStop1 = 0;
    }

    else if(_5CycleStop2 == _NCycle+1)
    {
        g_eSysFlagManage = SYS_STA_STOP;
    }



    switch(RATurn_Flag)
    {
        case 0:
        {      
    //         if((_5CycleStop1 == 1)&&(gray_status==0))
    //         {
    //             //换回原来的参数
    //             g_tTrackPID.kp = g_TrackKp_Buff;
    //             g_tTrackPID.kd = g_TrackKd_Buff;
    //             gBaseSpeed = 1.65;
                
    //             _5CycleStop2++;
    //             _5CycleStop1 = 0;
    //         }

    //         // else if(_5CycleStop2 == _NCycle)
    //         // {
    //         //     g_eSysFlagManage = SYS_STA_STOP;
    //         // }

    //         gray_check();
            pid_closeloop_track(0);
            pid_closeloop_motor(gBaseSpeed-g_tTrackPID.out, gBaseSpeed+g_tTrackPID.out);
            car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
        }break;

        case 1:
        {
            pid_closeloop_angle(WalkAngleBuff + 90);
            pid_closeloop_motor(gBaseSpeed_Angle-g_tAnglePID.out,gBaseSpeed_Angle+g_tAnglePID.out);
            car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);


            // /* ***角度归一化处理*** */
            // float angle_diff = g_tAnglePID.target - g_tAnglePID.current;
            // AngleBuff = angle_diff;
		    // if (angle_diff > 180) angle_diff -= 360;
		    // if (angle_diff < -180) angle_diff += 360;
		    // if((angle_diff > -25) && (angle_diff <25 ))
		    // {
            //     RATurn_Flag = 0;
            //     g_eSysFlagManage = SYS_STA_STOP;
		    // }

            if((gray_status >= -6)&&(gray_status <= -3))
            {
                // g_eSysFlagManage = SYS_STA_STOP;
                
                _5CycleStop1=1;

                RATurn_Flag = 0;


                // car_control(0, 0);
                // g_eSysFlagManage = SYS_STA_CHOOSE;////////////////////

            }
        }break;

        default:break;

    }


    // printf("%d,%d,%.2f,%.2f\r\n",_5CycleStop2,_NCycle,g_tAnglePID.limit, yaw);
    }

/*******************************************************************************
 * @brief   第一题
 * @return  
 * @note    
 *******************************************************************************/
void Topic1(void)
{
    gray_check();
    if(Write_Check())
    {
        WalkAngleBuff = yaw;
        RATurn_Flag = 1;
        _5CycleStop1=1;
    }

    else 
    {
        RATurn_Flag = 0;
    }

    switch(RATurn_Flag)
    {
        case 0:
        {
            if((_5CycleStop1 == 1)&&(gray_status==0))
            {
                _5CycleStop2++;
                _5CycleStop1 = 0;
            }

            else if(_5CycleStop2 == _NCycle)
            {
                g_eSysFlagManage = SYS_STA_STOP;
            }
            
            gray_check();
            pid_closeloop_track(0);
            pid_closeloop_motor(1.6-g_tTrackPID.out, 1.6+g_tTrackPID.out);
            car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
        }break;

        case 1:
        {
            pid_closeloop_angle(WalkAngleBuff + 90);
            pid_closeloop_motor(1.6-g_tAnglePID.out,1.6+g_tAnglePID.out);
            car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);

            /* ***角度归一化处理*** */
            // 计算两个角度之间的最小差值（考虑环形特性）
            float angle_diff = g_tAnglePID.target - g_tAnglePID.current;
		    // 将差值归一化到 [-180°, 180°]
		    if (angle_diff > 180) angle_diff -= 360;
		    if (angle_diff < -180) angle_diff += 360;
		    //成功转弯
		    if((angle_diff > -25) && (angle_diff <25 ))
		    {
                RATurn_Flag = 0;
                // _5CycleStop1++;
		    }
        }break;

    }


    printf("%d,%d,%.2f\r\n",RATurn_Flag,_5CycleStop1,yaw);
}


/*******************************************************************************
 * @brief   灰度测试
 * @return  
 * @note    
 *******************************************************************************/
 void Gray_Test(void)
 {
    gray_check();
    g_ChosFlag = Write_Check();

    sprintf((char *)oled_buffer4, "%d", gray_status);
    OLED_ShowString(6*11,0,oled_buffer4,8);

    // printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
    //     sensor_val[0],sensor_val[1],sensor_val[2],sensor_val[3],sensor_val[4],sensor_val[5],
    //     sensor_val[6],sensor_val[7],sensor_val[8],sensor_val[9],sensor_val[10],sensor_val[11]);
    printf("%d,%d,%d\r\n",gray_status,sensor_val[0],g_ChosFlag);

    if(Write_Check())
    {
        g_ChosFlag =1;
    }

 }


/*******************************************************************************
 * @brief   各种电机调参测试任务
 * @return  
 * @note    
 *******************************************************************************/
extern int now_right_counter;
extern int now_left_counter;
/*  
    if条件重定义
    0为左轮pid调参
    1为右轮pid调参
    2为角度pid调参
    3转向pid调参
    4距离pid调参
    6电机编码值打印
    5姿态打印
*/
#define NOW_CLOSELOOP_TARGET 3
void pid_closeloop_motor_task(void)
{
#if NOW_CLOSELOOP_TARGET == 0
    /*************************************左轮pid调参*****************************/
    /*5ms*/ 
    /*反馈值*/ 
    vofa_draw_graphical(PID_LEFT_MOTOR);
    /*PID运算*/ 
    pid_closeloop_motor(gLeftTargetBuff, gRightTargetBuff); //     /*PID输出值的执行*/ 
    car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);


#elif NOW_CLOSELOOP_TARGET == 1
    /*************************************右轮pid调参*****************************/
    /*5ms*/ 
    vofa_draw_graphical(PID_RIGHT_MOTOR);
    /*PID运算*/ 
   pid_closeloop_motor(gLeftTargetBuff, gRightTargetBuff);
    /*PID输出值的执行*/ 
    car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
#elif NOW_CLOSELOOP_TARGET == 2 
    /*************************************角度pid调参*****************************/
    /*5ms*/ 
    // vofa_draw_graphical(PID_ANGLE);
    printf("%.2f\r\n",g_tAnglePID.current);
    // TTL_Hex2Dec();
    g_tAnglePID.current = yaw;
    pid_closeloop_angle(gAngleTargetBuff);
    pid_closeloop_motor(-g_tAnglePID.out,+g_tAnglePID.out);
    car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
#elif NOW_CLOSELOOP_TARGET == 3     
    /*************************************转向pid调参*****************************/
    /* 5ms*/
    vofa_draw_graphical(PID_TRACK);
    // printf("%.2f,%.2f,%.2f,%.2f\r\n",left_Motor.n_rpm_Speed,right_Motor.n_rpm_Speed,g_tDistancePID.current,g_tTrackPID.out);
    gray_check();
    pid_closeloop_track(0);
    pid_closeloop_motor(gBaseSpeed-g_tTrackPID.out, gBaseSpeed+g_tTrackPID.out);
    car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
#elif NOW_CLOSELOOP_TARGET == 4
    /*************************************距离pid调参*****************************/
    /* 5ms*/
    vofa_draw_graphical(PID_DISTANCE);
    pid_closeloop_distance(gDistanceTargetBuff);
    // TTL_Hex2Dec();
    g_tAnglePID.current = yaw;
    pid_closeloop_angle(gAngleTargetBuff);
    // pid_closeloop_motor(g_tDistancePID.out,g_tDistancePID.out);
    pid_closeloop_motor(g_tDistancePID.out-g_tAnglePID.out,g_tDistancePID.out+g_tAnglePID.out);
    car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
#elif NOW_CLOSELOOP_TARGET == 5
    /*************************************陀螺仪测试*****************************/
    TTL_Hex2Dec();
    printf("%f\n\r",AHRSData_Packet.Heading*57.2958);
#elif NOW_CLOSELOOP_TARGET == 6
    /*************************************陀螺仪测试*****************************/
    printf("%d\r\n",now_right_counter);
    printf("%d\r\n",now_left_counter);
#endif 
}


/*******************************************************************************
 * @brief   选题（在key文件中
 * @return  
 * @note    
 *******************************************************************************/





/*******************************************************************************
 * @brief   直线巡线任务(距离+巡线)
 * @return  
 * @note    输入距离，自动用距离环和巡线环前进
 *******************************************************************************/
 void straight_task(float _distancetarget)
 {
    pid_closeloop_distance(_distancetarget);
    pid_closeloop_track(0);
    pid_closeloop_motor(g_tDistancePID.out-g_tTrackPID.out, g_tDistancePID.out+g_tTrackPID.out);
    car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
 }


/*******************************************************************************
 * @brief   转弯任务
 * @return  
 * @note    输入目标角度，包含获取当前角度和角度环运算
 *******************************************************************************/
 void turn_task(float _target)
 {
    // g_tAnglePID.current = yaw;
    pid_closeloop_angle(_target);
    pid_closeloop_motor(-g_tAnglePID.out,+g_tAnglePID.out);
    car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
 }

 /*******************************************************************************
 * @brief   停止任务
 * @return  
 * @note    
 *******************************************************************************/
 void stop_task(void)
 {
    pid_closeloop_motor(0,0);
    car_control(g_tLeftMotorPID.out, g_tRightMotorPID.out);
 }




/*******************************************************************************
 * @brief   vofa&编码器&xxx调试任务
 * @return  
 * @note    你可以在这里放任何你想调试的东西
 *******************************************************************************/
void uart_VofaEncoder_Test(void)
{

    // printf("%.2f\r\n", u.f);
    
    // OLED_ShowString(0,0,(uint8_t *)"aaa",8);
    // sprintf((char *)oled_buffer, "%d", g_eSysFlagManage);
    // OLED_ShowString(5*8,0,oled_buffer,16);
    
    
}

/*******************************************************************************
 * @brief   角度更新函数
 * @return  
 * @note    会实时更新并储存到g_tAnglePID.current
 *******************************************************************************/
void AngleCal_Task(void)
{

    Read_Quad();
    g_tAnglePID.current = yaw;

    
}

//云台
void error_sotp(void)
{
    delay_ms(1);
    send_enable_cmd(1,0);
    delay_ms(1);
    send_enable_cmd(2,0);
}

void enable(void)
{
    delay_ms(1);
    send_enable_cmd(1,1);
    delay_ms(1);
    send_enable_cmd(2,1);
}

void protect_time_out(void)
{
    if(uart_time_out == 1)
    {
        error_sotp();
        protect_flag = 1;
    }
    else if(uart_receive == 1 && protect_flag == 1)
    {
        enable();
        protect_flag = 0;
    }
}

void basics2(void)//基本问二
{
    DL_UART_Main_transmitDataBlocking(UART_k230_INST,1);
}

uint8_t basics3_mod = 1;
void basics3(void)//基本问三
{       
    Key_Process(&ChosStr_Key);
    Key_Process(&Confr_Key);
        switch (basics3_mod) 
    {
        case 1:
            if(ChosStr_Key.event_flag == 2)
            {
                origin_set(1);
                delay_ms(1);
                origin_set(2);
                delay_ms(1);
                Emm42_EN_init();
                basics3_mod = 2;
                ChosStr_Key.event_flag = 0;
            }
            if(Confr_Key.event_flag == 2)
            {
                Emm42_EN_init();
                delay_ms(2);
                origin_run(1);
                delay_ms(2);
                origin_run(2);
                basics3_mod = 2;
                DL_UART_Main_transmitDataBlocking(UART_k230_INST,1);
                Confr_Key.event_flag = 0;
            }
            break;
        case 2:

            break;
    }
}

void extra1(void)
{
    Walk();
    if(Write_Check())
    {
        switch(TurnCompltFlag)
        {
            case 0:
            {
                send_position_cmd(2, 1, 10, 0, 1100, 1);
                delay_ms(2);
            }break;

            case 1:
            {
                send_position_cmd(2, 1, 10, 0, 2800, 1);
                delay_ms(2);
            }break;

            case 2:
            {
                send_position_cmd(2, 1, 10, 0, 2800, 1);
                delay_ms(2);
            }break;

            case 3:
            {
                send_position_cmd(2, 1, 10, 0, 2800, 1);
                delay_ms(2);
            }break;
        }

    }

    else if(_5CycleStop2 == 1)
    {
        TurnCompltFlag = 1;
        send_position_cmd(2, 1, 12, 0, 1250, 1);
        // g_eSysFlagManage = SYS_STA_STOP;
        delay_ms(2);

    }   

    else if(_5CycleStop2 == 2)
    {
        TurnCompltFlag = 2;
        send_position_cmd(2, 1, 12, 0, 1250, 1);
        delay_ms(2);
    }

    else if(_5CycleStop2 == 3)
    {
        TurnCompltFlag = 4;
        send_position_cmd(2, 1, 12, 0, 1250, 1);
        delay_ms(2);
    }

    else if(_5CycleStop2 == 4)
    {
        TurnCompltFlag = 4;
        send_position_cmd(2, 1, 12, 0, 1250, 1);
        delay_ms(2);
    }

    printf("%d\r\n",TurnCompltFlag);

    // else if(_5CycleStop2 == 3)
    // {

    // }

    // else if(_5CycleStop2 == 4)
    // {

    // }
}