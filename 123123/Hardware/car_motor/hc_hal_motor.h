#ifndef __HC_HAL_MOTOR_H__
#define __HC_HAL_MOTOR_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 电机方向枚举
 */
typedef enum {
    HC_MOTOR_DIR_STOP = 0,
    HC_MOTOR_DIR_FORWARD,
    HC_MOTOR_DIR_REVERSE
} hc_motor_dir_t;

/**
 * @brief 电机ID枚举
 */
typedef enum {
    HC_MOTOR_ID_LEFT = 0,
    HC_MOTOR_ID_RIGHT
} hc_motor_id_t;

/**
 * @brief 设置电机方向
 * @param motor_id 电机ID (HC_MOTOR_ID_LEFT 或 HC_MOTOR_ID_RIGHT)
 * @param dir 电机方向 (HC_MOTOR_DIR_STOP, HC_MOTOR_DIR_FORWARD, HC_MOTOR_DIR_REVERSE)
 */
void hc_hal_motor_set_dir(hc_motor_id_t motor_id, hc_motor_dir_t dir);

/**
 * @brief 设置电机PWM占空比
 * @param motor_id 电机ID (HC_MOTOR_ID_LEFT 或 HC_MOTOR_ID_RIGHT)
 * @param duty PWM占空比值
 */
void hc_hal_motor_set_pwm(hc_motor_id_t motor_id, uint32_t duty);

/**
 * @brief 初始化电机相关引脚和PWM (如果有需要，可以在这里封装底层的 DL_xxx_init)
 */
void hc_hal_motor_init(void);

#endif // __HC_HAL_MOTOR_H__
