#include "hc_hal_motor.h"
#include "ti_msp_dl_config.h" // 包含实际的芯片底层SDK定义

// 这里定义特定于硬件的宏。如果其他地方定义了，这里可以借用，但最好在这里隔离。
// 根据之前的motor.c可知：
// 左轮PWM: Motor_PWM_INST, CC_0_INDEX
// 右轮PWM: Motor_PWM_INST, CC_1_INDEX
// 左轮方向: GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN1_PIN,
// GPIO_MOTOR_CONTROL_left_IN2_PIN 右轮方向: GPIO_MOTOR_CONTROL_PORT,
// GPIO_MOTOR_CONTROL_right_IN1_PIN, GPIO_MOTOR_CONTROL_right_IN2_PIN

void hc_hal_motor_set_dir(hc_motor_id_t motor_id, hc_motor_dir_t dir) {
  if (motor_id == HC_MOTOR_ID_LEFT) {
    if (dir == HC_MOTOR_DIR_FORWARD) {
      DL_GPIO_setPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN1_PIN);
      DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT,
                        GPIO_MOTOR_CONTROL_left_IN2_PIN);
    } else if (dir == HC_MOTOR_DIR_REVERSE) {
      DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT,
                        GPIO_MOTOR_CONTROL_left_IN1_PIN);
      DL_GPIO_setPins(GPIO_MOTOR_CONTROL_PORT, GPIO_MOTOR_CONTROL_left_IN2_PIN);
    } else { // STOP
      DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT,
                        GPIO_MOTOR_CONTROL_left_IN1_PIN);
      DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT,
                        GPIO_MOTOR_CONTROL_left_IN2_PIN);
    }
  } else if (motor_id == HC_MOTOR_ID_RIGHT) {
    if (dir == HC_MOTOR_DIR_FORWARD) {
      DL_GPIO_setPins(GPIO_MOTOR_CONTROL_PORT,
                      GPIO_MOTOR_CONTROL_right_IN1_PIN);
      DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT,
                        GPIO_MOTOR_CONTROL_right_IN2_PIN);
    } else if (dir == HC_MOTOR_DIR_REVERSE) {
      DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT,
                        GPIO_MOTOR_CONTROL_right_IN1_PIN);
      DL_GPIO_setPins(GPIO_MOTOR_CONTROL_PORT,
                      GPIO_MOTOR_CONTROL_right_IN2_PIN);
    } else { // STOP
      DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT,
                        GPIO_MOTOR_CONTROL_right_IN1_PIN);
      DL_GPIO_clearPins(GPIO_MOTOR_CONTROL_PORT,
                        GPIO_MOTOR_CONTROL_right_IN2_PIN);
    }
  }
}

void hc_hal_motor_set_pwm(hc_motor_id_t motor_id, uint32_t duty) {
  // duty 是倒置的：2000 -
  // 实际值，由于上层调用传递的是绝对值，这里负责处理底层逻辑
  if (motor_id == HC_MOTOR_ID_LEFT) {
    DL_TimerG_setCaptureCompareValue(Motor_PWM_INST, 2000 - duty,
                                     DL_TIMER_CC_0_INDEX);
  } else if (motor_id == HC_MOTOR_ID_RIGHT) {
    DL_TimerG_setCaptureCompareValue(Motor_PWM_INST, 2000 - duty,
                                     DL_TIMER_CC_1_INDEX);
  }
}

void hc_hal_motor_init(void) {
  // 这里可以放置 DL 相关的初始化，如果原来 motor_Init 是在这里做的，
  // 由于之前 motor.h 里声明了 motor_Init 和 motor_pwm0_Init，
  // 这里保留一个扩展口。具体实现在原始代码中如果调用了SYSCFG，这里可能为空。
}
