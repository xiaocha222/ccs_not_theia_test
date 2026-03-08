#include "system.h"

/* =============================状态机============================= */

/* 状态机标志位 */
SYS_FLAG_STA_E g_eSysFlagManage;  // 系统标志位管理(在不同状态下任务中去改)


void DebugTask(void){}//声明一下，方便后面举状态机"debug状态"的例子


/******************************************************************
 * @brief   任务汇总函数
 * @return {*}
 * @note    定义成员类型为TaskComps_T的数组，数组成员即任务
 ******************************************************************/
static TaskComps_T g_tTaskComps[] =
{
    // {SYS_STA_DEBUG, 0, 6, 6, pid_closeloop_motor_task},   //速度环调参业务(debug模式)
    // {SYS_STA_DEBUG, 0, 6, 6, pid_closeloop_motor_task},
    // {SYS_STA_DEBUG, 0, 50, 50, uart_VofaEncoder_Test},
    // {SYS_STA_DEBUG, 0, 6, 6, Topic1},
    // {SYS_STA_DEBUG, 0, 20, 20, AngleCal_Task},
    // {SYS_STA_DEBUG, 0, 20, 20, Gray_Test},

    // {SYS_STA_DEBUG, 0, 20, 20, Topic1},                   //基础部分第一题
    // {SYS_STA_DEBUG, 0, 20, 20, AngleCal_Task},            //角度运算

    // {SYS_STA_DEBUG, 0, 6, 6, Choose_Topic},              //选题模式测试
    // {SYS_STA_DEBUG, 0, 5, 5, KeyCount_Topic},            //按键获取

    // {SYS_STA_CHOOSE, 0, 5, 5, Choose_Topic},
    {SYS_STA_CHOOSE, 0, 5, 5, KeyCount_Topic},
    // {SYS_STA_CHOOSE, 0, 1, 1, AngleCal_Task},
    {SYS_STA_CHOOSE, 0, 350, 350, Menu_RefreshDisplay},
    {SYS_STA_CHOOSE, 0, 30, 30, Menu_ProcessInput},
    
    
    // {DEBUG_Q1, 0, 20, 20, Topic1_Debug},
    {DEBUG_Q2, 0, 20, 20, Walk},
    {DEBUG_Q2, 0, 5, 5, AngleCal_Task},

    {DEBUG_Q3, 0, 5, 5, uart_VofaEncoder_Test},

    {BASIC_Q1, 0, 5, 5, Walk_Basic},
    {BASIC_Q1, 0, 5, 5, AngleCal_Task},

    {BASIC_Q2, 0, 50, 20, basics2}, 

    {BASIC_Q3, 0, 50, 20, basics3}, 
    {BASIC_Q3, 0, 5, 5, KeyCount_Topic},

    {EXTRA_Q1, 0, 10, 10, extra1},

    // {BASIC, 0, 20, 20, AngleCal_Task},Extra1
    // {BASIC, 0, 20, 20, Topic1},
    // {BASIC, 0, 20, 20, KeyCount_Topic},
    {SYS_STA_STOP, 0, 30, 30, stop_task},
    
       

    

        // 如果需要添加其他任务，可以继续添加

};



/* =============================系统函数============================= */
static void TaskStartSchedule(void);//详细定义前使用了，提前声明一下，后面有详细定义
/******************************************************************
 * @brief   系统初始化函数
 * @return {*}
 * @note    将你的各种初始化函数添加进这里
 ******************************************************************/
void SysInit(void)
{
    /* 系统参数初始化(状态机、各种标志位) */
	g_eSysFlagManage = SYS_STA_INIT;

    /* 系统硬件初始化(各种外设) */
    //// m0有的 ////

    //基本初始化
    SYSCFG_DL_init();

    mspm0_delay_ms(10);      //6050上电启动比m0慢，延时1ms等待6050搞定后再对它初始化

    //开启定时器
    DL_TimerG_startCounter(Motor_PWM_INST);

    //开启串口中断
    NVIC_ClearPendingIRQ(UART_Debugger_INST_INT_IRQN);      //调试器串口
   // NVIC_ClearPendingIRQ(UART_k230_INST_INT_IRQN);     //蓝牙串口
    NVIC_ClearPendingIRQ(UART_StepperCMD_INST_INT_IRQN);         //其他串口
    NVIC_EnableIRQ(UART_Debugger_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_k230_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_StepperCMD_INST_INT_IRQN);

    //开启编码器中断
    NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);


    //// 自己编写的 ////
    /* 应用程序初始化(PID、模块、……) */
    //mpu6050初始化
    MPU6050_Init();

    //OLED初始化
    OLED_Init();

    //按键初始化
    Key_Init(&ChosStr_Key,GPIO_CHOOSETOPIC_PORT,GPIO_CHOOSETOPIC_Confirm_PIN);
    Key_Init(&Confr_Key,GPIO_CHOOSETOPIC_PORT,GPIO_CHOOSETOPIC_ChooseStart_PIN);
    
    //pid参数初始化
    pid_Init();

    error_sotp();

}

/******************************************************************
 * @brief   运行函数
 * @return {*}
 * @note    开始任务调度，在主函数中根据时间切片执行各种功能函数
 ******************************************************************/
void SysRun(void)
{
	// 切换状态机，进入xxx状态
	// g_eSysFlagManage = SYS_STA_DEBUG;
    g_eSysFlagManage = SYS_STA_CHOOSE;
    // g_eSysFlagManage = BASIC_Q2;
	
	// 开启任务调度管理
	TaskStartSchedule();
}

/******************************************************************
 * @brief   任务调度函数
 * @return {*}
 * @note    遍历并判断所有任务，通过就执行
 ******************************************************************/
static void TaskStartSchedule(void)
{
    while(1)
    {
        // 遍历所有任务
        for (uint8_t i = 0; i < sizeof(g_tTaskComps) / sizeof(g_tTaskComps[0]); i++)
        {
            // 如果任务需要执行
            if (g_tTaskComps[i].Run)
            {
                // 执行任务
                g_tTaskComps[i].pTaskFunc();
                // 执行完任务后，将任务标记为不可运行
                g_tTaskComps[i].Run = 0;
            }
        }

    }
}

/******************************************************************
 * @brief   时间切片函数
 * @return {*}
 * @note    任务时间片管理(1us)
 ******************************************************************/
void TaskTimeSliceManage(void)
{
    // 遍历所有任务
    for (uint8_t i = 0; i < sizeof(g_tTaskComps) / sizeof(g_tTaskComps[0]); i++)
    {
        // 状态系统状态机是属于当前任务的（如果不是就相当于暂停了） && 如果任务的计时器不为0
        if (g_tTaskComps[i].SysStaID == g_eSysFlagManage && g_tTaskComps[i].TimCount)
        {
            // 计时器减1
            g_tTaskComps[i].TimCount--;
            // 如果计时器为0
            if (g_tTaskComps[i].TimCount == 0)
            {
                // 计时器重新加载
                g_tTaskComps[i].TimCount = g_tTaskComps[i].TimRload;
                // 设置任务为可运行
                g_tTaskComps[i].Run = 1;
            }
        }
    }
}
