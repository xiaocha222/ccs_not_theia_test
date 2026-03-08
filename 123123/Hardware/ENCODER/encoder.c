#include "encoder.h"

int EncoderLeftBuff = 0;

// /******************************************************************
//  * @brief   编码器中断服务函数
//  * @return {*}
//  * @note    将你的各种初始化函数添加进这里
//  ******************************************************************/
//  void GROUP1_IRQHandler(void)
//  {
//     if(DL_GPIO_getRawInterruptStatus(GPIO_ENCODER_L_PORT, GPIO_ENCODER_L_LEFT_A_PIN) == GPIO_ENCODER_L_LEFT_A_PIN)
//     {
//         if( DL_GPIO_readPins(GPIO_ENCODER_L_PORT, GPIO_ENCODER_L_LEFT_B_PIN) == 0)
//         {
//             EncoderLeftBuff++;
//         }
//         else
//         {
//             EncoderLeftBuff--;
//         }
//     }
// }


// ***********************************//
// **
// **		运动控制变量
// **
// ****电机[运动参数+PID参数]变量：****//
// **左轮电机&&速度环PID**//
Motor left_Motor =	{
	.n_rpm_Speed = 0,      	//电机转速(每秒多少转)
	.v_Speed = 0,			//电机线速度(每秒多少毫米)
	.Alway_counter = 0,		//累加计数：计步
	.Time_counter = 0,		//周期计数：测速
};

Motor right_Motor = {
	.n_rpm_Speed = 0,      	//电机转速(每秒多少转)
	.v_Speed = 0,			//电机线速度(每秒多少毫米)
	.Alway_counter = 0,		//累加计数：计步
	.Time_counter = 0,		//周期计数：测速
};

//***********************************//
//**
//**		外部中断编码器计数函数
//**

/**
 * @brief GROUP1中断组的中断服务函数（GPIOA，GPIOB都在这个组，所有AB的中断都在这）——用于编码器的外部中断测速
 * @param None
 * @param None
 * @retval None
 * @note None[双相测速]
 */
void GROUP1_IRQHandler(void)
{
    //获取中断源
        //左轮A相——PA24,B相——PA25
    uint32_t left_flag_A = DL_GPIO_getEnabledInterruptStatus(GPIO_ENCODER_L_PORT, GPIO_ENCODER_L_LEFT_A_PIN);
    uint32_t left_flag_B = DL_GPIO_getEnabledInterruptStatus(GPIO_ENCODER_L_PORT, GPIO_ENCODER_L_LEFT_B_PIN);
        //右轮A相——PB18,B相——PA25
    uint32_t right_flag_A = DL_GPIO_getEnabledInterruptStatus(GPIO_ENCODER_R_RIGHT_A_PORT, GPIO_ENCODER_R_RIGHT_A_PIN);
    uint32_t right_flag_B = DL_GPIO_getEnabledInterruptStatus(GPIO_ENCODER_R_RIGHT_B_PORT, GPIO_ENCODER_R_RIGHT_B_PIN);

    // 左电机的【左电机是反过来的！，这里AB相判断是反过来的】
    // 左轮编码器A相触发
    if((left_flag_A & GPIO_ENCODER_L_LEFT_A_PIN) == GPIO_ENCODER_L_LEFT_A_PIN) 		// 判断是否为GPIOD_PIN引起的中断
    {
		//读取B相
        if (DL_GPIO_readPins(GPIO_ENCODER_L_PORT, GPIO_ENCODER_L_LEFT_B_PIN) == 0)      //负方向
        {
			left_Motor.direct = -1;
			left_Motor.Time_counter--;
        }
		else
        {
            left_Motor.direct = 1;
			left_Motor.Time_counter++;
        }
    }
    // 左轮编码器B相触发
    else if((left_flag_B & GPIO_ENCODER_L_LEFT_B_PIN) == GPIO_ENCODER_L_LEFT_B_PIN)
    {
        if(DL_GPIO_readPins(GPIO_ENCODER_L_PORT, GPIO_ENCODER_L_LEFT_A_PIN) == 0) 		//正方向 
        {
            left_Motor.direct = 1;
			left_Motor.Time_counter++;
        }
        else
        {
            left_Motor.direct = -1;
			left_Motor.Time_counter--;
        }
        //清空标志位
    }

    //清空标志位
    DL_GPIO_clearInterruptStatus(GPIO_ENCODER_L_PORT, GPIO_ENCODER_L_LEFT_A_PIN | GPIO_ENCODER_L_LEFT_B_PIN);

    // 右电机的
    // 右电机A相触发 
    if(right_flag_A & GPIO_ENCODER_R_RIGHT_A_PIN)
    {
        if (DL_GPIO_readPins(GPIO_ENCODER_R_RIGHT_B_PORT, GPIO_ENCODER_R_RIGHT_B_PIN) == 0) //（整体）负方向——若顺时针，A相在B相前（假如），若左轮让小车向前，则右轮让小车往后，故加负号aa
        {
            right_Motor.direct = -1;
			right_Motor.Time_counter--;
        }
		else
        {
			right_Motor.direct = 1;
			right_Motor.Time_counter++;
        }
    }
	// 右电机B相触发
    else if (right_flag_B & GPIO_ENCODER_R_RIGHT_B_PIN)                                       //（整体）正方向——画图知B相触发则与A相触发相反，故直接对上面加负号
    {
        if(DL_GPIO_readPins(GPIO_ENCODER_R_RIGHT_A_PORT, GPIO_ENCODER_R_RIGHT_A_PIN) == 0) //正方向
        {
			right_Motor.direct = 1;
			right_Motor.Time_counter++;
        }
		else
        {
			right_Motor.direct = -1;
			right_Motor.Time_counter--;
        }
        //清空标志位
        // DL_GPIO_clearInterruptStatus(GPIOA, DL_GPIO_PIN_25);
    }
    
        //清空标志位
    DL_GPIO_clearInterruptStatus(GPIO_ENCODER_R_RIGHT_A_PORT, GPIO_ENCODER_R_RIGHT_A_PIN);
    DL_GPIO_clearInterruptStatus(GPIO_ENCODER_R_RIGHT_B_PORT, GPIO_ENCODER_R_RIGHT_B_PIN);

}