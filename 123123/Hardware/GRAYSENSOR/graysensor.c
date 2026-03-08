#include "graysensor.h"
#define ALL_WHITE_STATE 0x0FFF

/*============================================= 全局变量 ======================================*/
uint8_t stop_way = 0;				//停止检测
uint8_t sensor_val[12];				//灰度返回值
uint16_t gray_state = 0x0000;			//当前灰度状态
int16_t gray_status;					//灰度量化后的值
int8_t gtay_IT;

/*============================================= 函数 ======================================*/
uint16_t Sensor_GetState(void)
{
	uint16_t State = 0X0000;
	sensor_val[11] = P12!= 0?0x01:0x0;
	sensor_val[10] = P11!= 0?0x01:0x0;
	sensor_val[9] = P10!= 0?0x01:0x00;
	sensor_val[8] = P9!= 0?0x01:0x0;
	sensor_val[7] = P8!= 0?0x01:0x00;
	sensor_val[6] = P7!= 0?0x01:0x0;
	sensor_val[5] = P6!= 0?0x01:0x0;
	sensor_val[4] = P5!= 0?0x01:0x0;
	sensor_val[3] = P4!= 0?0x01:0x0;
	sensor_val[2] = P3!= 0?0x01:0x0;
	sensor_val[1] = P2!= 0?0x01:0x0;
	sensor_val[0] = P1!= 0?0x01:0x0;
	
	State |= (sensor_val[0] << 0);
	State |= (sensor_val[1] << 1);
	State |= (sensor_val[2] << 2);
	State |= (sensor_val[3] << 3);
	State |= (sensor_val[4] << 4);
	State |= (sensor_val[5] << 5);
	State |= (sensor_val[6] << 6);
	State |= (sensor_val[7] << 7);
	State |= (sensor_val[8] << 8);
	State |= (sensor_val[9] << 9);
	State |= (sensor_val[10] << 10);
	State |= (sensor_val[11] << 11);
	
	
	return State;
	
}
/*******************************************************************************
 * @brief   当前巡线状态
 * @return  
 * @note    
 *******************************************************************************/
void gray_check(void)
{
	gray_state = Sensor_GetState();
	
	// if(enter_flag == 1)
    // {
        // gray_state = gray_state & 0x1f;											// 111110000000
		gray_state = gray_state;											// 111110000000
    // }

	switch(gray_state)
	{
		
		case 0x0001:gray_status=-11;   	   break;									// 000000000001b
		case 0x0003:gray_status=-10;   	   break;									// 000000000011b
	    case 0x0002:gray_status=-9;	  	   break;									// 000000000010b
		case 0x0006:gray_status=-8;	  	   break;									// 000000000110b
		case 0x0004:gray_status=-7;	  	   break;									// 000000000100b
		case 0x000C:gray_status=-6;	  	   break;									// 000000001100b
		case 0x0008:gray_status=-5;	  	   break;									// 000000001000b
		case 0x0018:gray_status=-4;	  	   break;									// 000000011000b
		case 0x0010:gray_status=-3;	  	   break;									// 000000010000b
		case 0x0030:gray_status=-2;	  	   break;									// 000000110000b
		case 0x0020:gray_status=-1;	  	   break;									// 000000100000b
		case 0x0060:gray_status=0;		   break;									// 000001100000b
		case 0x0040:gray_status=1;		   break;									// 000001000000b
		case 0x00C0:gray_status=2;		   break;									// 000011000000b
		case 0x0080:gray_status=3;		   break;									// 000010000000b
		case 0x0180:gray_status=4;		   break;									// 000110000000b
		case 0x0100:gray_status=5;		   break;									// 000100000000b
		case 0x0300:gray_status=6;		   break;									// 001100000000b
		case 0x0200:gray_status=7;		   break;									// 001000000000b
		case 0x0600:gray_status=8;		   break;									// 011000000000b
		case 0x0400:gray_status=9;		   break;									// 010000000000b
		case 0x0C00:gray_status=10;	  	   break;									// 110000000000b
		case 0x0800:gray_status=11;	  	   break;									// 100000000000b
	}
}

uint8_t  Write_Check(void)
{
	return (gray_state == 0x0000); // 12位全1的掩码是0x0FFF
}