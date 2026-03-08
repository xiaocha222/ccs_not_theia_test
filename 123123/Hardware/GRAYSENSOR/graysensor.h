#ifndef __TRACKING_H
#define __TRACKING_H
#include "system.h"

#define P1				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O1_PIN)
#define P2				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O2_PIN)
#define P3				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O3_PIN)
#define P4				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O4_PIN)
#define P5				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O5_PIN)
#define P6				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O6_PIN)
#define P7				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O7_PIN)
#define P8				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O8_PIN)
#define P9				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O9_PIN)
#define P10				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O10_PIN)
#define P11				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O11_PIN)
#define P12				DL_GPIO_readPins(GPIO_GRAYSENSOR_PORT, GPIO_GRAYSENSOR_O12_PIN)

#define P_LEFT                  P6 || P5 || P4 || P3 || P2 || P1
#define P_RIGHT                 P12|| P11|| P10|| P9 || P8 || P7 || P6 
#define P_IN                    P12|| P11|| P10|| P9 || P8 || P7 || P6 || P5 || P4 || P3 || P2 || P1
#define P_OUT                   P12&& P11&& P10&& P9 && P8 && P7 && P6 && P5 && P4 && P3 && P2 && P1

extern int16_t gray_status;
extern uint16_t gray_state;
extern uint8_t sensor_val[12];	

void gray_check(void);
uint8_t  Write_Check(void);
#endif
