/**
 * @file delay.c
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief 阻塞式延时库(SysTick). 适用于裸机/简单任务, 不依赖RTOS.
 * @version 1.0
 * @date 2026-01-25
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "delay.h"

/* Private macros ------------------------------------------------------------*/

#define SYSTICK_LOAD_MAX      (0x00FFFFFFu)      // SysTick->LOAD 24bit
#define US_PER_SEC            (1000000u)
#define MS_PER_SEC            (1000u)

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

//每微秒需要的时钟tick数(按HCLK计). 默认按 72MHz.
static uint32_t s_ticks_per_us = (DELAY_DEFAULT_HCLK_HZ / US_PER_SEC);

/* Private function declarations ---------------------------------------------*/
static void Delay_UsRaw(uint32_t us);

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化延时库(配置HCLK频率)
 * @param hclk_hz 当前HCLK频率(Hz), 例如 72000000
 */
void Delay_Init(uint32_t hclk_hz)
{
    if (hclk_hz == 0)
    {
        // 防御: 保持默认值
        s_ticks_per_us = (DELAY_DEFAULT_HCLK_HZ / US_PER_SEC);
        return;
    }

    s_ticks_per_us = hclk_hz / US_PER_SEC;

    // 防御: 极低频导致除完为0时, 至少为1
    if (s_ticks_per_us == 0)
    {
        s_ticks_per_us = 1;
    }
}

/**
 * @brief 微秒级延时(阻塞)
 * @param us 延时时长(微秒)
 *
 * @note 为兼容不同主频与超长延时, 本函数会自动分段.
 */
void Delay_us(uint32_t us)
{
    if (us == 0)
    {
        return;
    }

    // 计算单次最大可延时 us: LOAD = ticks_per_us * us <= 0xFFFFFF
    // 这里不做 -1 修正, 与旧实现保持一致的数量级/习惯.
    const uint32_t max_us_per_round = (s_ticks_per_us == 0) ? 0 : (SYSTICK_LOAD_MAX / s_ticks_per_us);

    if (max_us_per_round == 0)
    {
        // 极端情况下 ticks_per_us 非法, 退回默认72MHz
        s_ticks_per_us = (DELAY_DEFAULT_HCLK_HZ / US_PER_SEC);
        return;
    }

    while (us > 0)
    {
        uint32_t chunk = (us > max_us_per_round) ? max_us_per_round : us;
        Delay_UsRaw(chunk);
        us -= chunk;
    }
}

/**
 * @brief 毫秒级延时(阻塞)
 * @param ms 延时时长(毫秒)
 */
void Delay_ms(uint32_t ms)
{
    while (ms--)
    {
        Delay_us(1000u);
    }
}

/**
 * @brief 秒级延时(阻塞)
 * @param s 延时时长(秒)
 */
void Delay_s(uint32_t s)
{
    while (s--)
    {
        Delay_ms(MS_PER_SEC);
    }
}

/* Private function definitions ---------------------------------------------*/

/**
 * @brief 单次 SysTick 微秒延时(不分段)
 * @param us 延时时长(微秒), 必须满足 ticks_per_us * us <= 0xFFFFFF
 */
static void Delay_UsRaw(uint32_t us)
{
    // 装载计数值
    SysTick->LOAD = (s_ticks_per_us * us);
    SysTick->VAL  = 0u;

    // 选择HCLK作为时钟源, 关闭中断, 使能计数
    // CTRL bit2: CLKSOURCE=1(HCLK) ; bit1: TICKINT=0 ; bit0: ENABLE=1
    SysTick->CTRL = 0x00000005u;

    // 等待计数到0 (COUNTFLAG bit16 置位)
    while ((SysTick->CTRL & 0x00010000u) == 0u)
    {
        // busy wait
    }

    // 关闭计数器(保留CLKSOURCE=1)
    SysTick->CTRL = 0x00000004u;
}

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
