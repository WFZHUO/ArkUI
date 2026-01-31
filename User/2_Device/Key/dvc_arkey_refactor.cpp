/**
 * @file dvc_arkey_refactor.cpp
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief Ark按键驱动. 模仿电脑鼠标按键逻辑,自带GPIO初始化配置
 * @version 1.0
 * @date 2026-01-24
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "dvc_arkey_refactor.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

//定义并初始化静态链表头指针
Class_ArkKey *Class_ArkKey::head = nullptr;
Class_ArkKey *Class_ArkKey::tail = nullptr;

/* Private function declarations ---------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 根据状态机产生用户事件标志(与ScanKeyState同周期调用)
 * @note 按下只是按下，你要怎么解释这个按下是应用层的事。
 *       要想真正实现双击或者长按，就要在应用层对按键的互动进行设计，若为长按/双击，则撤销单击效果。
 *
 */
void Class_ArkKey::UserApp()
{
    for (Class_ArkKey *key = head; key != nullptr; key = key->next)
    {
        // ---- 按下瞬间 ----
        if (key->state == ArkKey_State_DOWN)
        {
            key->isPressed = true;     // 立即按下事件
            key->holdTime = 0;         // holdTime清零并开始计holdTime

            // 如果是第一次点击的按下，则先判定为单击
            if (key->clickCount == 0)
            {
                key->clickState = 1;   // 先认为是单击
                key->clickCount = 1;   // 记录点击次数
            }
            // 如果已经是等待第二次点击，升级为双击
            else if (key->clickCount == 1 && key->intervalTime <= INTERVAL_THRESHOLD_MS)
            {
                key->isMultiClick = true;
                key->clickState = 2; // 双击
                key->clickCount = 0; // 双击后重置点击次数
            }
        }

        // ---- 持续按下 ----
        if (key->state == ArkKey_State_PRESSED)
        {
            key->holdTime += UPDATE_KEY_STATE_MS;// 持续按下时增加holdTime
            if (key->holdTime >= HOLD_THRESHOLD_MS)
            {
                key->isHold = true;  // 触发长按事件,如果一直按那就一直触发长按事件
                key->clickCount = 0; // 长按不参与多击
            }
        }

        // ---- 松手瞬间 ----
        if (key->state == ArkKey_State_UP)
        {
            key->isUp = true; // 设置释放标志
            // 如果是第一次点击的松手则等待可能的双击
            if (key->clickCount == 1)
            {
                key->intervalTime = 0; // intervalTime清零并开始计intervalTime
            }
        }

        // ---- 完全释放 ----
        if (key->state == ArkKey_State_RELEASED)
        {
            key->isReleased = true; // 设置持续释放标志
            if (key->clickCount > 0)
            {
                key->intervalTime += UPDATE_KEY_STATE_MS;// 持续释放时增加intervalTime

                // 超时后确认单击（已经发过单击标志，这里只是结束等待）
                if (key->intervalTime > INTERVAL_THRESHOLD_MS)
                {
                    key->clickCount = 0; // 连击等待结束
                    key->intervalTime = 0;//超时后此值无意义，不再代表Up1-Down2之间的时间，清零
                }
            }
        }
    }
}

/**
 * @brief 清除一次性事件标志(建议主循环读取后立即调用)
 * @note  在这里清除isHold和isReleased其实就是个摆设,因为下一周期它们还会true回来，但是本着要清就全清的原则，还是清了
 */
void Class_ArkKey::ClearFlags()
{
    isPressed = false;
    isHold = false;
    isMultiClick = false;
    isUp = false;
    isReleased = false;
    clickState = 0;
}

/**
 * @brief 初始化一个按键对象并加入按键链表
 *
 * @param GPIOX GPIO端口(如GPIOA/GPIOB)
 * @param GPIO_Pin_x GPIO引脚(如GPIO_Pin_1)
 */
void Class_ArkKey::Init(GPIO_TypeDef *GPIOX, uint16_t GPIO_Pin_x)
{
    value = 1;
    preValue = 1;
    next = nullptr;
    holdTime = 0;
    intervalTime = 0;
    clickCount = 0;
    state = ArkKey_State_RELEASED;

    isPressed = false;
    isHold = false;
    isMultiClick = false;
    isUp = false;
    isReleased = false;
    clickState = 0;

    gpio = GPIOX;
    pin = GPIO_Pin_x;

    // 加入静态链表
    if (head == nullptr)
    {
        head = this;
        tail = this;
    }
    else
    {
        tail->next = this;
        tail = this;
    }

    // GPIO初始化(保持原逻辑: 仅支持A/B口, 上拉输入)
    if (GPIOX == GPIOA)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    else if (GPIOX == GPIOB)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}

/**
 * @brief 防抖滤波
 * @note  使用时间采样法滤去高频抖动,仅能滤除抖动持续时间小于UPDATE_KEY_STATE_MS的抖动(顶多按键反馈会延时UPDATE_KEY_STATE_MS),
 *        如果抖动时间大于UPDATE_KEY_STATE_MS,则有概率触发多次按下等情况
 *        且连续主观按下间隔要大于2*UPDATE_KEY_STATE_MS(这个值不是严密推导得出的),否则会漏变化沿导致后续逻辑错误，应该没有超人般手速的人吧...
 */
void Class_ArkKey::DebounceFilter()
{
    for (Class_ArkKey *key = head; key != nullptr; key = key->next)
    {
        key->preValue = key->value;
        key->value = GPIO_ReadInputDataBit(key->gpio, key->pin);
    }
}

/**
 * @brief 扫描所有按键GPIO并刷新状态机(放在定时器中周期调用),需保证前后两次调用此函数的间隔为UPDATE_KEY_STATE_MS
 *
 */
void Class_ArkKey::ScanKeyState()
{
    DebounceFilter();

    for (Class_ArkKey *key = head; key != nullptr; key = key->next)
    {
        // 状态迁移判断(只负责“读GPIO -> 更新state”，不在这里维护计时器)
        if (key->preValue > key->value)       // 下降沿(按下)
            key->state = ArkKey_State_DOWN;
        else if (key->preValue < key->value)  // 上升沿(释放)
            key->state = ArkKey_State_UP;
        else if (key->value == 0)             // 持续低电平
            key->state = ArkKey_State_PRESSED;
        else                                  // 持续高电平
            key->state = ArkKey_State_RELEASED;
    }
}

/**
 * @brief TIM定时器中断检测并更新按键
 *
 */
void Class_ArkKey::TIM_Update_PeriodElapsedCallback()
{
    ScanKeyState();
    UserApp();
}

/*
 * @brief 清除所有按键的一次性事件标志
 *
 */
void Class_ArkKey::ClearAllFlags()
{
    for (Class_ArkKey *key = head; key != nullptr; key = key->next)
    {
        key->ClearFlags();
    }
}


/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
