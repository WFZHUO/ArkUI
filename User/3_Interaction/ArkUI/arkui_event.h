/**
 * @file arkui_event.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief 用于放置ArkUI的各种事件触发回调函数
 * @version 1.0
 * @date 2026-1-26
 * @copyright Wang Fangzhuo (c) 2026
 */

#ifndef ARKUI_EVENT_H
#define ARKUI_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "arkui_config.h"
#include "arkui.h"
#include "arkui_animation.h"

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported function declarations --------------------------------------------*/

void ArkUIEventChangeUint(ArkUIItem_t *item);
void ArkUIEventChangeUint_Cursor(ArkUIItem_t *item);
void ArkUIEventChangeInt(ArkUIItem_t *item);
void ArkUIEventChangeFloat(ArkUIItem_t *item);
void ArkFunctionExit(ArkUIItem_t *item);

/* -------- IconPageEvent---------- -------- */
extern volatile uint8_t g_iconNeedEntryAnim;// icon页面是否需要进入动画
void IconPageEvent(ArkUIPage_t *page);
void ArkUIEventEnterIconPage(ArkUIItem_t *item);

/* -------- MiniGame: Catch Brick -------- */
extern paramType gGame_FallSpeed;     // 砖块下落速度(px/帧)
extern paramType gGame_SpawnRate;     // 生成概率(0~1，每帧)
extern paramType gGame_PaddleWidth;   // 接板宽度(px)
extern paramType gGame_MaxBricks;     // 同屏最大砖块数(1~10)
void ArkUIEventCatchBrick(ArkUIItem_t *item);

#ifdef __cplusplus
}
#endif

#endif

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
