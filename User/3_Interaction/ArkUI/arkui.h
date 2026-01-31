/**
 * @file arkui.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief ArkUI页面以及列表的定义
 * @version 1.0
 * @date 2026-1-26
 * @copyright Wang Fangzhuo (c) 2026
 */

#ifndef ARKUI_H
#define ARKUI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "arkui_config.h"
#include "arkui_cursor.h"

#include <string.h>
#include <stdarg.h>//用于支持 可变参数函数
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 定义参数类型 paramType，用于表示菜单项的参数类型
 */
typedef     double      paramType;

/**
 * @brief 定义枚举类型 ArkUIItem_e，用于表示不同功能的 UI 菜单项
 */
typedef enum
{
    ITEM_PAGE_DESCRIPTION,  // 页面描述项，用于显示页面的相关说明
    ITEM_JUMP_PAGE,         // 页面跳转项，用于跳转到其他页面
    ITEM_SWITCH,            // 开关项，通常用于切换某种状态
    ITEM_CHANGE_VALUE,      // 值修改项，用于修改某个数值
    ITEM_PROGRESS_BAR,      // 进度条项，用于显示任务的进度
    ITEM_RADIO_BUTTON,      // 单选按钮项，用于在多个选项中选择一个
    ITEM_CHECKBOX,          // 复选框项，用于在多个选项中选择多个
    ITEM_MESSAGE,            // 消息项，用于显示提示消息

    ITEM_CUSTOM             // 自定义项，用于用户自定义功能
} ArkUIItem_e;

/**
 * @brief 定义枚举类型 ArkUIPage_e，用于表示不同类型的 UI 页面
 */
typedef enum
{
    PAGE_LIST,    // 列表页面，用于以列表形式展示内容
    
    PAGE_CUSTOM   // 自定义页面，用于用户自定义布局和内容
} ArkUIPage_e;

/**
 * @brief 定义结构体 ArkUIItem_t，用于表示 UI 菜单项
 */
typedef struct ArkUI_item
{
    struct ArkUI_item *next;  // 指向下一个菜单项的指针，用于构建菜单项链表

    ArkUIItem_e funcType;     // 菜单项的功能类型，对应 ArkUIItem_e 枚举
    uint8_t id;               // 菜单项的唯一标识符
    int16_t lineId;           // 菜单项所在的行号,0~(ITEM_LINES-1)是在屏幕内
    float posForCal;          // 菜单项位置(浮点数版,用于累加步长时不出现取整抖动),和下面那个本质一样
    int16_t position;         // 菜单项位置
    float step;               // 每帧移动的步长（像素）
    char *title;              // 菜单项的标题

    void (*Event)(struct ArkUI_item *item);  // item触发的事件回调函数指针

    /*----- 标准可选属性（语义固定）-----*/
    const uint8_t *icon;            // 图标页面（PAGE_CUSTOM）使用的图标数据指针
    const char *msg;                // 消息项（ITEM_MESSAGE）显示的消息内容指针
    bool *flag;               // 复选框（ITEM_CHECKBOX）、单选按钮（ITEM_RADIO_BUTTON）和开关（ITEM_SWITCH）的状态标志指针
    bool flagDefault;         // 出厂默认设置的状态标志
    paramType *param;         // 值修改项（ITEM_CHANGE_VALUE）和进度条（ITEM_PROGRESS_BAR）的参数指针
    paramType paramDefault;   // 出厂默认设置的参数值
    paramType paramBackup;    // 值修改项（ITEM_CHANGE_VALUE）和进度条（ITEM_PROGRESS_BAR）的备份参数值
    uint8_t pageId;           // 页面跳转项（ITEM_JUMP_PAGE）要跳转的页面 ID
    /*----- 扩展槽：给 PAGE_CUSTOM / ITEM_CUSTOM / 未来新需求用 -----*/
    void *userData;               // 任意自定义数据/上下文指针（自定义结构体）

} ArkUIItem_t;

/**
 * @brief ArkUIItem_t参数包,用于初始化ArkUIItem_t
 */
typedef struct
{
    /* 标准可选属性 */
    const uint8_t *icon;      // 可选：图标
    const char *msg;                // 可选：message 文本
    bool *flag;               // 可选：checkbox/switch 状态指针
    paramType *param;         // 可选：数值/进度条参数指针
    uint8_t pageId;           // 可选：jump page 目标页

    /* 行为 */
    void (*Event)(ArkUIItem_t *item);  // 可选：事件/运行态回调

    /* 扩展槽 */
    void *userData;           // 可选：自定义扩展

} ArkUIItemArgs_t;

/**
 * @brief 定义结构体 ArkUI_page，用于表示 UI 页面
 */
typedef struct ArkUI_page
{
    struct ArkUI_page *next;  // 指向下一个页面的指针，用于构建页面链表

    ArkUIPage_e funcType;     // 页面的功能类型，对应 ArkUIPage_e 枚举
    ArkUIItem_t *itemHead, *itemTail;  // 页面中菜单项链表的头指针和尾指针
    uint8_t id;               // 页面的唯一标识符

    void (*Event)(struct ArkUI_page *page);  // 自定义页面（PAGE_CUSTOM）触发的事件回调函数指针

} ArkUIPage_t;

/* Exported variables --------------------------------------------------------*/

extern ArkUIPage_t *pageHead, *pageTail;
extern uint8_t pageIndex[MAX_LAYER];
extern uint8_t itemIndex[MAX_LAYER];
extern uint8_t layer;                
extern uint8_t opnUp,opnDown,opnClick,opnExit;
extern uint8_t g_arkui_tick_ms;
extern bool functionIsRunning, listLoop;
extern Class_ArkUI_Cursor Cursor;

/* Exported function declarations --------------------------------------------*/

/*------用户函数--------------------------------------------------------------*/
void ArkUIInit(void);
void ArkUI(uint8_t timer);
void ArkUIKeyActionMonitor(void);

/*------对于arkui_...可调用的工具函数-------------------------------------------*/
void ArkUIAddItem(ArkUIPage_t *page, ArkUIItem_t *item, char *_title, ArkUIItem_e func,const ArkUIItemArgs_t *args);
void ArkUIAddPage(ArkUIPage_t *page, ArkUIPage_e func, ...);
void ArkUIItemOperationResponse(ArkUIPage_t *page, ArkUIItem_t *item, uint8_t *index);

#ifdef __cplusplus
}
#endif

#endif

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
