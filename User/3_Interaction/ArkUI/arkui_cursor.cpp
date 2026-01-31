/**
 * @file arkui_cursor.cpp
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief ArkUI 通用光标管理器
 * @version 1.0
 * @date 2026-01-30
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "arkui_cursor.h"
#include "arkui.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static Class_ArkUI_Cursor g_ArkCursor;

/* Private function declarations ---------------------------------------------*/

/* Function definitions -------------------------------------------------------*/

/**
 * @brief 初始化光标
 */
void Class_ArkUI_Cursor::Init()
{
    Visible = 1;
    Active = 0;
    Radius = 1;
    Object = 0xFFFFFFFFu;
    Now_X = Now_Y = Now_W = Now_H = 0;
    Start_X = Start_Y = Start_W = Start_H = 0;
    Target_X = Target_Y = Target_W = Target_H = 0;
    T_ms = 0;
    Dur_ms = 1;
}

/**
 * @brief 限制 t 在 [0, 1] 范围内
 * @param t 输入值
 * @return float 限制后的结果
 */
inline float Class_ArkUI_Cursor::Clamp01(float t)
{
    if (t < 0.0f) return 0.0f;
    if (t > 1.0f) return 1.0f;
    return t;
}

/**
 * @brief 线性插值
 * @param a 起始值
 * @param b 目标值
 * @param t 插值参数（0~1）
 * @return float 插值结果
 */
inline float Class_ArkUI_Cursor::Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

/**
 * @brief 放置光标到某位置并不再移动
 * @param __x 矩形 x 坐标
 * @param __y 矩形 y 坐标
 * @param __w 矩形宽度
 * @param __h 矩形高度
 * @param __radius 圆角半径
 */
void Class_ArkUI_Cursor::Set_Now(float __x, float __y, float __w, float __h, uint8_t __radius)
{
    Radius = __radius;

    Now_X = __x; Now_Y = __y; Now_W = __w; Now_H = __h;

    // 同步起点/目标，停止动画
    Start_X = Now_X; Start_Y = Now_Y; Start_W = Now_W; Start_H = Now_H;
    Target_X = Now_X; Target_Y = Now_Y; Target_W = Now_W; Target_H = Now_H;

    T_ms = 0;
    Dur_ms = 1;
    Active = 0;
}

/**
 * @brief 设置当前光标从当前位置到某位置
 * @param __object 对象 ID
 * @param __tx 目标 x 坐标
 * @param __ty 目标 y 坐标
 * @param __tw 目标宽度
 * @param __th 目标高度
 * @param __dur_ms 动画持续时间（毫秒）
 * @param __radius 圆角半径
 */
void Class_ArkUI_Cursor::Goto(uint32_t __object, float __tx, float __ty, float __tw, float __th, uint16_t __dur_ms, uint8_t __radius)
{
    Radius = __radius;
    if (__dur_ms == 0) __dur_ms = 1;
    Object = __object;

    Start_X = Now_X; Start_Y = Now_Y; Start_W = Now_W; Start_H = Now_H;
    Target_X = __tx; Target_Y = __ty; Target_W = __tw; Target_H = __th;

    T_ms = 0;
    Dur_ms = __dur_ms;
    Active = 1;
}

/**
 * @brief 设置当前矩形从某位置到某位置
 * @param __object 对象 ID
 * @param __sx 起点 x 坐标
 * @param __sy 起点 y 坐标
 * @param __sw 起点宽度
 * @param __sh 起点高度
 * @param __tx 目标 x 坐标
 * @param __ty 目标 y 坐标
 * @param __tw 目标宽度
 * @param __th 目标高度
 * @param __dur_ms 动画持续时间（毫秒）
 * @param __radius 圆角半径
 */
void Class_ArkUI_Cursor::Goto_From(uint32_t __object,float __sx, float __sy, float __sw, float __sh,float __tx, float __ty, float __tw, float __th,uint16_t __dur_ms, uint8_t __radius)
{
    Radius = __radius;
    if (__dur_ms == 0) __dur_ms = 1;

    Object = __object;

    Start_X = __sx; Start_Y = __sy; Start_W = __sw; Start_H = __sh;
    Target_X = __tx; Target_Y = __ty; Target_W = __tw; Target_H = __th;

    Now_X = __sx; Now_Y = __sy; Now_W = __sw; Now_H = __sh;

    T_ms = 0;
    Dur_ms = __dur_ms;
    Active = 1;
}

/**
 * @brief 更新当前矩形位置
 * @param __dt_ms 时间增量（毫秒）
 */
void Class_ArkUI_Cursor::Update(uint16_t __dt_ms)
{
    if (!Active) return;

    T_ms = (uint16_t)(T_ms + __dt_ms);
    float t = Clamp01((float)T_ms / (float)Dur_ms);

    Now_X = Lerp(Start_X, Target_X, t);
    Now_Y = Lerp(Start_Y, Target_Y, t);
    Now_W = Lerp(Start_W, Target_W, t);
    Now_H = Lerp(Start_H, Target_H, t);

    if (T_ms >= Dur_ms)
    {
        Now_X = Target_X; Now_Y = Target_Y; Now_W = Target_W; Now_H = Target_H;
        Active = 0;
    }
}

/**
 * @brief 绘制光标
 */
void Class_ArkUI_Cursor::Draw_Cursor()
{
    if (!Visible) return;

    int16_t x = (int16_t)(Now_X + 0.5f);
    int16_t y = (int16_t)(Now_Y + 0.5f);
    int16_t w = (int16_t)(Now_W + 0.5f);
    int16_t h = (int16_t)(Now_H + 0.5f);

    if (w <= 0 || h <= 0) return;

    ArkUISetDrawColor(OLED_DRAWMODE_XOR);
    ArkUIDrawRBox(x, y, w, h, 1, Radius);
    ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
}

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
