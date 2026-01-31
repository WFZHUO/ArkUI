/**
 * @file arkui_config.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief ArkUI参数配置以及ArkUI调用到的API的配置,阻断ArkUI与底层驱动的耦合,方便使用其它底层驱动
 * @version 1.0
 * @date 2026-1-26
 * @copyright Wang Fangzhuo (c) 2026
 */

#ifndef ARKUI_CONFIG_H
#define ARKUI_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

//ArkUI调用到的API的头文件
#include "dvc_arkoled.h"
#include "dvc_arkey_refactor.h"
#include "delay.h"

/* Exported macros -----------------------------------------------------------*/

/**
 * @brief 屏幕硬件相关声明及UI界面设置（与硬件参数对齐）
 */
#define SCREEN_WIDTH            96  // 屏幕宽度(像素)
#define SCREEN_HEIGHT           96  // 屏幕高度(像素)
#define FONT_WIDTH              6  // 字体宽度(像素)
#define FONT_HEIGHT             8  // 字体高度(像素)
#define ITEM_HEIGHT             12  // 列表项高度(像素)
#define CHECK_BOX_OFFSET        2  // 复选框偏移量(像素)
#define SCROLL_BAR_WIDTH        4  // 滚动条宽度(像素)
#define ITEM_LINES              ((uint8_t)(SCREEN_HEIGHT / ITEM_HEIGHT))  // 屏幕可显示列表项行数
#define MAX_LAYER               10  // 界面最大层数
#define ICON_SIZE               50  // 图标尺寸(像素)
#define WELOCOME_PICTURE_HEIGHT 96  // 欢迎图片高度(像素)
#define WELOCOME_PICTURE_WIDTH  96  // 欢迎图片宽度(像素)

/**
 * @brief 界面动画时间设置
 */
#define INDICATOR_MOVE_TIME     100  // 指示器移动动画时间(ms)
#define ITEM_MOVE_TIME          100  // 列表项移动动画时间(ms)
#define TRANSITION_TIME         120  // 页面切换过渡动画时间(ms)

/**
 * @brief 屏幕操作调用相关API接口对齐
 */
/*------------------------------初始化与清屏，更新------------------------------*/
#define ArkUIScreenInit()                                      (OLED_Init())    // 初始化屏幕
#define ArkUIClearBuffer()                                     (OLED_Clear())   // 清空屏幕缓冲区
#define ArkUISendBuffer()                                      (OLED_Update())  // 更新到屏幕显示

/*------------------------------字符串、数值显示------------------------------*/
#define ArkUIDisplayStr(x, y, str)                             (OLED_ShowString(x, y, str, OLED_6X8)) // 显示字符串
#define ArkUIDisplayFloat(x, y, dat, num, pointNum)            (OLED_ShowFloatNum(x, y, dat, (num - pointNum), pointNum, OLED_6X8)) // 显示浮点数，num为总位数，pointNum为小数位数
#define ArkUIPrintf(x, y, format, ...)                         (OLED_Printf(x, y, OLED_6X8, format, ##__VA_ARGS__))

/*------------------------------点与图形绘制------------------------------*/
#define ArkUIDrawDot(x, y, color)                              (OLED_DrawPoint(x, y, color)) // 绘制点
#define ArkUIDrawBox(x, y, width, height, color)               (OLED_DrawRectangle(x, y, width, height, OLED_FILLED, color)) // 绘制实心矩形框
#define ArkUIDrawFrame(x, y, width, height, color)             (OLED_DrawRectangle(x, y, width, height, OLED_UNFILLED, color)) // 绘制矩形边框
#define ArkUIDrawRFrame(x, y, width, height, color, r)         (OLED_DrawRoundRect(x, y, width, height, r, OLED_UNFILLED, color)) // 绘制圆角矩形边框
#define ArkUIDrawRBox(x, y, width, height, color, r)           (OLED_DrawRoundRect(x, y, width, height, r, OLED_FILLED, color)) // 绘制圆角实心矩形框

/*------------------------------绘制模式 & 位图显示------------------------------*/
#define ArkUISetDrawColor(mode)                                (OLED_SetDrawMode((OLED_DrawMode)(mode))) //OLED_DRAWMODE_XOR,OLED_DRAWMODE_NORMAL
#define ArkUIDisplayBMP(x, y, w, h, pic)                       (OLED_ShowImage(x, y, w, h, pic,1))

/*------------------------------反色显示------------------------------*/
#define ArkUIModifyColor(x)                                     (OLED_InvertDisplay(x))    // 1开启反色,0关闭反色

/** 
 * @brief 延时函数相关API接口对齐
 */ 
#define ArkUIDelay_ms(time)                                    (Delay_ms(time))

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

extern Class_ArkKey keyUp, keyDown, keyConfirm;

/* Exported function declarations --------------------------------------------*/

void ArkUIKeyInit(void);


#ifdef __cplusplus
}
#endif

#endif

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
