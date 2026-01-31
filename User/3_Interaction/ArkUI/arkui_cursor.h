/**
 * @file arkui_cursor.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief ArkUI 通用光标管理器
 * @version 1.0
 * @date 2026-01-30
 * @copyright Wang Fangzhuo (c) 2026
 */

#ifndef ARKUI_CURSOR_H
#define ARKUI_CURSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "arkui_config.h"
#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 光标矩形位置结构体
 */
typedef struct
{
    float x;
    float y;
    float w;
    float h;
} Struct_ArkUI_Rect;

/**
 * @brief ArkUI 通用光标
 */
class Class_ArkUI_Cursor
{
public:
    void Init();

    inline void Set_Visible(uint8_t __visible);
    inline uint8_t Get_Visible();

    inline uint32_t Get_Object();

    inline Struct_ArkUI_Rect Get_Now_Cursor();

    void Set_Now(float __x, float __y, float __w, float __h, uint8_t __radius);

    void Goto(uint32_t __object, float __tx, float __ty, float __tw, float __th, uint16_t __dur_ms, uint8_t __radius);

    void Goto_From(uint32_t __object,float __sx, float __sy, float __sw, float __sh,float __tx, float __ty, float __tw, float __th,uint16_t __dur_ms, uint8_t __radius);

    void Update(uint16_t __dt_ms);

    void Draw_Cursor();

protected:
    // 当前矩形（浮点用于平滑）
    float Now_X = 0;
    float Now_Y = 0;
    float Now_W = 0;
    float Now_H = 0;

    // 起点
    float Start_X = 0;
    float Start_Y = 0;
    float Start_W = 0;
    float Start_H = 0;

    //目标
    float Target_X = 0;
    float Target_Y = 0;
    float Target_W = 0;
    float Target_H = 0;

    // 时间
    uint16_t T_ms = 0; //这次从起点->目标动画已经过去的时间
    uint16_t Dur_ms = 1; //目标动画的持续时间

    // 外观/状态
    uint8_t Radius = 1;//圆角半径
    uint8_t Visible = 1;//是否可见
    uint8_t Active = 0;//是否激活（正在进行动画）

    //对象的标识,Object = (pageId << 16) | (index << 8) | ...
    uint32_t Object = 0xFFFFFFFFu;

    // 工具函数
    inline float Lerp(float a, float b, float t);
    inline float Clamp01(float t);
};

/* Exported variables --------------------------------------------------------*/

/* Exported function declarations --------------------------------------------*/

/**
 * @brief 设置光标可见性
 * @param __visible 1 可见，0 不可见
 */
inline void Class_ArkUI_Cursor::Set_Visible(uint8_t __visible)
{
    Visible = __visible;
}

/**
 * @brief 获取光标可见性
 * @return uint8_t 1 可见，0 不可见
 */
inline uint8_t Class_ArkUI_Cursor::Get_Visible()
{
    return Visible;
}

/**
 * @brief 获取对象标识
 * @return uint32_t 当前对象的标识
 */
inline uint32_t Class_ArkUI_Cursor::Get_Object()
{
    return Object;
}

/**
 * @brief 获取当前光标位置和尺寸
 * @return Struct_ArkUI_Rect 当前光标矩形位置结构体
 */
inline Struct_ArkUI_Rect Class_ArkUI_Cursor::Get_Now_Cursor()
{
    Struct_ArkUI_Rect NowCursor;
    NowCursor.x = Now_X;
    NowCursor.y = Now_Y;
    NowCursor.w = Now_W;
    NowCursor.h = Now_H;
    return NowCursor;
}


#ifdef __cplusplus
}
#endif

#endif

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
