/**
 * @file arkui_animation.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief ArkUI动画库
 * @version 1.0
 * @date 2026-1-26
 * @copyright Wang Fangzhuo (c) 2026
 */

#ifndef ARKUI_ANIMATION_H
#define ARKUI_ANIMATION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "arkui_config.h"
#include "arkui.h"

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported function declarations --------------------------------------------*/

// 执行页面切换过渡动画
void ArkUITransitionAnim(void);

// 执行背景模糊效果
void ArkUIBackgroundBlur(void);

// 这个函数用于在屏幕上绘制一个消息框（弹窗）   msg: 要显示的消息内容
void ArkUIDrawMsgBox(const char *msg);

// 这个函数用于绘制一个进度条对话框，显示在屏幕中央。
void ArkUIDrawProgressBar(ArkUIItem_t *item);

// 画复选框，有选中非选中两种状态
void ArkUIDrawCheckbox(int16_t x, int16_t y, uint16_t size, uint8_t offset, bool boolValue, uint8_t r);

// 在图标四周画四个直角（L 型）作为角标
void ArkUI_DrawCornerBrackets(int16_t iconX, int16_t iconY,int16_t w, int16_t h, int16_t pad, int16_t cornerLen, int16_t cornerThk);

#ifdef __cplusplus
}
#endif

#endif

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
