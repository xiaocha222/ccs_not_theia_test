#ifndef __KEY_H__
#define __KEY_H__

#include "ti_msp_dl_config.h"
#include "system.h"

/**************按键获取******************* */
typedef enum {
    KEY_STATE_IDLE,      // 空闲状态
    KEY_STATE_DEBOUNCE,  // 消抖检测
    KEY_STATE_PRESSED    // 确认按下
} KeyState;

typedef struct {
    GPIO_Regs* port;     // GPIO端口（如GPIOA_PORT）
    uint32_t pin;        // 引脚（如GPIOB_PIN_21）
    KeyState state;      // 当前状态
    uint32_t tick;  // 状态计时
    uint8_t event_flag;  // 按键事件标志
} KeyHandler;

/**************选题******************* */
// 菜单状态定义
typedef enum {
    MENU_LEVEL1,    // 一级菜单
    MENU_DEBUG,     // 二级菜单-Debug
    MENU_BASIC,     // 二级菜单-Basic
    MENU_EXTRA,     // 二级菜单-Extra
    MENU_EXIT       // 退出状态
} MenuState_t;
// 全局状态变量
MenuState_t g_eMenuState;



typedef struct {
    const char *text;        // 菜单显示文本
    MenuState_t nextState;   // 确认后跳转的状态
    void (*action)(void);    // 关联的功能函数
} MenuItem_t;



// // Debug二级菜单
void DebugQ1_Handler();
void DebugQ2_Handler();
void DebugQ3_Handler();



// // Basic二级菜单（示例）
void BasicQ1_Handler();
void BasicQ2_Handler();



// // Extra二级菜单（示例）


//函数
void Debug_Q1(void);
void Debug_Q2(void);
void Debug_Q3(void);
void Basic_Q1(void);
void Basic_Q2(void);
void Basic_Q3(void);
void Extra_Q1(void);
void Extra_Q2(void);
void Extra_Q3(void);
void Menu_Back(void);

void Menu_ProcessInput(void);
const MenuItem_t* GetCurrentMenu(void);
void Menu_RefreshDisplay(void) ;
/**************选题******************* */
extern KeyHandler ChosStr_Key;
extern KeyHandler Confr_Key;


void Key_Init(KeyHandler* handler, GPIO_Regs* port, uint32_t pin);
void Key_Process(KeyHandler* handler);  // 需在1ms定时中断中调用
uint8_t Key_GetEvent(KeyHandler* handler);
void Key_ClearEvent(KeyHandler* handler);
void KeyCount_Topic(void);




//  void Menu_ProcessInput(void);
#endif