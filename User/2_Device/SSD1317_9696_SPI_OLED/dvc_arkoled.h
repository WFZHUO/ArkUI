/**
 * @file dvc_arkoled.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief Ark OLED 驱动(软件SPI/显存缓冲). 兼容 C/C++ 工程.
 * @version 1.0
 * @date 2026-01-25
 * @copyright Wang Fangzhuo (c) 2026
 */

#ifndef DVC_ARKOLED_H
#define DVC_ARKOLED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "dvc_oled_data.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

/* Exported macros -----------------------------------------------------------*/

/*FontSize参数取值*/
/*此参数值不仅用于判断，而且用于计算横向字符偏移，默认值为字体像素宽度*/
#define OLED_8X16				(8)
#define OLED_6X8				(6)

/*IsFilled参数数值*/
#define OLED_UNFILLED			(0)
#define OLED_FILLED				(1)

/* 屏幕尺寸参数 */
#define OLED_WIDTH      (96)
#define OLED_HEIGHT     (96)
#define OLED_PAGES      (OLED_HEIGHT / 8)  // 12页

/* Exported types ------------------------------------------------------------*/
/*
NORMAL模式
在NORMAL模式下（oled_draw_mode = OLED_DRAWMODE_NORMAL），函数行为如下：
传入operation=1：像素被设置为白色。
传入operation=0：像素被设置为黑色。
XOR模式
在XOR模式下（oled_draw_mode = OLED_DRAWMODE_XOR），函数行为如下：
传入operation=1：如果当前像素颜色为白色，则设置为黑色；如果当前像素颜色为黑色，则设置为白色。
传入operation=0：像素被设置为黑色。
*/
typedef enum {
    OLED_DRAWMODE_NORMAL = 0,
    OLED_DRAWMODE_XOR       
} OLED_DrawMode;

/* Exported function declarations --------------------------------------------*/

//  初始化函数   /* 底层 OLED_WriteCommand  OLED_Clear */
void OLED_Init(void);
//  命令函数
void OLED_InvertDisplay(uint8_t invert);

//  光标更新函数    /* 底层 OLED_WriteCommand */
//void OLED_SetCursor(uint8_t Page, uint8_t X) 

//  更新函数   /* 底层 OLED_SetCursor  OLED_WriteData  OLED_DisplayBuf */
void OLED_Update(void);
void OLED_UpdateArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

//  显存控制函数  /* 底层 OLED_DisplayBuf */
void OLED_Clear(void);
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);
void OLED_Reverse(void);
void OLED_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height);

//设置显示模式函数
void OLED_SetDrawMode(OLED_DrawMode mode);

//  显示函数   /* 底层 OLED_ShowChar ---> OLED_DrawPoint ---> OLED_DisplayBuf */
void OLED_DrawPoint(int16_t x, int16_t y, uint8_t operation);
void OLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize);
void OLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize);
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize);
void OLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize);
void OLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...);
//------------------------------------------------------------------------------------------------------------------------
//  绘图函数   /* 底层 OLED_DrawPoint ---> OLED_DisplayBuf */
uint8_t OLED_GetPoint(int16_t X, int16_t Y);
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, uint8_t IncludeEndpoints, uint8_t operation);
void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled,uint8_t operation);
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled,uint8_t operation);
/*
对于画圆相关的函数，我的 4 条硬性要求
NORMAL 和 XOR 模式有不同表现（NORMAL 高效，XOR 不会缺边、不重复翻转）。
XOR 模式下不会对同一像素多次操作 → 不会出现断裂或“缺块”。
弧线圆润闭合，不会“四四方方”。
效率尽量高（NORMAL 走 Bresenham/中点法，XOR 下扫描线+边界补画）。
*/
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled,uint8_t operation);
void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled,uint8_t operation);
void OLED_DrawArc(int16_t X, int16_t Y, uint8_t Radius, int16_t StartAngle, int16_t EndAngle, uint8_t IsFilled,unsigned char operation);
void OLED_DrawRoundRect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint8_t radius, uint8_t isFilled,uint8_t operation);
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image,unsigned char IsMix);/*  OLED_DisplayBuf */


#ifdef __cplusplus
} // extern "C"
#endif

#endif

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
