/**
 * @file arkui_event.cpp
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief 用于放置ArkUI的各种事件触发回调函数
 * @version 1.0
 * @date 2026-1-26
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "arkui_event.h"
#include "arkui_cursor.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/

/* Function definitions -------------------------------------------------------*/

 /**
 * @brief 修改Uint类型参数的事件处理函数
 *        该函数用于显示一个对话框，允许用户修改Uint类型的参数值，
 *        同时可以调整修改参数时的步进值，支持保存修改或返回初始值。
 * 
 * @param item 指向 ArkUIItem_t 结构体的指针，包含要修改的参数信息
 */
void ArkUIEventChangeUint(ArkUIItem_t *item)
{
    static int16_t x, y;
    static uint16_t width, height;
    static uint8_t index = 1, step = 1;
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1;
    static bool changeVal = false, changeStep = false;

    ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

    // Display information and draw box
    height = ITEM_HEIGHT * 4 + 2;
    if (strlen(item->title) + 1 > 12)
        width = (strlen(item->title) + 1) * FONT_WIDTH + 7;
    else
        width = 12 * FONT_WIDTH + 7;
    if (width < 2 * SCREEN_WIDTH / 3)
        width = 2 * SCREEN_WIDTH / 3;
    x = (SCREEN_WIDTH - width) / 2;
    y = (SCREEN_HEIGHT - height) / 2;

	ArkUIDrawRBox(x - 1,y - 1,width + 2,height + 2,0,8);
	ArkUIDrawRFrame(x - 1,y - 1,width + 2, height + 2,1,8);
    ArkUIDisplayStr(x + 3, y + itemHeightOffset, item->title);
    ArkUIDisplayStr(x + 3 + strlen(item->title) * FONT_WIDTH, y + itemHeightOffset, ":");
    ArkUIDisplayStr(x + 3, y + 2 * ITEM_HEIGHT + itemHeightOffset, "Step:");
    ArkUIDisplayStr(x + 3, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Save");
    ArkUIDisplayStr(x + width - 6 * FONT_WIDTH - 4, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Return");
    // Change value of param or step
    if (changeVal)
    {
        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        ArkUIDrawRBox(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
        if (opnUp)
            *item->param += step;
        if (opnDown)
        {
            if (*item->param - step >= 0)
                *item->param -= step;
            else
                *item->param = 0;
        }
    } else if (changeStep)
    {
        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        ArkUIDrawRBox(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
        if (opnUp)
        {
            if (step == 1)
                step = 10;
            else if (step == 10)
                step = 100;
            else
                step = 1;
        }
        if (opnDown)
        {
            if (step == 100)
                step = 10;
            else if (step == 10)
                step = 1;
            else
                step = 100;
        }
    } else
    {
        if (opnDown)
        {
            if (index < 4)
                index++;
            else
                index = 1;
        }
        if (opnUp)
        {
            if (index > 1)
                index--;
            else
                index = 4;
        }
    }

    // Display step
		ArkUIPrintf(x + 3,y + ITEM_HEIGHT + itemHeightOffset,"%d",(unsigned int)*item->param);
    if (step == 1)
        ArkUIDisplayStr(x + 3 + 9 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "1");
    else if (step == 10)
        ArkUIDisplayStr(x + 3 + 8 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "10");
    else
        ArkUIDisplayStr(x + 3 + 7 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "100");

    // Draw indicator
    if (index == 1)
        ArkUIDrawRFrame(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else if (index == 2)
        ArkUIDrawRFrame(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else if (index == 3)
        ArkUIDrawRFrame(x + 1, y + 1 + 3 * ITEM_HEIGHT, 4 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else
        ArkUIDrawRFrame(x + width - 6 * FONT_WIDTH - 6, y + 1 + 3 * ITEM_HEIGHT, 6 * FONT_WIDTH + 5, ITEM_HEIGHT,
                         1, 4);

    // Operation move reaction
    if (opnClick)
    {
        if (index == 1)
            changeVal = true;
        else if (index == 2)
            changeStep = true;
        else if (index == 3)
        {
            item->paramBackup = *item->param;
            functionIsRunning = false;
            ArkUIBackgroundBlur();
            index = 1;
            step = 1;
        } else
        {
            *item->param = item->paramBackup;
            functionIsRunning = false;
            ArkUIBackgroundBlur();
            index = 1;
            step = 1;
        }
    }
    if (opnExit)
    {
        if (index == 1)
            changeVal = false;
        else if (index == 2)
            changeStep = false;
    }

    // Clear the states of key to monitor next key action
    opnExit = opnUp = opnDown = opnClick = false;

    ArkUISendBuffer();
}

/**
 * @brief 数值参数项变更事件（光标模式）,残次版,以后修
 * @param item 数值参数项指针
 */
void ArkUIEventChangeUint_Cursor(ArkUIItem_t *item)
{
    static int16_t x, y;
    static uint16_t width, height;
    static uint8_t index = 1, step = 1;
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1;
    static bool changeVal = false, changeStep = false;

    // --- Cursor 衔接相关 ---
    static uint8_t cursorInited = 0;
    static uint8_t cursorLastIndex = 0;

    ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

    // Display information and draw box
    height = ITEM_HEIGHT * 4 + 2;
    if (strlen(item->title) + 1 > 12)
        width = (strlen(item->title) + 1) * FONT_WIDTH + 7;
    else
        width = 12 * FONT_WIDTH + 7;
    if (width < 2 * SCREEN_WIDTH / 3)
        width = 2 * SCREEN_WIDTH / 3;
    x = (SCREEN_WIDTH - width) / 2;
    y = (SCREEN_HEIGHT - height) / 2;

    ArkUIDrawRBox(x - 1, y - 1, width + 2, height + 2, 0, 8);
    ArkUIDrawRFrame(x - 1, y - 1, width + 2, height + 2, 1, 8);

    ArkUIDisplayStr(x + 3, y + itemHeightOffset, item->title);
    ArkUIDisplayStr(x + 3 + strlen(item->title) * FONT_WIDTH, y + itemHeightOffset, ":");
    ArkUIDisplayStr(x + 3, y + 2 * ITEM_HEIGHT + itemHeightOffset, "Step:");
    ArkUIDisplayStr(x + 3, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Save");
    ArkUIDisplayStr(x + width - 6 * FONT_WIDTH - 4, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Return");

    // Change value of param or step
    if (changeVal)
    {
        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        ArkUIDrawRBox(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

        if (opnUp)
            *item->param += step;
        if (opnDown)
        {
            if (*item->param - step >= 0)
                *item->param -= step;
            else
                *item->param = 0;
        }
    }
    else if (changeStep)
    {
        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        ArkUIDrawRBox(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

        if (opnUp)
        {
            if (step == 1) step = 10;
            else if (step == 10) step = 100;
            else step = 1;
        }
        if (opnDown)
        {
            if (step == 100) step = 10;
            else if (step == 10) step = 1;
            else step = 100;
        }
    }
    else
    {
        if (opnDown)
        {
            if (index < 4) index++;
            else index = 1;
        }
        if (opnUp)
        {
            if (index > 1) index--;
            else index = 4;
        }
    }

    // Display value & step
    ArkUIPrintf(x + 3, y + ITEM_HEIGHT + itemHeightOffset, "%d", (unsigned int)*item->param);

    if (step == 1)
        ArkUIDisplayStr(x + 3 + 9 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "1");
    else if (step == 10)
        ArkUIDisplayStr(x + 3 + 8 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "10");
    else
        ArkUIDisplayStr(x + 3 + 7 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "100");

    // -------- Cursor：衔接到对话框选中框（非编辑模式才显示） --------
    {
        const uint16_t dt_ms = (uint16_t)(g_arkui_tick_ms ? g_arkui_tick_ms : 1);
        const uint8_t cursorVisible = (changeVal || changeStep) ? 0 : 1;
        Cursor.Set_Visible(cursorVisible);

        // 计算当前 index 的目标框（和你原来的 RFrame 一致）
        float targetX, targetY, targetW, targetH;
        targetH = (float)ITEM_HEIGHT;

        if (index == 1)
        {
            targetX = (float)(x + 1);
            targetY = (float)(y + 1);
            targetW = (float)((strlen(item->title) + 1) * FONT_WIDTH + 5);
        }
        else if (index == 2)
        {
            targetX = (float)(x + 1);
            targetY = (float)(y + 1 + 2 * ITEM_HEIGHT);
            targetW = (float)(5 * FONT_WIDTH + 5);
        }
        else if (index == 3)
        {
            targetX = (float)(x + 1);
            targetY = (float)(y + 1 + 3 * ITEM_HEIGHT);
            targetW = (float)(4 * FONT_WIDTH + 5);
        }
        else
        {
            targetX = (float)(x + width - 6 * FONT_WIDTH - 6);
            targetY = (float)(y + 1 + 3 * ITEM_HEIGHT);
            targetW = (float)(6 * FONT_WIDTH + 5);
        }

        if (cursorVisible)
        {
            const uint32_t dlgObject = 0xE0000000u | ((uint32_t)item->id << 8) | (uint32_t)index;

            if (!cursorInited)
            {
                // 第一帧：从上一页 Cursor 的当前位置衔接过来
                Struct_ArkUI_Rect from = Cursor.Get_Now_Cursor();
                if (from.w <= 0.0f || from.h <= 0.0f)
                {
                    // 避免上电默认 0 矩形导致从左上角飞
                    from.x = targetX; from.y = targetY; from.w = targetW; from.h = targetH;
                    Cursor.Set_Now(from.x, from.y, from.w, from.h, 4);
                }

                Cursor.Goto_From(dlgObject,
                                 from.x, from.y, from.w, from.h,
                                 targetX, targetY, targetW, targetH,
                                 120, 4);  // 120ms：你想更快就 80/60
                cursorInited = 1;
                cursorLastIndex = index;
            }
            else
            {
                // index 变了就推新目标
                if (cursorLastIndex != index || Cursor.Get_Object() != dlgObject)
                {
                    Cursor.Goto(dlgObject, targetX, targetY, targetW, targetH, 80, 4);
                    cursorLastIndex = index;
                }
            }

            Cursor.Update(dt_ms);
            Cursor.Draw_Cursor();
        }
    }
    // ---------------------------------------------------------------

    // Draw indicator（保留你原来的外框提示；如果你觉得重复，可以把这一段删掉）
    if (index == 1)
        ArkUIDrawRFrame(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else if (index == 2)
        ArkUIDrawRFrame(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else if (index == 3)
        ArkUIDrawRFrame(x + 1, y + 1 + 3 * ITEM_HEIGHT, 4 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else
        ArkUIDrawRFrame(x + width - 6 * FONT_WIDTH - 6, y + 1 + 3 * ITEM_HEIGHT, 6 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);

    // Operation move reaction
    if (opnClick)
    {
        if (index == 1)
            changeVal = true;
        else if (index == 2)
            changeStep = true;
        else if (index == 3)
        {
            item->paramBackup = *item->param;
            functionIsRunning = false;
            ArkUIBackgroundBlur();
            index = 1;
            step = 1;

            // 退出对话框：下次进来重新做一次衔接
            cursorInited = 0;
            cursorLastIndex = 0;
            Cursor.Set_Visible(1);
        }
        else
        {
            *item->param = item->paramBackup;
            functionIsRunning = false;
            ArkUIBackgroundBlur();
            index = 1;
            step = 1;

            // 退出对话框：下次进来重新做一次衔接
            cursorInited = 0;
            cursorLastIndex = 0;
            Cursor.Set_Visible(1);
        }
    }

    if (opnExit)
    {
        if (index == 1)
            changeVal = false;
        else if (index == 2)
            changeStep = false;
        // 退出编辑模式后 Cursor 会自动重新显示并跟随 index（上面逻辑会处理）
    }

    // Clear the states of key to monitor next key action
    opnExit = opnUp = opnDown = opnClick = false;

    ArkUISendBuffer();
}


/**
 * @brief 修改Int类型参数的事件处理函数
 *        该函数用于显示一个对话框，允许用户修改Int类型的参数值，
 *        同时可以调整修改参数时的步进值，支持保存修改或返回初始值。
 * 
 * @param item 指向 ArkUIItem_t 结构体的指针，包含要修改的参数信息
 */
void ArkUIEventChangeInt(ArkUIItem_t *item)
{
    static int16_t x, y;
    static uint16_t width, height;
    static uint8_t index = 1, step = 1;
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1;
    static bool changeVal = false, changeStep = false;

    ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

    // Display information and draw box
    height = ITEM_HEIGHT * 4 + 2;
    if (strlen(item->title) + 1 > 12)
        width = (strlen(item->title) + 1) * FONT_WIDTH + 7;
    else
        width = 12 * FONT_WIDTH + 7;
    if (width < 2 * SCREEN_WIDTH / 3)
        width = 2 * SCREEN_WIDTH / 3;
    x = (SCREEN_WIDTH - width) / 2;
    y = (SCREEN_HEIGHT - height) / 2;
		
	ArkUIDrawRBox(x - 1,y - 1,width + 2,height + 2,0,8);
	ArkUIDrawRFrame(x - 1,y - 1,width + 2, height + 2,1,8);
    ArkUIDisplayStr(x + 3, y + itemHeightOffset, item->title);
    ArkUIDisplayStr(x + 3 + strlen(item->title) * FONT_WIDTH, y + itemHeightOffset, ":");
    ArkUIDisplayStr(x + 3, y + 2 * ITEM_HEIGHT + itemHeightOffset, "Step:");
    ArkUIDisplayStr(x + 3, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Save");
    ArkUIDisplayStr(x + width - 6 * FONT_WIDTH - 4, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Return");

    // Change value of param or step
    if (changeVal)
    {
        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        ArkUIDrawRBox(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
        if (opnUp)
            *item->param += step;
        if (opnDown)
            *item->param -= step;
    } else if (changeStep)
    {
        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        ArkUIDrawRBox(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
        if (opnUp)
        {
            if (step == 1)
                step = 10;
            else if (step == 10)
                step = 100;
            else
                step = 1;
        }
        if (opnDown)
        {
            if (step == 100)
                step = 10;
            else if (step == 10)
                step = 1;
            else
                step = 100;
        }
    } else
    {
        if (opnDown)
        {
            if (index < 4)
                index++;
            else
                index = 1;
        }
        if (opnUp)
        {
            if (index > 1)
                index--;
            else
                index = 4;
        }
    }

    // Display step
		ArkUIPrintf(x + 3,y + ITEM_HEIGHT + itemHeightOffset,"%d",(int)*item->param);
    if (step == 1)
        ArkUIDisplayStr(x + 3 + 9 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "1");
    else if (step == 10)
        ArkUIDisplayStr(x + 3 + 8 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "10");
    else
        ArkUIDisplayStr(x + 3 + 7 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "100");

    // Draw indicator
    if (index == 1)
        ArkUIDrawRFrame(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else if (index == 2)
        ArkUIDrawRFrame(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else if (index == 3)
        ArkUIDrawRFrame(x + 1, y + 1 + 3 * ITEM_HEIGHT, 4 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else
        ArkUIDrawRFrame(x + width - 6 * FONT_WIDTH - 6, y + 1 + 3 * ITEM_HEIGHT, 6 * FONT_WIDTH + 5, ITEM_HEIGHT,
                         1, 4);

    // Operation move reaction
    if (opnClick)
    {
        if (index == 1)
            changeVal = true;
        else if (index == 2)
            changeStep = true;
        else if (index == 3)
        {
            item->paramBackup = *item->param;
            functionIsRunning = false;
            ArkUIBackgroundBlur();
            index = 1;
            step = 1;
        } else
        {
            *item->param = item->paramBackup;
            functionIsRunning = false;
            ArkUIBackgroundBlur();
            index = 1;
            step = 1;
        }
    }
    if (opnExit)
    {
        if (index == 1)
            changeVal = false;
        else if (index == 2)
            changeStep = false;
    }

    // Clear the states of key to monitor next key action
    opnExit = opnUp = opnDown = opnClick = false;

    ArkUISendBuffer();
}

/**
 * @brief 修改浮点数类型参数的事件处理函数
 *        该函数用于显示一个对话框，允许用户修改浮点数类型的参数值，
 *        同时可以调整修改参数时的步进值，支持保存修改或返回初始值。
 * 
 * @param item 指向 ArkUIItem_t 结构体的指针，包含要修改的参数信息
 */
void ArkUIEventChangeFloat(ArkUIItem_t *item)
{
    // 静态变量，用于记录对话框的位置
    static int16_t x, y;
    // 静态变量，用于记录对话框的尺寸
    static uint16_t width, height;
    // 静态变量，用于记录当前选中项的索引，初始值为 1
    static uint8_t index = 1;
    // 静态变量，用于记录修改参数时的步进值，初始值为 0.0001
    static double step = 0.0001;
    // 静态变量，用于记录文本垂直居中偏移量
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1;
    // 静态变量，用于标记是否正在修改参数值
    static bool changeVal = false;
    // 静态变量，用于标记是否正在修改步进值
    static bool changeStep = false;

    // 设置绘制模式为正常模式
    ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

    // 计算对话框高度，固定为 4 个列表项高度加 2 像素边距
    height = ITEM_HEIGHT * 4 + 2;
    // 根据标题长度计算对话框宽度，如果标题较长则使用标题长度计算，否则使用默认值 12
    if (strlen(item->title) + 1 > 12)
        width = (strlen(item->title) + 1) * FONT_WIDTH + 7;
    else
        width = 12 * FONT_WIDTH + 7;
    // 确保对话框宽度不小于屏幕宽度的 2/3
    if (width < 2 * SCREEN_WIDTH / 3)
        width = 2 * SCREEN_WIDTH / 3;
    // 计算对话框水平居中位置
    x = (SCREEN_WIDTH - width) / 2;
    // 计算对话框垂直居中位置
    y = (SCREEN_HEIGHT - height) / 2;


	ArkUIDrawRBox(x - 1,y - 1,width + 2,height + 2,0,8);
	ArkUIDrawRFrame(x - 1,y - 1,width + 2, height + 2,1,8);
    // 显示对话框标题
    ArkUIDisplayStr(x + 3, y + itemHeightOffset, item->title);
    // 显示对话框标题后的冒号
    ArkUIDisplayStr(x + 3 + strlen(item->title) * FONT_WIDTH, y + itemHeightOffset, ":");
    // 显示步进值提示文本
    ArkUIDisplayStr(x + 3, y + 2 * ITEM_HEIGHT + itemHeightOffset, "Step:");
    // 显示保存提示文本
    ArkUIDisplayStr(x + 3, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Save");
    // 显示返回提示文本
    ArkUIDisplayStr(x + width - 6 * FONT_WIDTH - 4, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Return");

    // 根据状态修改参数值或步进值
    // 如果正在修改参数值
    if (changeVal)
    {
        // 设置绘制模式为异或模式，高亮显示当前修改区域
        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        // 绘制高亮区域
        ArkUIDrawRBox(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
        // 恢复绘制模式为正常模式
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
        // 如果向上按键被按下，增加参数值
        if (opnUp)
            *item->param += step;
        // 如果向下按键被按下，减少参数值
        if (opnDown)
            *item->param -= step;
    } 
    // 如果正在修改步进值
    else if (changeStep)
    {
        // 设置绘制模式为异或模式，高亮显示当前修改区域
        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        // 绘制高亮区域
        ArkUIDrawRBox(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
        // 恢复绘制模式为正常模式
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
        // 如果向上按键被按下，调整步进值
        if (opnUp)
        {
						if (step == 0.0001)
								step = 0.001;
            else if (step == 0.001)
                step = 0.01;
            else if (step == 0.01)
                step = 0.1;
            else if (step == 0.1)
                step = 1;
						else 
								step = 0.0001;
        }
        // 如果向下按键被按下，调整步进值
        if (opnDown)
        {
						if (step == 0.0001)
								step = 1;
            else if (step == 1)
                step = 0.1;
            else if (step == 0.1)
                step = 0.01;
            else if (step == 0.01)
                step = 0.001;
						else 
								step = 0.0001;
        }
    } 
    // 如果既不修改参数值也不修改步进值，则处理选择项切换
    else
    {
        // 如果向前按键被按下，切换选中项
        if (opnDown)
        {
            if (index < 4)
                index++;
            else
                index = 1;
        }
        // 如果向后按键被按下，切换选中项
        if (opnUp)
        {
            if (index > 1)
                index--;
            else
                index = 4;
        }
    }
    // Display step
	ArkUIPrintf(x + 3,y + ITEM_HEIGHT + itemHeightOffset,"%.4f",*item->param);

	if (step == 0.0001)
        ArkUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "0.0001");
    else if (step == 0.001)
        ArkUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "0.001");
    else if (step == 0.01)
        ArkUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "0.01");
    else if (step == 0.1)
        ArkUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "0.1");
		else 
        ArkUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "1");
    // Draw indicator
    if (index == 1)
        ArkUIDrawRFrame(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else if (index == 2)
        ArkUIDrawRFrame(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else if (index == 3)
        ArkUIDrawRFrame(x + 1, y + 1 + 3 * ITEM_HEIGHT, 4 * FONT_WIDTH + 5, ITEM_HEIGHT, 1, 4);
    else
        ArkUIDrawRFrame(x + width - 6 * FONT_WIDTH - 6, y + 1 + 3 * ITEM_HEIGHT, 6 * FONT_WIDTH + 5, ITEM_HEIGHT,
                         1, 4);
    // Operation move reaction
    if (opnClick)
    {
        if (index == 1)
            changeVal = true;
        else if (index == 2)
            changeStep = true;
        else if (index == 3)
        {
            item->paramBackup = *item->param;
            functionIsRunning = false;
            ArkUIBackgroundBlur();
            index = 1;
            step = 0.01;
        } else
        {
            *item->param = item->paramBackup;
            functionIsRunning = false;
            ArkUIBackgroundBlur();
            index = 1;
            step = 0.01;
        }
    }
    if (opnExit)
    {
        if (index == 1)
            changeVal = false;
        else if (index == 2)
            changeStep = false;
    }

    // Clear the states of key to monitor next key action
    opnExit = opnUp = opnDown = opnClick = false;

    ArkUISendBuffer();
}

/**
 * @brief 退出函数执行
 *        当确认键被点击时，检查是否有函数正在运行。如果有，将函数运行状态设置为false。
 *        同时，将确认键点击状态重置为false。
 */
void ArkFunctionExit(ArkUIItem_t *item)
{
    (void) item;

	if(opnExit){
		functionIsRunning = false;
	}
	opnExit = false;
}

/*---------------------------------IconPageEvent相关逻辑-BEGIN--------------------------------------------------*/

/**
 * @brief 根据id查找页面中的项
 * @param pg 要查找项的页面
 * @param id 要查找的项的ID
 * @note 这是个辅助函数，IconPageEvent 中调用。
 * @return ArkUIItem_t* 指向匹配项的指针，如果未找到匹配项则返回 NULL。
 */
static ArkUIItem_t *ArkUI_FindItemById(ArkUIPage_t *pg, int id)
{
    if (pg == NULL || id < 0) return NULL;

    for (ArkUIItem_t *it = pg->itemHead; it != NULL; it = it->next) {
        if ((int)it->id == id) return it;
    }
    return NULL;
}

// icon页面是否需要进入动画
volatile uint8_t g_iconNeedEntryAnim = 0;

//进场动画的起点提示,给entryStartX/Y备用
int16_t g_icon_from_x = -1;
int16_t g_icon_from_y = -1;

// 进入 Icon Page 时，光标插值起点（从上一个页面的光标矩形继承）
static uint8_t g_icon_cursor_from_valid = 0; 
static Struct_ArkUI_Rect g_icon_cursor_from = {0,0,0,0};

/**
 * @brief 图标页面事件处理函数,动画纵享丝滑
 *        1. 处理页面的交互事件，包括图标显示、点击响应等。
 *        2. 该函数在主循环中被调用，用于响应用户输入。
 * @param page 指向 ArkUIPage_t 结构体的指针，代表要处理事件的页面。
 */
void IconPageEvent(ArkUIPage_t *page)
{
    static int entryStartX = -1;
    static int entryStartY = -1;
    /* ================ 0) 配置常量 （统一调参） ================ */
    const int ICON_W          = 32;   // 图标宽 (px)
    const int ICON_H          = 32;   // 图标高 (px)
    const int ICON_SPACING    = 10;   // 图标间距（中心到中心额外距离）
    const int SLOT_DX         = ICON_W + ICON_SPACING; // 槽位中心间距
    const int ICON_CENTER_Y   = (SCREEN_HEIGHT * 2) / 5; // 图标群垂直中心（上 ~2/3）
    const int TITLE_Y         = SCREEN_HEIGHT - FONT_HEIGHT - 6; // 标题目标 Y（底部固定）
    const int CURSOR_RADIUS   = 6;    // XOR 圆角半径
    const int CURSOR_ADHERE   = 8;    // 光标吸附最大像素偏移（小幅）6
    const int ENTRY_FRAMES    = 12;   // 页面进场动画帧数（tick）
    const int MOVE_FRAMES     = 8;    // 翻页动画帧数（tick）
    const int ENTRY_OFFSET    = (SCREEN_WIDTH / 2) + SLOT_DX; // 进场时图标起始偏移（右侧外）
    const int VISIBLE_WINDOW  = 12;   // 绘制窗口半宽（为性能可调）
    const int TITLE_EXTRA_PAD = 8;    // 标题脱屏额外缓冲（保证完全出屏）
    const int centerX         = SCREEN_WIDTH / 2; // 屏幕中心 X（IconPage 多处用到）
    /* ========================================================= */

    /* ================ 1) 防空与总项数 ================ */
    if (page == NULL || page->itemTail == NULL) return;      // 无页面或无项则退出
    int totalItems = (int)page->itemTail->id + 1;            // 计算总项数
    if (totalItems <= 0) return;                             // 无项退出

    /* ================ 2) 读取当前（全局）选中索引并容错 ================ */
    int curIndex = (int)itemIndex[layer];                   // 读取全局索引
    if (curIndex < 0) curIndex = 0;                         // 越界保护
    if (curIndex >= totalItems) curIndex = 0;

    /* ================ 3) 静态跨帧状态：动画 & 标题状态 ================ */
    // 这些 static 变量用于跨帧保持动画/标题的状态
    static int lastPageId = -1;      // 用于检测页面激活（page->id 变化）
    static int lastLayerSeen = -1;   // 用于检测 layer 变化（从二级返回时判断）

    // 进场动画状态（图标：右->左）
    static int entryPlaying = 0;     // 0/1 标志是否在进场动画
    static int entryTick = 0;        // 进场帧计数

    // 翻页动画状态（图标从 startCenter -> endCenter）
    static int movePlaying = 0;      // 0/1 标志是否在翻页动画
    static int moveTick = 0;         // 翻页帧计数
    static int moveFrom = 0;         // 翻页起始中心索引
    static int moveTo = 0;           // 翻页目标中心索引
    static int moveDir = 0;          // 图标整体移动方向： -1 left (next), +1 right (prev)

    // 标题的独立状态机（关键修复点）
    // titleCurrent: 当前稳定显示的标题对应 item id（-1 表示尚无）
    // titlePlaying: 标题是否正在过渡（纵向进/出）
    // titleFrom, titleTo: 过渡的旧标题/新标题 id（titleFrom == -1 表示没有旧标题，入场场景）
    // titleTick/titleDur: 标题过渡的帧计数与总帧
    static int titleCurrent = -1;    // 初始为 -1 表示尚未显示任何标题
    static int titlePlaying = 0;     // 0/1 标志标题是否在过渡
    static int titleFrom = -1;       // 旧标题 id（-1 表示无）
    static int titleTo = -1;         // 新标题 id
    static int titleTick = 0;        // 标题过渡帧计数
    static int titleDur = 0;         // 标题过渡总帧（ENTRY_FRAMES 或 MOVE_FRAMES）

    /* ================ 4) 页面激活检测：触发进场（并准备标题入场） ================ */
    // 当检测到页面 id 或 layer 变化时，开始页面进场动画。
    // 另外：从 icon -> item 再回到 icon，layer/pageId 都可能“看起来没变”，
    // 这种情况下用 Cursor 的 Object 来判断“刚回到 icon page”，用于光标二次衔接。
    const uint32_t iconObject = ((uint32_t)page->id << 16) | ((uint32_t)curIndex << 8) | 0x01u;
    const uint32_t lastCursorObject = Cursor.Get_Object();
    const uint8_t justReturnedToIcon = (uint8_t)(((lastCursorObject >> 16) & 0xFFFFu) != (uint32_t)page->id);

    // 只要“进入到 icon 页面”（包括：从 item/event 返回、layer 不变、pageId 不变等情况）就播进场动画。
    // justReturnedToIcon 用 Cursor.Object 判断上一帧是否处在其他页面：
    // 这样即便从 icon -> item -> icon，pageId/layer 看起来没变，也会触发动画。
    if (g_iconNeedEntryAnim || justReturnedToIcon || (int)page->id != lastPageId || lastLayerSeen != layer) {

        // 1) 准备进场起点（可选：从 list item 的中心进来）
        if (g_icon_from_y >= 0) {
            entryStartX = g_icon_from_x;
            entryStartY = g_icon_from_y;
            g_icon_from_x = -1;
            g_icon_from_y = -1;
        } else {
            entryStartX = -1;
            entryStartY = -1;
        }
        g_iconNeedEntryAnim = 0;

        // 2) 启动进场动画（图标从右向左进入）
        entryPlaying = 1;
        entryTick = 0;
        // 取消任何遗留翻页动画，保证干净进入
        movePlaying = 0;
        moveTick = 0;
        // 记录当前中心：进场结束后应停到 curIndex
        moveFrom = moveTo = curIndex;

        // 3) 标题入场：没有旧标题（-1），新的标题为 curIndex
        titlePlaying = 1;
        titleFrom = -1;
        titleTo = curIndex;
        titleTick = 0;
        titleDur = ENTRY_FRAMES;

        // 4) 更新激活检测标记
        lastPageId = (int)page->id;
        lastLayerSeen = layer;

        // 5) entry 开始：Cursor 跨页面插值（item->icon / event->icon）
        {
            const uint16_t dt_ms = (uint16_t)(g_arkui_tick_ms ? g_arkui_tick_ms : 1);
            const float targetW = (float)(ICON_W + 3);
            const float targetH = (float)(ICON_H + 3);
            const float targetX = (float)(centerX) - targetW * 0.5f;
            const float targetY = (float)(ICON_CENTER_Y) - targetH * 0.5f;

            Struct_ArkUI_Rect from = {0,0,0,0};
            bool fromPrevCursor = false;

            // 关键：如果是“从别的页面返回到 icon 页”触发的进场（justReturnedToIcon），
            // 那么 Cursor 里仍然保存着上一页的 Now 矩形，直接用它作为 from，保证衔接不断。
            if (!g_icon_cursor_from_valid && justReturnedToIcon)
            {
                from = Cursor.Get_Now_Cursor();
                // 过滤掉上电默认的 0 矩形（或不合理的矩形）
                if (from.w > 0 && from.h > 0)
                    fromPrevCursor = true;
                else
                    fromPrevCursor = false;
            }

            // 如果外部显式提供了 from（例如 item->icon 的 EnterIconPage），优先使用它
            if (g_icon_cursor_from_valid)
            {
                from = g_icon_cursor_from;
                g_icon_cursor_from_valid = 0;
                fromPrevCursor = true;
            }

            // 否则用 entryStartX/Y（或默认屏幕外）生成一个 from
            if (!fromPrevCursor)
            {
                float startCX = (entryStartX >= 0) ? (float)entryStartX : (-(float)ICON_W - 8.0f);
                float startCY = (entryStartY >= 0) ? (float)entryStartY : (float)ICON_CENTER_Y;
                from.x = startCX - targetW * 0.5f;
                from.y = startCY - targetH * 0.5f;
                from.w = targetW;
                from.h = targetH;
            }

            uint16_t dur_ms = (uint16_t)(ENTRY_FRAMES * dt_ms);
            // item -> icon 时的光标连贯插值：加快一点（不影响图标进场速度）
            if (fromPrevCursor)
            {
                // 0.60 倍时长 ≈ 1.6 倍速度，可按手感再微调
                dur_ms = (uint16_t)((float)dur_ms * 0.60f);
            }
            if (dur_ms == 0) dur_ms = 1;

            Cursor.Goto_From(iconObject, from.x, from.y, from.w, from.h,
                             targetX, targetY, targetW, targetH,
                             dur_ms, (uint8_t)CURSOR_RADIUS);
        }
    } else {
        // 页面未发生“激活变化”，仅更新 layer 记录
        lastLayerSeen = layer;
    }

    /* ================ 5) 输入：在没有动画时响应翻页并启动标题过渡 ================ */
    
    // 允许动画被按键打断：一旦检测到输入，立即“快进”到动画结束态，然后在同一帧继续处理该输入。
    if ((opnUp || opnDown || opnClick || opnExit) && (entryPlaying || movePlaying))
    {
        // 1) 若正在翻页动画：先落到目标索引
        if (movePlaying)
        {
            curIndex = moveTo;
            movePlaying = 0;
            moveTick = MOVE_FRAMES;
        }

        // 2) 若正在进场动画：直接结束进场
        if (entryPlaying)
        {
            entryPlaying = 0;
            entryTick = ENTRY_FRAMES;
        }

        // 3) 标题立即稳定到当前索引
        titlePlaying = 0;
        titleCurrent = curIndex;
        titleFrom = curIndex;
        titleTo = curIndex;
        titleTick = 0;

        // 4) 光标立即落到当前目标框（防止从屏幕外/左上角飞入造成“断开”）
        {
            const uint32_t iconObjectNow = ((uint32_t)page->id << 16) | ((uint32_t)curIndex << 8) | 0x01u;

            // 这里不能直接用后面 section 10 的 targetX/targetY/dt_ms（作用域不在这里），所以本地重算一次。
            const uint16_t dt_ms_now = (uint16_t)(g_arkui_tick_ms ? g_arkui_tick_ms : 1);
            const float targetW_now = (float)(ICON_W + 3);
            const float targetH_now = (float)(ICON_H + 3);
            const float targetX_now = (float)(centerX) - targetW_now * 0.5f;
            const float targetY_now = (float)(ICON_CENTER_Y) - targetH_now * 0.5f;

            Cursor.Goto(iconObjectNow, targetX_now, targetY_now, targetW_now, targetH_now, 1, (uint8_t)CURSOR_RADIUS);
            Cursor.Update(dt_ms_now); // 立刻完成 (Dur=1ms)
        }
    }

    // 只有当没有进场或翻页动画时才响应输入，避免动画被打断。
    if (!entryPlaying && !movePlaying) {
        // 向下（下一项）
        if (opnDown) {
            // 普通情况：若不是最后一项 -> moveTo = curIndex+1, icons 向左移动 (moveDir = -1)
            if (curIndex < totalItems - 1) {
                moveFrom = curIndex; moveTo = curIndex + 1; moveDir = -1;
            } else {
                // 在末端继续向下 -> wrap 到 0，整体 icons 向右移动 (moveDir = +1)
                moveFrom = curIndex; moveTo = 0; moveDir = +1;
            }
            // 启动翻页动画
            movePlaying = 1; moveTick = 0;
            // **关键**：同时启动标题过渡（旧标题下滑，新标题从下滑入）
            titlePlaying = 1;
            titleFrom = titleCurrent;  // 旧标题为当前已稳定显示的标题（可能为 -1）
            titleTo = moveTo;          // 新标题为移动目标
            titleTick = 0;
            titleDur = MOVE_FRAMES;    // 与翻页时长一致
        }
        // 向上（上一项）
        else if (opnUp) {
            if (curIndex > 0) {
                moveFrom = curIndex; moveTo = curIndex - 1; moveDir = +1;
            } else {
                // 在第一个继续向上 -> wrap 到最后一项，整体 icons 向左移动
                moveFrom = curIndex; moveTo = totalItems - 1; moveDir = -1;
            }
            movePlaying = 1; moveTick = 0;
            // 同步启动标题过渡
            titlePlaying = 1;
            titleFrom = titleCurrent;
            titleTo = moveTo;
            titleTick = 0;
            titleDur = MOVE_FRAMES;
        }
    }

    /* ================ 6) 推进帧计数并计算 easing 值（pure C: easeOutCubic） ================ */
    // 进场缓动 t_entry
    float t_entry = 1.0f;
    if (entryPlaying) {
        entryTick++;
        if (entryTick >= ENTRY_FRAMES) { entryTick = ENTRY_FRAMES; entryPlaying = 0; }
        float te = (float)entryTick / (float)ENTRY_FRAMES;
        float p = te - 1.0f;
        t_entry = (p * p * p + 1.0f); // easeOutCubic
    }
    // 翻页缓动 t_move
    float t_move = 1.0f;
    if (movePlaying) {
        moveTick++;
        if (moveTick >= MOVE_FRAMES) {
            moveTick = MOVE_FRAMES;
            movePlaying = 0;
            // 翻页完成：将全局索引写回为 moveTo
            curIndex = moveTo;
            itemIndex[layer] = (uint8_t)curIndex;
        }
        float tm = (float)moveTick / (float)MOVE_FRAMES;
        float q = tm - 1.0f;
        t_move = (q * q * q + 1.0f); // easeOutCubic
    }

    /* ================ 7) 计算 startCenter / endCenter（决定所有项位置） ================ */
    int startCenter = movePlaying ? moveFrom : curIndex; // 若正在移动，用 moveFrom 否则用 curIndex
    int endCenter   = movePlaying ? moveTo   : curIndex;

    /* ================ 8) 计算 posX（仅为窗口范围内的项计算并绘制） ================ */
    int winMin = endCenter - VISIBLE_WINDOW;
    int winMax = endCenter + VISIBLE_WINDOW;
    if (winMin < 0) winMin = 0;
    if (winMax >= totalItems) winMax = totalItems - 1;

    int baseIdx = winMin;
    float posXs[(VISIBLE_WINDOW * 2) + 3]; // 存储窗口内项的横坐标

    // 逐项计算 startX,endX -> posX（合并 entry/move/idle 三种模式）
    for (int i = winMin; i <= winMax; ++i) {
        int startSlot = i - startCenter; // 起始相对槽
        int endSlot   = i - endCenter;   // 目标相对槽
        float startX = (float)centerX + (float)startSlot * (float)SLOT_DX;
        float endX   = (float)centerX + (float)endSlot   * (float)SLOT_DX;
        float posX;

        if (entryPlaying) {
            // 进场 -> 所有项从右侧 (endX + ENTRY_OFFSET) 插值到 endX
            float entryStartX = endX + (float)ENTRY_OFFSET;
            posX = entryStartX + (endX - entryStartX) * t_entry;
        } else if (movePlaying) {
            // 翻页 -> startX -> endX 插值
            posX = startX + (endX - startX) * t_move;
        } else {
            // 静止 -> endX
            posX = endX;
        }
        posXs[i - baseIdx] = posX;
    }

    /* ================ 9) 绘制图标并确定视觉中心（离屏幕中心最近的项） ================ */
    int visualCenterId = -1;
    float bestDist = 1e9f;

    for (int i = winMin; i <= winMax; ++i) {
        float posX = posXs[i - baseIdx];
        // 若 posX 超出远处可跳过绘制（优化），阈值保持宽松以允许半图显示
        if (posX < - (float)ICON_W * 2 || posX > (float)SCREEN_WIDTH + (float)ICON_W * 2) {
            continue;
        }
        // 计算绘制左上角坐标
        int16_t drawX = (int16_t)(posX - (ICON_W / 2.0f) + 0.5f);
        int16_t drawY = (int16_t)(ICON_CENTER_Y - (ICON_H / 2.0f) + 0.5f);
        // 绘制图标或占位框
        ArkUIItem_t *it = ArkUI_FindItemById(page, i);
        if (it != NULL && it->icon) {
            ArkUIDisplayBMP(drawX, drawY, ICON_W, ICON_H, it->icon);
        } else {
            ArkUIDrawRFrame(drawX, drawY, ICON_W, ICON_H, 1, 4);
        }
        // 记录最接近 center 的项为视觉中心（视觉上处于正中）
        float d = fabsf(posX - (float)centerX);
        if (d < bestDist) { bestDist = d; visualCenterId = i; }
    }

    /* ================ 10) 光标（Cursor库）：跨页面插值 + 吸附（还原手感） ================ */
    // 目标矩形（居中高亮中间图标）
    const uint16_t dt_ms = (uint16_t)(g_arkui_tick_ms ? g_arkui_tick_ms : 1);
    const float targetW = (float)(ICON_W + 3);
    const float targetH = (float)(ICON_H + 3);
    const float targetX = (float)(centerX) - targetW * 0.5f;
    const float targetY = (float)(ICON_CENTER_Y) - targetH * 0.5f;

    // case A：从 item/icon 上一级返回到 icon page（但不要求进场动画）
    // 这时候 layer/pageId 可能没变，但 Cursor.Object 的 pageId 一定变了。
    static uint8_t cursorReturnInited = 0;
    if (!entryPlaying && !movePlaying && justReturnedToIcon && !cursorReturnInited)
    {
        cursorReturnInited = 1;
        // 从当前 Cursor 位置平滑移动到 icon 的中心高亮框
        uint16_t dur_ms = (uint16_t)(MOVE_FRAMES * dt_ms);
        if (dur_ms == 0) dur_ms = 1;
        Cursor.Goto(iconObject, targetX, targetY, targetW, targetH, dur_ms, (uint8_t)CURSOR_RADIUS);
    }
    if (!justReturnedToIcon)
    {
        cursorReturnInited = 0;
    }

    // case B：icon 翻页时的“吸附”手感（原来 XOR 光标有个小幅左右弹性偏移）
    // 注意：movePlaying 在 t_move 计算时可能“本帧结束”从 1 变 0，因此这里用 prevMovePlaying 捕捉边沿。
    static uint8_t prevMovePlaying = 0;
    if (!entryPlaying && movePlaying)
    {
        float adh = (t_move <= 0.5f) ? (t_move * 2.0f) : ((1.0f - t_move) * 2.0f);
        float offset = (float)moveDir * (float)CURSOR_ADHERE * adh;
        Cursor.Set_Now(targetX + offset, targetY, targetW, targetH, (uint8_t)CURSOR_RADIUS);
    }
    else
    {
        // 正常：跑插值（entry / return）
        Cursor.Update(dt_ms);
    }

    // move 结束的那一帧，把光标强制归位到正中（避免残留偏移）
    if (!entryPlaying && prevMovePlaying && !movePlaying)
    {
        Cursor.Set_Now(targetX, targetY, targetW, targetH, (uint8_t)CURSOR_RADIUS);
    }
    prevMovePlaying = (uint8_t)movePlaying;

    Cursor.Draw_Cursor();

    /* ================ 11) 标题状态机：仅按明确触发推进（修复跳回问题） ================ */
    // 说明要点：
    //  - 标题过渡只在两种情况下启动：页面 entry（titleFrom=-1,titleTo=curIndex）或用户触发 move（titleFrom=titleCurrent,titleTo=moveTo）
    //  - 过渡完成后才设置 titleCurrent = titleTo（避免中途被覆盖）
    //  - 绝不在其它地方（例如 visualCenterId 变化）再次触发过渡 —— 防止状态冲突

    // 11.a 进展标题过渡（若正在进行）
    if (titlePlaying) {
        // 增加帧计数（防止 titleDur 为 0）
        titleTick++;
        if (titleTick > titleDur) titleTick = titleDur;

        // 计算标题过渡进度 t_title (easeOutCubic)
        float tt = (titleDur > 0) ? ((float)titleTick / (float)titleDur) : 1.0f;
        float p = tt - 1.0f;
        float t_title = (p * p * p + 1.0f);

        // 11.a.i 如果 titleFrom == -1 => 没有旧标题（入场场景），仅绘制 titleTo 从下滑入
        if (titleFrom == -1) {
            if (titleTo >= 0 && titleTo < totalItems) {
                ArkUIItem_t *itn = ArkUI_FindItemById(page, titleTo);
                if (itn != NULL && itn->title != NULL) {
                    // 计算新标题 Y：从 TITLE_Y + OFF -> TITLE_Y
                    int OFF = (SCREEN_HEIGHT - TITLE_Y) + FONT_HEIGHT + TITLE_EXTRA_PAD;
                    float startY = (float)TITLE_Y + (float)OFF;
                    float curY = startY + ((float)TITLE_Y - startY) * t_title;
                    int textW = (int)(strlen(itn->title) * FONT_WIDTH);
                    int tx = (int)((float)centerX - (float)textW / 2.0f + 0.5f);
                    ArkUIDisplayStr(tx, (int)(curY + 0.5f), itn->title);
                }
            }
        } else {
            // 11.a.ii 常规过渡：绘制旧标题向下滑出 & 新标题从下方滑入
            // 旧标题： TITLE_Y -> TITLE_Y + OFF  （随 t_title）
            if (titleFrom >= 0 && titleFrom < totalItems) {
                ArkUIItem_t *ito = ArkUI_FindItemById(page, titleFrom);
                if (ito != NULL && ito->title != NULL) {
                    int OFF = (SCREEN_HEIGHT - TITLE_Y) + FONT_HEIGHT + TITLE_EXTRA_PAD;
                    float oldY = (float)TITLE_Y + (float)OFF * t_title; // 0 -> OFF
                    int textW = (int)(strlen(ito->title) * FONT_WIDTH);
                    int tx = (int)((float)centerX - (float)textW / 2.0f + 0.5f);
                    ArkUIDisplayStr(tx, (int)(oldY + 0.5f), ito->title);
                }
            }
            // 新标题： TITLE_Y + OFF -> TITLE_Y  （随 t_title）
            if (titleTo >= 0 && titleTo < totalItems) {
                ArkUIItem_t *itn = ArkUI_FindItemById(page, titleTo);
                if (itn != NULL && itn->title != NULL) {
                    int OFF = (SCREEN_HEIGHT - TITLE_Y) + FONT_HEIGHT + TITLE_EXTRA_PAD;
                    float newY = (float)TITLE_Y + (float)OFF * (1.0f - t_title); // OFF -> 0
                    int textW = (int)(strlen(itn->title) * FONT_WIDTH);
                    int tx = (int)((float)centerX - (float)textW / 2.0f + 0.5f);
                    ArkUIDisplayStr(tx, (int)(newY + 0.5f), itn->title);
                }
            }
        }

        // 如果标题过渡已完成，写回 titleCurrent 并清理
        if (titleTick >= titleDur) {
            titlePlaying = 0;
            titleCurrent = titleTo;   // 将当前标题设置为目标
            titleFrom = -1;
            titleTo = -1;
            titleTick = 0;
            titleDur = 0;
        }
    }
    // 11.b 非过渡态：仅绘制 titleCurrent（若存在）；**不**基于 visualCenterId 启动过渡（已移除）
    else {
        // 若 titleCurrent 尚未初始化，设置为 curIndex（稳妥默认）
        if (titleCurrent < 0) {
            titleCurrent = curIndex;
        }
        // 绘制当前标题（如果有效）
        if (titleCurrent >= 0 && titleCurrent < totalItems) {
            ArkUIItem_t *itc = ArkUI_FindItemById(page, titleCurrent);
            if (itc != NULL && itc->title != NULL) {
                int textW = (int)(strlen(itc->title) * FONT_WIDTH);
                int tx = centerX - textW / 2;
                ArkUIDisplayStr(tx, TITLE_Y, itc->title);
            }
        }
    }

    /* ================ 12) 点击响应（仅在无动画时响应） ================ */
    if (!entryPlaying && !movePlaying && opnClick) {
        ArkUIItem_t *it = ArkUI_FindItemById(page, curIndex);
        if (it != NULL) {
            ArkUIItemOperationResponse(page, it, (uint8_t *)&curIndex);
            itemIndex[layer] = (uint8_t)curIndex;
        }
    }

    /* ================ 13) 保证无动画时写回索引一致性 ================ */
    if (!entryPlaying && !movePlaying) {
        itemIndex[layer] = (uint8_t)curIndex;
    }

    /* ================  完成 ================ */
}

/**
 * @brief 为了IconPageEvent每次进入时都能播进场动画，专门写的item事件来达到跳页和g_iconNeedEntryAnim置1
 * @param item 图标页面的指针
 * @retval None
*/
void ArkUIEventEnterIconPage(ArkUIItem_t *item)
{
    // 1) 告诉 IconPageEvent：下次显示时强制播进场动画
    g_iconNeedEntryAnim = 1;
    // 继承当前 Cursor 矩形作为 Icon Page 光标插值起点（实现 item->icon 光标连贯过渡）
    g_icon_cursor_from = Cursor.Get_Now_Cursor();
    g_icon_cursor_from_valid = 1;

    g_icon_from_x = FONT_WIDTH * 2;
    g_icon_from_y = item->lineId * ITEM_HEIGHT + ITEM_HEIGHT / 2;
    // 2) 手动执行“JUMP_PAGE 的入栈跳转”
    //    按你 ArkUI 的逻辑：进入下一层
    layer++;
    pageIndex[layer] = item->pageId;   // 目标页（你在 register 里写的是 page8.id）
    itemIndex[layer] = 0;             // 新页默认光标（你也可以设成 1，看你系统 id 习惯）

    // 3) 退出 function 模式 + 播转场（如果你有）
    functionIsRunning = false;
    ArkUITransitionAnim();
}

/*---------------------------------IconPageEvent相关逻辑-END--------------------------------------------------*/

/*---------------------------------MiniGame: Catch Brick相关逻辑-BEGIN----------------------------------------*/

/**
 * 操作：
 *  - Up/Down：左右移动
 *  - Click：暂停/继续（GameOver 时 Click=重开）
 *  - Exit：退出回到图标页
 *
 * 玩法：
 *  1) Lives(HP)=3：漏接不立即死，扣血到 0 才 GameOver
 *  2) 连击 Combo：连续接住会加分倍率；一段时间没接就断
 *  3) FEVER：连击达到阈值触发一段时间 x2 得分（有爽点）
 *  4) 特殊砖块：金币/宽板/慢速/炸弹/加命/护盾（长相明显区分）
 */

paramType gGame_FallSpeed   = 0.5;   // px/帧
paramType gGame_SpawnRate   = 0.08;  // 0~1
paramType gGame_PaddleWidth = 22;    // px
paramType gGame_MaxBricks   = 2;     // 1~10

static uint32_t _arkui_rng = 0x12345678u;
static uint32_t ArkUI_RandU32(void)
{
    _arkui_rng = _arkui_rng * 1664525u + 1013904223u;
    return _arkui_rng;
}
static int ArkUI_ClampInt(int v, int lo, int hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

typedef enum {
    BR_NORMAL = 0,
    BR_GOLD,     // $
    BR_WIDE,     // W
    BR_SLOW,     // S
    BR_BOMB,     // !
    BR_LIFE,     // +
    BR_SHIELD    // D
} BrickType_t;

static char CatchBrick_TypeChar(uint8_t t)
{
    switch ((BrickType_t)t) {
        default:        return 'o';
        case BR_GOLD:   return '$';
        case BR_BOMB:   return '!';
        case BR_WIDE:   return 'W';
        case BR_SLOW:   return 'S';
        case BR_LIFE:   return '+';
        case BR_SHIELD: return 'D';
    }
}

void ArkUIEventCatchBrick(ArkUIItem_t *item)
{
    (void)item;

    /* 砖块用 ASCII 显示：尺寸按字体来 */
#ifndef FONT_WIDTH
#define FONT_WIDTH 6
#endif
#ifndef FONT_HEIGHT
#define FONT_HEIGHT 8
#endif

    const int BRICK_W = FONT_WIDTH;
    const int BRICK_H = FONT_HEIGHT;

    /* Paddle 也做成 ASCII：一行 '=' */
    const int PADDLE_Y = SCREEN_HEIGHT - 1;

    typedef struct {
        int x;          // 像素坐标（但始终按字符格对齐）
        float y;        // 像素坐标
        uint8_t alive;
        uint8_t type;   // BrickType_t
    } Brick_t;

    /* ---------- 游戏状态（跨帧） ---------- */
    static uint8_t inited = 0;
    static Brick_t bricks[10];

    static uint32_t score = 0;
    static uint8_t gameOver = 0;
    static uint8_t paused = 0;

    static int lives = 3;

    /* Buff */
    static int wideTick = 0;     // 宽板
    static int slowTick = 0;     // 减速
    static int shieldTick = 0;   // 盾（漏接免一次）

    /* 让人能玩久：连击 + FEVER */
    static int combo = 0;
    static int comboTick = 0;    // 连击窗口（帧）
    static int feverTick = 0;    // FEVER（帧）

    /* Paddle（像素） */
    static int paddleX = 0;

    /* ---------- Exit：退出游戏 ---------- */
    if (opnExit) {
        // // 退出回到 icon page 时：强制播一次进场动画（满足“event 回退到 icon 也要播进场”）
        // g_iconNeedEntryAnim = 1;
        // // 继承当前 Cursor 矩形作为 icon 光标插值起点（如果游戏里没画光标，也不影响）
        // g_icon_cursor_from = Cursor.Get_Now_Cursor();
        // g_icon_cursor_from_valid = 1;

        functionIsRunning = false;
        opnExit = false;

        inited = 0; // 下次进来重开游戏
        opnUp = opnDown = opnClick = false; // 清除按键状态

        return;
    }

    /* ---------- 首次初始化 ---------- */
    if (!inited) {
        inited = 1;
        for (int i = 0; i < 10; ++i) bricks[i].alive = 0;

        score = 0;
        gameOver = 0;
        paused = 0;

        lives = 3;
        wideTick = slowTick = shieldTick = 0;
        combo = 0; comboTick = 0;
        feverTick = 0;

        paddleX = (SCREEN_WIDTH / 2 / FONT_WIDTH) * FONT_WIDTH; // 居中对齐到字符格
    }

    /* ---------- Click：暂停/继续；GameOver 时重开 ---------- */
    if (opnClick) {
        if (gameOver) {
            for (int i = 0; i < 10; ++i) bricks[i].alive = 0;

            score = 0;
            gameOver = 0;
            paused = 0;

            lives = 3;
            wideTick = slowTick = shieldTick = 0;
            combo = 0; comboTick = 0;
            feverTick = 0;

            paddleX = (SCREEN_WIDTH / 2 / FONT_WIDTH) * FONT_WIDTH;
        } else {
            paused = (uint8_t)!paused;
        }
        opnClick = false;
    }

    /* ---------- 参数读取 + 容错 ---------- */
    int maxBricks = ArkUI_ClampInt((int)gGame_MaxBricks, 1, 10);

    float fallBase = (float)gGame_FallSpeed;
    if (fallBase < 0.2f) fallBase = 0.2f;
    if (fallBase > 6.0f) fallBase = 6.0f;

    float spawnBase = (float)gGame_SpawnRate;
    if (spawnBase < 0.0f) spawnBase = 0.0f;
    if (spawnBase > 1.0f) spawnBase = 1.0f;

    int paddleW0 = ArkUI_ClampInt((int)gGame_PaddleWidth, 10, SCREEN_WIDTH - 2);
    gGame_PaddleWidth = (paramType)paddleW0;

    /* paddleW：宽板 buff 生效 */
    int paddleW = paddleW0;
    if (wideTick > 0) paddleW = ArkUI_ClampInt(paddleW0 + 18, 10, SCREEN_WIDTH - 2);

    /* ASCII paddle：换算成“字符数量” */
    int paddleChars = paddleW / FONT_WIDTH;
    if (paddleChars < 2) paddleChars = 2;
    paddleW = paddleChars * FONT_WIDTH;

    /* 难度递增：慢一点才玩得久 */
    float fall = fallBase + 0.10f * (float)(score / 30);  // 每30分+0.10
    if (feverTick > 0) fall *= 1.08f;
    if (slowTick > 0)  fall *= 0.65f;

    float spawn = spawnBase;
    if (feverTick > 0) {
        spawn += 0.05f;
        if (spawn > 1.0f) spawn = 1.0f;
    }

    /* ---------- 输入（暂停/结束不动） ---------- */
    if (!gameOver && !paused) {
        int stepChars = 1;
        if (opnUp)   paddleX -= stepChars * FONT_WIDTH;
        if (opnDown) paddleX += stepChars * FONT_WIDTH;

        /* 对齐字符格 + 边界 */
        int minX = 1;
        int maxX = SCREEN_WIDTH - paddleW - 1;
        paddleX = ArkUI_ClampInt(paddleX, minX, maxX);
        paddleX = (paddleX / FONT_WIDTH) * FONT_WIDTH;
    }

    /* ---------- 生成砖块（暂停/结束不生成） ---------- */
    if (!gameOver && !paused) {
        int aliveCnt = 0;
        for (int i = 0; i < 10; ++i) if (bricks[i].alive) aliveCnt++;

        if (aliveCnt < maxBricks) {
            uint32_t r  = ArkUI_RandU32() & 0xFFFFu;
            uint32_t th = (uint32_t)(spawn * 65535.0f);

            if (r < th) {
                for (int i = 0; i < 10; ++i) {
                    if (!bricks[i].alive) {

                        /* x 对齐到字符格 */
                        int cols = (SCREEN_WIDTH - 2) / FONT_WIDTH;
                        if (cols < 1) cols = 1;
                        int col = (int)(ArkUI_RandU32() % (uint32_t)cols);
                        int x = 1 + col * FONT_WIDTH;

                        /* 类型概率（保证能玩久：稀但关键） */
                        uint32_t rr = ArkUI_RandU32() & 0xFFu; // 0..255
                        uint8_t type = BR_NORMAL;

                        if      (rr < 10) type = BR_GOLD;     // ~4%
                        else if (rr < 18) type = BR_WIDE;     // ~3%
                        else if (rr < 26) type = BR_SLOW;     // ~3%
                        else if (rr < 32) type = BR_SHIELD;   // ~2%
                        else if (rr < 36) type = BR_LIFE;     // ~1.6%
                        else if (rr < 50) type = BR_BOMB;     // ~5.5%

                        bricks[i].x = x;
                        bricks[i].y = 0.0f;
                        bricks[i].alive = 1;
                        bricks[i].type = type;
                        break;
                    }
                }
            }
        }
    }

    /* ---------- 更新世界（暂停/结束不更新） ---------- */
    if (!gameOver && !paused) {
        /* 连击窗口：大约 60 帧（按你帧率调） */
        if (comboTick > 0) comboTick--;
        if (comboTick == 0) combo = 0;

        for (int i = 0; i < 10; ++i) {
            if (!bricks[i].alive) continue;

            bricks[i].y += fall;

            if (bricks[i].y >= (float)(PADDLE_Y - BRICK_H)) {
                int bx0 = bricks[i].x;
                int bx1 = bricks[i].x + BRICK_W;
                int px0 = paddleX;
                int px1 = paddleX + paddleW;

                int hit = !(bx1 < px0 || bx0 > px1);

                if (hit) {
                    /* 命中：连击续命 */
                    combo++;
                    comboTick = 60;

                    /* FEVER：连击到 12 触发（约 5 秒，按帧率调整） */
                    if (combo >= 12 && feverTick == 0) {
                        feverTick = 250;
                        combo = 0;
                        comboTick = 0;
                    }

                    /* 倍率：FEVER x2；连击每 5 段 +1（最多再+2） */
                    int mult = (feverTick > 0) ? 2 : 1;
                    int cbonus = combo / 5;
                    if (cbonus > 2) cbonus = 2;
                    mult += cbonus;

                    /* 结算 */
                    switch ((BrickType_t)bricks[i].type) {
                        case BR_GOLD:
                            score += (uint32_t)(5 * mult);
                            break;
                        case BR_WIDE:
                            score += (uint32_t)(2 * mult);
                            wideTick = 240;
                            break;
                        case BR_SLOW:
                            score += (uint32_t)(2 * mult);
                            slowTick = 240;
                            break;
                        case BR_SHIELD:
                            score += (uint32_t)(2 * mult);
                            shieldTick = 240;
                            break;
                        case BR_LIFE:
                            score += (uint32_t)(1 * mult);
                            if (lives < 5) lives++;
                            break;
                        case BR_BOMB:
                            /* 炸弹：扣命 + 断连击（风险） */
                            if (lives > 0) lives--;
                            combo = 0; comboTick = 0;
                            break;
                        default:
                            score += (uint32_t)(1 * mult);
                            break;
                    }

                    bricks[i].alive = 0;
                    if (lives <= 0) gameOver = 1;
                } else {
                    /* 漏接：有盾免一次，否则扣命，并断连击 */
                    bricks[i].alive = 0;
                    combo = 0; comboTick = 0;

                    if (shieldTick > 0) {
                        shieldTick = 0;
                    } else {
                        if (lives > 0) lives--;
                        if (lives <= 0) gameOver = 1;
                    }
                }
            }
        }

        /* buff tick */
        if (wideTick > 0) wideTick--;
        if (slowTick > 0) slowTick--;
        if (shieldTick > 0) shieldTick--;
        if (feverTick > 0) feverTick--;
    }

    /* ---------- 绘制 ---------- */
    ArkUIClearBuffer();

    /* 边框 */
    ArkUIDrawRFrame(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 4);

    /* HUD */
    ArkUIPrintf(3,  2, "SC:%lu", (unsigned long)score);
    ArkUIPrintf(64, 2, "HP:%d", lives);

    /* 状态 */
    if (wideTick   > 0) ArkUIDisplayStr(2,  10, "W");
    if (slowTick   > 0) ArkUIDisplayStr(12, 10, "S");
    if (shieldTick > 0) ArkUIDisplayStr(22, 10, "D");
    if (feverTick  > 0) ArkUIDisplayStr(32, 10, "F");
    if (!paused && !gameOver && combo >= 5) ArkUIPrintf(50, 10, "C:%d", combo);

    /* 砖块（ASCII 单字符） */
    for (int i = 0; i < 10; ++i) {
        if (!bricks[i].alive) continue;

        int by = (int)(bricks[i].y + 0.5f);
        if (by < 0) by = 0;
        if (by > SCREEN_HEIGHT - BRICK_H) by = SCREEN_HEIGHT - BRICK_H;

        /* 对齐到字符格 */
        int cx = (bricks[i].x / FONT_WIDTH) * FONT_WIDTH;
        int cy = by;

        char ch = CatchBrick_TypeChar(bricks[i].type);
        ArkUIPrintf(cx, cy, "%c", ch);
    }

    /* Paddle：一排 '='（ASCII 风格） */
    {
        int y = (PADDLE_Y / FONT_HEIGHT) * FONT_HEIGHT;
        int x0 = (paddleX / FONT_WIDTH) * FONT_WIDTH;
        for (int k = 0; k < paddleChars; ++k) {
            ArkUIPrintf(x0 + k * FONT_WIDTH, y, "=");
        }
    }

    /* Overlay 提示 */
    if (gameOver) {
        // 实心遮罩
        ArkUIDrawRBox(10, 26, SCREEN_WIDTH - 20, 30, 1, 4);

        // 反色文字
        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        ArkUIDisplayStr(21, 34, (char*)"GAME OVER");
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

    } else if (paused) {
        ArkUIDrawRBox(12, 26, SCREEN_WIDTH - 24, 30, 1, 4);

        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        ArkUIDisplayStr(30, 34, (char*)"PAUSED");
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

    } else if (feverTick > 0) {
        ArkUIDrawRBox(16, 26, SCREEN_WIDTH - 32, 18, 1, 4);

        ArkUISetDrawColor(OLED_DRAWMODE_XOR);
        ArkUIDisplayStr(21, 32, (char*)"FEVER x2!");
        ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

    }
    ArkUISendBuffer();

    /* 清移动键 */
    opnUp = opnDown = false;
}

/*---------------------------------MiniGame: Catch Brick相关逻辑-END----------------------------------------*/

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
