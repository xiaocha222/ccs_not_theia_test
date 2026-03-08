#include "key.h"

uint8_t Buff = 0;

/*============================================= 全局变量 ======================================*/
// 一级菜单项
const MenuItem_t Level1Menu[] = {
    {"Debug Mode",  MENU_DEBUG, NULL},
    {"Basic Mode",  MENU_BASIC, NULL},
    {"Extra Mode",  MENU_EXTRA, NULL},
    {NULL, MENU_LEVEL1, NULL}           //结束OLED标记
};

//二级菜单项        这里面的MENU_LEVEL1只有一级菜单项用过，虽然一二级菜单项都是以MenuItem_t定义的
//DEBUG
const MenuItem_t DebugMenu[] = {
    {"SetCycles", MENU_LEVEL1, Debug_Q1},
    {"Debug_Q2", MENU_LEVEL1, Debug_Q2},
    {"Debug_Q3", MENU_LEVEL1, Debug_Q3},
    {"Back",     MENU_LEVEL1, Menu_Back},
    {NULL, MENU_DEBUG, NULL}
};

//BASIC
const MenuItem_t BasicMenu[] = {
    {"Basic_Q1", MENU_LEVEL1, Basic_Q1},
    {"Basic_Q2", MENU_LEVEL1, Basic_Q2},
    {"Basic_Q3", MENU_LEVEL1, Basic_Q3},
    {"Back",     MENU_LEVEL1, Menu_Back},
    {NULL, MENU_BASIC, NULL}
};

//EXTRA
const MenuItem_t ExtraMenu[] = {
    {"Extra_Q1", MENU_LEVEL1, Extra_Q1},
    {"Extra_Q2", MENU_LEVEL1, Extra_Q2},
    {"Extra_Q3", MENU_LEVEL1, Extra_Q3},
    {"Back",     MENU_LEVEL1, Menu_Back},
    {NULL, MENU_EXTRA, NULL}
};

uint8_t g_CurrentSelection = 0;  // 当前选中项索引

/*============================================= 其他变量 ======================================*/

KeyHandler ChosStr_Key = {0};
KeyHandler Confr_Key = {0};

/*******************************************************************************
 * @brief   按键初始化
 * @return  
 * @note    传入按键结构体，和对应的引脚，函数将将他们绑定
 *******************************************************************************/
void Key_Init(KeyHandler* handler, GPIO_Regs* port, uint32_t pin) {
    handler->port = port;
    handler->pin = pin;
    handler->state = KEY_STATE_IDLE;
    handler->tick = 0;
    handler->event_flag = 0;
    
    // 配置引脚上拉（根据硬件连接调整）[5](@ref)
}

/*******************************************************************************
 * @brief   更新按键状态
 * @return  
 * @note    需配合按键计时使用
 *******************************************************************************/
void Key_Process(KeyHandler* handler) {
    uint8_t pin_state = (DL_GPIO_readPins(handler->port, handler->pin) != 0);

    switch (handler->state) {
        case KEY_STATE_IDLE:
            if (pin_state == 0) { // 按下为低电平[1](@ref)
                handler->tick = 0; // 记录时间戳
                handler->state = KEY_STATE_DEBOUNCE;
            }
            break;
        
        case KEY_STATE_DEBOUNCE:
            if (handler->tick >= 3) { // 15ms消抖
                if (pin_state == 0) { // 确认按下
                    handler->state = KEY_STATE_PRESSED;
                    handler->event_flag = 1; // 标记事件
                } else {
                    handler->state = KEY_STATE_IDLE; // 抖动干扰
                }
            }
            break;
        
        case KEY_STATE_PRESSED:
            if (pin_state != 0) { // 按键释放
                handler->event_flag = 2; // 标记事件;应用处理完后需要清0
                handler->tick = 0;
                handler->state = KEY_STATE_IDLE;
            }
            break;
    }

}

/*******************************************************************************
 * @brief   按键计时
 * @return  
 * @note    5ms进行一次，可完成15ms消抖
 *******************************************************************************/
void KeyCount_Topic(void)
 {
    if(ChosStr_Key.state == KEY_STATE_DEBOUNCE)
    {
        ChosStr_Key.tick++;
    }

    if(Confr_Key.state == KEY_STATE_DEBOUNCE)
    {
        Confr_Key.tick++;
    }
 }

 /*************************************************************************************************************************************/


 /*******************************************************************************
 * @brief   
 * @return  
 * @note    
 *******************************************************************************/
 void Menu_ProcessInput(void)
 {
    Key_Process(&ChosStr_Key);
    Key_Process(&Confr_Key);
    printf("%d\r\n",g_CurrentSelection);


    /********** 统计任务数 **********/
    const MenuItem_t *menu = GetCurrentMenu();
    uint8_t itemCount = 0;
    while(menu[itemCount].text != NULL) itemCount++;

    /********** 更新按键状态 **********/
    if(ChosStr_Key.event_flag == 2)
    {
        g_CurrentSelection ++;
        if(g_CurrentSelection>=itemCount)
            g_CurrentSelection = 0;

        ChosStr_Key.event_flag = 0;
    }

    else if(Confr_Key.event_flag == 2)
    {
        switch (g_eMenuState)
        {
            case MENU_LEVEL1:
            {
                g_eMenuState = Level1Menu[g_CurrentSelection].nextState;
                g_CurrentSelection = 0;  // 重置二级菜单选中项
            } break;

            case MENU_DEBUG:
            {

                OLED_Clear();
                // OLED_ShowNum(42,1,_NCycle,8,8);
                DebugMenu[g_CurrentSelection].action();
                g_CurrentSelection = 0;
            } break;

            case MENU_BASIC:
            {
                OLED_Clear();
                
                BasicMenu[g_CurrentSelection].action();
                g_CurrentSelection = 0;
            } break;

            case MENU_EXTRA:
            {
                OLED_Clear();
                
                ExtraMenu[g_CurrentSelection].action();
                g_CurrentSelection = 0;
            } break;

            case MENU_EXIT:
              break;

            default:break;
        }
        // OLED_Clear();
        // mspm0_delay_ms(1);
        Confr_Key.event_flag = 0;
            
    }


 }

  /*******************************************************************************
 * @brief   
 * @return  
 * @note    
 *******************************************************************************/
const MenuItem_t* GetCurrentMenu() {
    switch(g_eMenuState) {
        case MENU_DEBUG: return DebugMenu;
        case MENU_BASIC: return BasicMenu;
        case MENU_EXTRA: return ExtraMenu;
        default: return Level1Menu;
    }
}

  /*******************************************************************************
 * @brief   
 * @return  
 * @note    
 *******************************************************************************/
void Menu_RefreshDisplay() 
{
    if(g_eSysFlagManage == SYS_STA_CHOOSE)
    {
    
    const MenuItem_t *menu = GetCurrentMenu();
    
    // 显示标题
    switch(g_eMenuState) {
        case MENU_LEVEL1: OLED_ShowString(0,0,(uint8_t *)"MAIN_MENU",8); break;
        case MENU_DEBUG:  OLED_ShowString(0,0,(uint8_t *)"Debug",8); break;
        case MENU_BASIC:  OLED_ShowString(0,0,(uint8_t *)"Basic",8); break;
        case MENU_EXTRA:  OLED_ShowString(0,0,(uint8_t *)"Extra",8); break;
        default:break;
    }

    // 显示菜单项（带选中标记）
    for(int i=0; menu[i].text != NULL; i++) {
        if(i == g_CurrentSelection) 
        {
            OLED_ShowString(0, i+1, (uint8_t *)">", 8);
        }
        else if(i != g_CurrentSelection)
        {
            OLED_ShowString(0, i+1, (uint8_t *)" ", 8);
        }
        OLED_ShowString(6, i+1, (uint8_t *)menu[i].text, 8);
    }

    }

 }
 
 //Debug
 void Debug_Q1(void)
{
    Buff++;
    _NCycle = Buff*4;
    if(Buff >= 5)
        Buff = 0;
    OLED_ShowNum(42,1,_NCycle,8,8);
}
void Debug_Q2(void)
{
    g_eSysFlagManage = DEBUG_Q2;
}
void Debug_Q3(void)
{
    g_eSysFlagManage = DEBUG_Q3;
}

//Basic
void Basic_Q1(void)
{
    gBaseSpeed = 2.0;
    gBaseSpeed_Angle = 1.2;
    g_eSysFlagManage = BASIC_Q1;
}
void Basic_Q2(void)
{
    g_eSysFlagManage = BASIC_Q2;
}
void Basic_Q3(void)
{
    g_eSysFlagManage = BASIC_Q3;
}

//Extra
void Extra_Q1(void)
{
    Emm42_EN_init();
    g_eSysFlagManage = EXTRA_Q1;
}
void Extra_Q2(void)
{
    NVIC_ClearPendingIRQ(UART_k230_INST_INT_IRQN);
    g_eSysFlagManage = EXTRA_Q1;
}
void Extra_Q3(void)
{
    g_eSysFlagManage = EXTRA_Q1;
}

void Menu_Back(void)
{
    g_eMenuState = MENU_LEVEL1;
}