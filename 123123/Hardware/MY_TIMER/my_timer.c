#include "my_timer.h"

// 必须在指定时间执行的任务周期
#define _TIME_READSPEED 5       // 测速周期(ms)
#define MeasureTime 0.005
#define LeftStandardPulse 734.6
#define RightStandardPulse 731.8

// uint8_t _TIME_READSPEED = 5;       // 测速周期(ms)，小于255
// uint16_t LeftStandardPulse = 1059; //一圈标准脉冲，小于65535
// uint16_t RightStandardPulse = 1062;

/*************************************全局变量*****************************/
float distance;
uint8_t step_count_flag;
uint8_t beep_cnt = 100;
uint8_t beep_flag;
static void speedRead(void);
static void beep_light_gogo(void);
uint8_t g_KeyFlag = 0;

static uint8_t _uctime_readspeed = 0;
uint8_t uart_time = 0;
uint8_t uart_time_out = 0;
uint32_t uart_time_out_cnt;
uint8_t uart_receive;

/*******************************************************************************
 * @brief   滴答定时器中断函数(1ms)
 * @return  
 * @note    
 *******************************************************************************/
void SysTick_Handler(void)
{
    static uint8_t _uctime_readspeed = 0;

    if(_uctime_readspeed++ > _TIME_READSPEED) 
    {
        speedRead();
        _uctime_readspeed = 0;
    }

    tick_ms++;                                                                               /*******************************************************************************/

    // 裸机调度的时间片处理
    TaskTimeSliceManage();
        if(uart_time == 1)
    {
        uart_time_out_cnt ++;
        if(uart_time_out_cnt >= 500)
        {
            uart_time_out = 1;
            uart_receive = 0;
        }
    }

}


/*******************************************************************************
 * @brief   获取当前速度任务
 * @return  
 * @note    定时器中断中
 *******************************************************************************/
static void speedRead(void)
{
    // 获取单位时间内脉冲数【在外部中断中】
    // 获取累积脉冲数[2倍频]
        left_Motor.Alway_counter += left_Motor.Time_counter/2.0;
        right_Motor.Alway_counter += right_Motor.Time_counter/2.0;
        g_tDistancePID.current = (right_Motor.Alway_counter + left_Motor.Alway_counter) / 2; 

    //转速r/s版：
    //二倍频的 rpm  = 单位时间脉冲*1000*1s  / 13*2*30*测速周期s (r/s)
    left_Motor.n_rpm_Speed = ((float)left_Motor.Time_counter * 1000.0f) / (_TIME_READSPEED * 723.0f);	//左轮是反过来的，修改在编码器计数那边，AB相相反了
    right_Motor.n_rpm_Speed = ((float)right_Motor.Time_counter * 1000.0f) / (_TIME_READSPEED * 723.0f);
    
    // left_Motor.n_rpm_Speed = (float)left_Motor.Time_counter / LeftStandardPulse /MeasureTime;	//左轮是反过来的，修改在编码器计数那边，AB相相反了
    // right_Motor.n_rpm_Speed = (float)right_Motor.Time_counter / RightStandardPulse /MeasureTime;

    left_Motor.Time_counter = 0;
    right_Motor.Time_counter = 0;
}