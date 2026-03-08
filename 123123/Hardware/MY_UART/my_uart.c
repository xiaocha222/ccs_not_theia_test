#include "my_uart.h"

//串口数据结构体定义
struct uart_data BT_UART_data = {0};	    //uart1：蓝牙

UartPacketParser Emm42;

#define PKT_HEAD_Emm42          0xFF // 帧头
#define PKT_TAIL_Emm42          0xFE // 帧尾
#define PKT_LEN_Emm42           14   // 长度
#define PKT_LEN_Distance        6    // 长度


// 添加全局标志和浮点数变量

float Emm42_Data[3];
static uint8_t Emm42_Data_buffer[14] = {0};


//printf捆绑的串口地址:
UART_Regs * printf_uartbase = UART_Debugger_INST;

/*
 * @brief 串口printf重定义函数：【目前定义给了蓝牙串口】
 * @param None
 * @retval None
 * @note 在CCS中，M0，三个都要定义！！这里默认给到蓝牙-UART1
 */
int fputc(int c, FILE* stream)
{
	DL_UART_Main_transmitDataBlocking(printf_uartbase, c);
    return c;
}
 
int fputs(const char* restrict s, FILE* restrict stream)
{
    uint16_t i, len;
    len = strlen(s);
    for(i=0; i<len; i++)
    {
        DL_UART_Main_transmitDataBlocking(printf_uartbase, s[i]);
    }
    return len;
}
 
int puts(const char *_ptr)
{
        int count = fputs(_ptr,stdout);
        count +=fputs("\n",stdout);
        return count;
}

uint16_t X_Target;//X轴目标值
uint16_t Y_Target;//Y轴目标值
uint16_t X_Current;//X轴当前值
uint16_t Y_Current;//Y轴当前值
U_f32_2_u8 m;
void ProcessPacket_XY_data(void)
{
        m.c[0] = Emm42_Data_buffer[1];
        m.c[1] = Emm42_Data_buffer[2];
        m.c[2] = Emm42_Data_buffer[3];
        m.c[3] = Emm42_Data_buffer[4];
        m.c[4] = Emm42_Data_buffer[5];
        m.c[5] = Emm42_Data_buffer[6];
        m.c[6] = Emm42_Data_buffer[7];
        m.c[7] = Emm42_Data_buffer[8];
        if(m.n[0] < 0 || m.n[0] > 400|| m.n[1] < 0 || m.n[1]>240 || m.n[2] < 0 || m.n[2] > 400 || m.n[3] < 0 || m.n[3] > 240)
        {
            X_Target = Y_Target = X_Current = Y_Current = 0;
        } 
        else
        {
        X_Target = m.n[0];
        Y_Target = m.n[1];
        X_Current = m.n[2];
        Y_Current = m.n[3];
        }
}

/**
 * @brief Debugger调试器串口接收中断函数（UART0)
 * @param None
 * @retval None
 * @note 给到vofa
 **/
void UART_Debugger_INST_IRQHandler(void)
{
//只使能了接收中断，可不用判断中断源
    switch (DL_UART_Main_getPendingInterrupt(UART_Debugger_INST)) 
     {
        case DL_UART_MAIN_IIDX_RX://接收中断
        { 
            uint8_t DATA;
            DATA = DL_UART_Main_receiveData(UART_Debugger_INST);
            vofa_set_data(DATA);
         }break;
        default:
            break;
 }

}


/*
 * @brief 230中断函数
 * @param None
 * @retval None
 * @note 注意！！！这里只使能了接收中断，可不用判断中断源
 */
void UART_k230_INST_IRQHandler(void)
{   
    switch (DL_UART_Main_getPendingInterrupt(UART_k230_INST)) 
    {
        case DL_UART_MAIN_IIDX_RX:  //接收中断
        {   
            uart_time = 0;
            _find_target_flag = 1;
            if(uart_time_out == 1)
            {
                uart_receive = 1;
                uart_time_out_cnt = 0;
                uart_time_out = 0;
            }


            Emm42.RxBuff[0] = DL_UART_Main_receiveData(UART_k230_INST);
            // 情况1：等待帧头（检测帧头）
            if (!Emm42.isReceiving && (Emm42.RxBuff[0] == PKT_HEAD_Emm42)) 
            {
                Emm42.isReceiving = 1;    // 开始接收数据
                Emm42.index = 0;         // 重置数据长度
                Emm42.DataBuff[Emm42.index++] = Emm42.RxBuff[0];  // 存储帧头
            }
            // 情况2：正在接收数据
            else if (Emm42.isReceiving)
            {
                // 存储数据（防止溢出）
                if (Emm42.index < MAX_DATA_LEN)
                {
                    Emm42.DataBuff[Emm42.index++] = Emm42.RxBuff[0];
                }
                else
                {
                    // 缓冲区溢出，复位状态
                    Emm42.isReceiving = 0;
                    Emm42.index = 0;
                }
                // 检查帧尾
                if (Emm42.RxBuff[0] == PKT_TAIL_Emm42)
                {
                    memcpy(Emm42_Data_buffer, Emm42.DataBuff, PKT_LEN_Emm42);
                    Emm42.isReceiving = 0;
                }
            }
            Emm42.RxBuff[0] = 0;     
        } break;
        default:
        break;
    }
}



// /**
//   * 函    数：串口发送一个字节
//   * 参    数：Byte 要发送的一个字节
//   * 返 回 值：无
//   */
// void Serial_SendByte(uint8_t Byte)
// {
// 	DL_UART_Main_transmitDataBlocking(UART_wit9011_INST ,Byte);	//将字节数据写入数据寄存器，写入后USART自动生成时序波形
// }


// /**
//   * 函    数：串口发送一个数组
//   * 参    数：Array 要发送数组的首地址
//   * 参    数：Length 要发送数组的长度
//   * 返 回 值：无
//   */
// void Serial_SendArray(uint8_t *Array, uint16_t Length)
// {
// 	uint16_t i;
// 	for (i = 0; i < Length; i ++)		//遍历数组
// 	{
// 		Serial_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
// 	}
// }

// void SensorUartSend(uint8_t *p_data, uint32_t uiSiz)
// {
//     Serial_SendArray(p_data,uiSiz);
// }