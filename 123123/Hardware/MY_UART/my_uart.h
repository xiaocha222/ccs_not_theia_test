#ifndef __MY_UART_H__
#define __MY_UART_H__

#include "system.h"

#define MIN_DATA_LEN 			3
#define MAX_DATA_LEN 			50 // 最大接收数据长度


//串口接收数据结构体
//串口接收数据结构体
struct uart_data
{
	char Rx_Data[100];			//接收的数据（数组）
	unsigned char Rx_Data_Buf;	//接收数据的缓冲区【先放buf，后放入数据数组】
	unsigned char Rx_Cnt;		//接收数据个数/索引

	char Tx_String_Buf[100];	//要发送的数据缓冲区
};

typedef union
{
    char c[8];
    uint16_t n[4];//XY目标和当前值
} U_f32_2_u8;


typedef struct
{
	uint8_t RxBuff[1]; // 数据缓冲区
	uint8_t DataBuff[MAX_DATA_LEN];
	uint16_t index;		   // 当前写入位置
	uint16_t expected_len; // 预期总长度（依赖类型）
	uint8_t isReceiving;
} UartPacketParser;

//串口数据
extern struct uart_data BT_UART_data;	    //uart1：蓝牙
extern U_f32_2_u8 m;
extern uint16_t X_Target;//X轴目标值
extern uint16_t Y_Target;//Y轴目标值
extern uint16_t X_Current;//X轴当前值
extern uint16_t Y_Current;//Y轴当前值
//***********************************//
//**
//**			  发送函数
//**
/**
 * @brief 自定义封装的发送字符串函数：
 * @param None
 * @retval None
 * @note 注意！通过这种方式可以让所有串口使用上printf(...)!!!
    1、可发送字符串，可发送\r\n
    ...

    如果想用可使用%d, %f...，参考下面:（因为printf发送出去也还是字符串！！！）
        char txbuf[...]
        sprintf(txbuf,"%d...",...);
        SendString(txbuf)

    txbuf在串口数据结构体定义了
 **/
void UART_SendString(UART_Regs * UART_Base_INST,char * str);

/**
 * @brief 串口printf重定义函数：
 * @param None
 * @retval None
 * @note 在CCS中，M0，三个都要定义！！这里默认给到蓝牙-UART1
 **/
int fputc(int c, FILE* stream);
 
int fputs(const char* restrict s, FILE* restrict stream);
 
int puts(const char *_ptr);

//***********************************//
//**
//**		串口中断响应函数
//**

//串口1——蓝牙
void UART_BT_INST_IRQHandler(void);

void UART_TRA_MV_INST_IRQHandler(void);

void UART_DIS_MV_INST_IRQHandler(void);

//***********************************//
//**
//**		其他函数
//**
void TTL_Hex2Dec(void);
void ProcessPacket_XY_data(void);

#endif
