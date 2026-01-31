/**
 * @file delay.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief 阻塞式延时库(SysTick). 适用于裸机/简单任务, 不依赖RTOS.
 * @version 1.0
 * @date 2026-01-25
 * @copyright Wang Fangzhuo (c) 2026
 *
 * @note
 * 1. 本库使用 SysTick 轮询方式实现延时(忙等), 会阻塞CPU.
 * 2. 默认按 72MHz(HCLK) 计算, 如主频不是72MHz, 请先调用 Delay_Init().
 * 3. 超长延时会被自动分段以满足 SysTick 24bit 装载限制.
 * 4. 中断环境不建议使用长延时(会拉长中断响应).
 */

#ifndef DELAY_H
#define DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "main.h"

/* Exported macros -----------------------------------------------------------*/

/**
 * @brief 默认HCLK(Hz). 若主频不是72MHz, 调用 Delay_Init(hclk_hz) 进行配置.
 */
#ifndef DELAY_DEFAULT_HCLK_HZ
#define DELAY_DEFAULT_HCLK_HZ (72000000u)
#endif

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported function declarations --------------------------------------------*/

/**
 * @brief 初始化延时库(配置HCLK频率)
 * @param hclk_hz 当前HCLK频率(Hz), 例如 72000000
 */
void Delay_Init(uint32_t hclk_hz);

/**
 * @brief 微秒级延时(阻塞)
 * @param us 延时时长(微秒)
 */
void Delay_us(uint32_t us);

/**
 * @brief 毫秒级延时(阻塞)
 * @param ms 延时时长(毫秒)
 */
void Delay_ms(uint32_t ms);

/**
 * @brief 秒级延时(阻塞)
 * @param s 延时时长(秒)
 */
void Delay_s(uint32_t s);

#ifdef __cplusplus
}
#endif

#endif

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
