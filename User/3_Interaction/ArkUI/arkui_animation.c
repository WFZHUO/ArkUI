/**
 * @file arkui_animation.c
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief ArkUI动画函数
 * @version 1.0
 * @date 2026-1-26
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/
#include "arkui_animation.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/

/* Function definitions -------------------------------------------------------*/

/**
 * @brief 屏幕渐变消失至0,阻塞函数
 */
void ArkUITransitionAnim()
{
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 0; i < SCREEN_WIDTH + 1; i += 2)
        {
            ArkUIDrawDot(i, j, 0);
        }
    }
    ArkUIDelay_ms(TRANSITION_TIME / 4);
    ArkUISendBuffer();
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            ArkUIDrawDot(i, j, 0);
        }
    }
    ArkUIDelay_ms(TRANSITION_TIME / 4);
    ArkUISendBuffer();
    for (int j = 0; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            ArkUIDrawDot(i, j, 0);
        }
    }
    ArkUIDelay_ms(TRANSITION_TIME / 4);
    ArkUISendBuffer();
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            ArkUIDrawDot(i - 1, j - 1, 0);
        }
    }
    ArkUIDelay_ms(TRANSITION_TIME / 4);
    ArkUISendBuffer();
}

/**
 * @brief 屏幕渐变消失至1/3,阻塞函数
 */
void ArkUIBackgroundBlur()
{
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 0; i < SCREEN_WIDTH + 1; i += 2)
        {
            ArkUIDrawDot(i, j, 0);
        }
    }
    ArkUIDelay_ms(TRANSITION_TIME / 3);
    ArkUISendBuffer();
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            ArkUIDrawDot(i, j, 0);
        }
    }
    ArkUIDelay_ms(TRANSITION_TIME / 3);
    ArkUISendBuffer();
    for (int j = 0; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            ArkUIDrawDot(i, j, 0);
        }
    }
    ArkUIDelay_ms(TRANSITION_TIME / 3);
    ArkUISendBuffer();
}

/**
 * @brief 绘制消息框，阻塞函数
 * @param msg 消息内容
 */
void ArkUIDrawMsgBox(const char *msg)
{
    if (!msg) return;

    // ===== 布局参数（左上角悬浮气泡）=====
    const uint8_t marginL = 3;     // 左边距
    const uint8_t marginT = 3;     // 上边距
    const uint8_t marginR = 3;     // 右安全边界（用于限制最大宽度）
    const uint8_t marginB = 3;     // 下安全边界（用于限制最大高度）

    const uint8_t padX    = 4;     // 文字左右内边距
    const uint8_t padY    = 3;     // 文字上下内边距
    const uint8_t lineGap = 2;     // 行距（像素）
    const uint8_t lineH   = (uint8_t)(FONT_HEIGHT + lineGap);

    // ===== 最大内容区（不越界）=====
    // 内容宽/高 = 屏幕安全区减去内边距
    int16_t maxContentW = (int16_t)(SCREEN_WIDTH  - marginL - marginR - 2 * padX);
    int16_t maxContentH = (int16_t)(SCREEN_HEIGHT - marginT - marginB - 2 * padY);
    if (maxContentW < FONT_WIDTH)  maxContentW = FONT_WIDTH;
    if (maxContentH < FONT_HEIGHT) maxContentH = FONT_HEIGHT;

    // 每行最多字符数、最多行数
    uint16_t charsPerLine = (uint16_t)(maxContentW / FONT_WIDTH);
    if (charsPerLine == 0) charsPerLine = 1;

    uint16_t maxLines = (uint16_t)(maxContentH / lineH);
    if (maxLines == 0) maxLines = 1;

    // ===== 预排版：统计能显示的行，并判断是否需要省略号 =====
    const char *p = msg;
    uint16_t totalNeededLines = 0;
    while (*p) {
        uint16_t taken = 0;
        while (*p && *p != '\n' && taken < charsPerLine) { ++p; ++taken; }
        totalNeededLines++;
        if (*p == '\n') ++p;
    }

    // 实际要画的行数
    bool truncated = (totalNeededLines > maxLines);
    uint16_t drawLines = truncated ? maxLines : totalNeededLines;

    // ===== 二次排版：逐行取出要显示的内容，顺便计算气泡宽度 =====
    // 说明：为保证“宽度自适应且不越界”，气泡宽度取本次要绘制的各行中“最长那一行”的像素宽度
    uint16_t longestLineChars = 0;

    // 为嵌入式考虑，使用小栈缓冲；96×96、6×8 字体一行最多≈14字，这里给32足够
    char lineBuf[32];

    // 需要先走一遍，才能得到最终的最长行宽；因此用一个轻量的游标扫描
    const char *q = msg;
    for (uint16_t ln = 0; ln < drawLines; ++ln)
    {
        uint16_t taken = 0;
        while (*q && *q != '\n' && taken < charsPerLine) { ++q; ++taken; }
        if (*q == '\n') ++q;

        // 如果是最后一可见行且后面还有内容 → 需要为 "..." 预留空间
        if (ln == drawLines - 1) {
            if (truncated) {
                if (charsPerLine >= 3) {
                    if (taken > charsPerLine - 3) taken = (uint16_t)(charsPerLine - 3);
                    longestLineChars = (uint16_t)((taken + 3) > longestLineChars ? (taken + 3) : longestLineChars);
                } else {
                    // 极端：一行容纳不到3字符
                    longestLineChars = 1;
                }
            } else {
                if (taken > longestLineChars) longestLineChars = taken;
            }
        } else {
            if (taken > longestLineChars) longestLineChars = taken;
        }
    }

    // 兼容空字符串：至少给一个最小宽度
    if (longestLineChars == 0) longestLineChars = 1;

    // ===== 气泡最终尺寸（宽度随内容自适应，但不超过最大内容区）=====
    uint16_t contentW = (uint16_t)(longestLineChars * FONT_WIDTH);
    if (contentW > (uint16_t)maxContentW) contentW = (uint16_t)maxContentW;

    uint16_t contentH = (uint16_t)(drawLines * lineH);

    uint16_t boxW = (uint16_t)(contentW + 2 * padX);
    uint16_t boxH = (uint16_t)(contentH + 2 * padY);

    // 位置锚在左上角
    int16_t x = (int16_t)marginL;
    int16_t y = (int16_t)marginT;

    // 圆角半径：尽量圆润，同时保证合法
    uint8_t r;
    {
        // 6 比较圆润；但不能超过宽/高的一半-1
        uint16_t rCandidate = 6;
        uint16_t rMaxW = (boxW  > 2) ? (boxW  - 1) / 2 : 1;
        uint16_t rMaxH = (boxH  > 2) ? (boxH  - 1) / 2 : 1;
        uint16_t rMax  = (rMaxW < rMaxH ? rMaxW : rMaxH);
        if (rCandidate > rMax) rCandidate = rMax;
        r = (uint8_t)rCandidate;
        if (r == 0) r = 1;
    }

    // ===== 绘制 =====
    ArkUIBackgroundBlur();                        // 背景轻微虚化（与你现有风格一致）
    ArkUIDrawRBox(x, y, boxW, boxH, 1, r);        // 只画一个实心圆角矩形（无外框）

    ArkUISetDrawColor(OLED_DRAWMODE_XOR);         // 文本用 XOR，保证可读性

    // 逐行真正绘制文本
    p = msg;
    for (uint16_t ln = 0; ln < drawLines; ++ln)
    {
        // 取本行文本
        uint16_t taken = 0;
        while (*p && *p != '\n' && taken < charsPerLine && taken < sizeof(lineBuf) - 1) {
            lineBuf[taken++] = *p++;
        }
        if (*p == '\n') ++p;

        // 末行截断：追加省略号
        if (ln == drawLines - 1) {
            bool hasMore = (*p != '\0'); // 后面还有内容没显示
            if (hasMore) {
                if (charsPerLine >= 3) {
                    if (taken > charsPerLine - 3) taken = (uint16_t)(charsPerLine - 3);
                    if (taken > sizeof(lineBuf) - 3 - 1) taken = (uint16_t)(sizeof(lineBuf) - 3 - 1);
                    lineBuf[taken++] = '.';
                    lineBuf[taken++] = '.';
                    lineBuf[taken++] = '.';
                } else {
                    // 极端：只能放一个点
                    taken = 1;
                    lineBuf[0] = '.';
                }
            }
        }

        lineBuf[taken] = '\0';

        // 行基线位置（垂直方向做个轻微居中，让行距更自然）
        int16_t ty = (int16_t)(y + padY + ln * lineH + (lineH - FONT_HEIGHT) / 2);
        ArkUIDisplayStr((int16_t)(x + padX), ty, lineBuf);
    }

    ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
    ArkUISendBuffer();
}

/**
 * @brief 绘制一个进度条对话框，显示在屏幕中央。
 * @param item 进度条项指针
 */
void ArkUIDrawProgressBar(ArkUIItem_t *item)
{
    static int16_t x, y;   // 对话框位置
    static uint16_t width, height;  // 对话框尺寸
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1;  // 文本垂直居中偏移量
    static uint16_t barWidth;  // 进度条实际宽度

    ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);  // 使用正常绘制模式

    // Display information and draw box
    height = ITEM_HEIGHT * 2 + 2;  // 固定高度：2个列表项高度+边距
    // 根据标题长度计算宽度
    if (strlen(item->title) + 1 > 12)
        width = (strlen(item->title) + 1) * FONT_WIDTH + 7;
    else
        width = 12 * FONT_WIDTH + 7;
    // 确保宽度不小于屏幕宽度的2/3
    if (width < 2 * SCREEN_WIDTH / 3)
        width = 2 * SCREEN_WIDTH / 3;
    //计算对话框位置
    x = (SCREEN_WIDTH - width) / 2;   // 水平居中
    y = (SCREEN_HEIGHT - height) / 2;  // 垂直居中
    //计算进度条宽度
    barWidth = width - 6 * FONT_WIDTH - 10;// 减去右侧数值显示区域
    //绘制对话框框架
    ArkUIDrawFrame(x - 1, y - 1, width + 2, height + 2, 1);// 外框
    ArkUIDrawBox(x, y, width, height, 0);// 背景填充
    //显示标题
    ArkUIDisplayStr(x + 3, y + itemHeightOffset, item->title);// 标题文本
    ArkUIDisplayStr(x + 3 + strlen(item->title) * FONT_WIDTH, y + itemHeightOffset, ":");// 标题后的冒号
		float temp_Bar = (float)*item->param;
		if(temp_Bar < 0){
			temp_Bar = 0;
		}
		if(temp_Bar > 100){
			temp_Bar = 100;
		}
    //绘制进度条
    ArkUIDrawFrame(x + 3, y + ITEM_HEIGHT + itemHeightOffset, barWidth, FONT_HEIGHT, 1);// 进度条背景框
    ArkUIDrawBox(x + 5, y + ITEM_HEIGHT + itemHeightOffset + 2, temp_Bar/ 100 * (barWidth - 4),FONT_HEIGHT - 4, 1);// 进度条填充
    //显示进度数值
		if(temp_Bar < 10 && temp_Bar > 0){
			ArkUIPrintf(x + width - 5 * FONT_WIDTH - 4, y + ITEM_HEIGHT + itemHeightOffset,"%.2f%%",temp_Bar);
		}else if(temp_Bar < 100 && temp_Bar >= 10){
			ArkUIPrintf(x + width - 6 * FONT_WIDTH - 5, y + ITEM_HEIGHT + itemHeightOffset,"%.2f%%",temp_Bar);
		}else if(temp_Bar == 100){
			ArkUIPrintf(x + width - 4 * FONT_WIDTH - 4, y + ITEM_HEIGHT + itemHeightOffset,"100%%");
		}else{
			ArkUIPrintf(x + width - 2 * FONT_WIDTH - 4, y + ITEM_HEIGHT + itemHeightOffset,"0%%");
		}
		
    ArkUISendBuffer();
}

/**
 * @brief 画复选框，有选中非选中两种状态
 * @param x 复选框左上角x坐标
 * @param y 复选框左上角y坐标
 * @param size 复选框尺寸（宽度=高度）
 * @param offset 内部填充框与外框的间距（建议不超过尺寸的一半）
 * @param boolValue 是否选中
 * @param r 圆角半径（建议不超过尺寸的一半）
 */
void ArkUIDrawCheckbox(int16_t x, int16_t y, uint16_t size, uint8_t offset, bool boolValue, uint8_t r)
{
    // 1. 绘制复选框外框
    ArkUIDrawRFrame(x, y, size, size, 1, r);
    
    // 2. 仅当复选框被选中且参数有效时绘制内部填充
    if (boolValue)
    {
        // 参数校验和边界保护
        uint8_t actualOffset = offset;
        uint16_t innerSize = size;
        
        // 确保偏移量不超过尺寸的一半
        if (actualOffset > size / 2) {
            actualOffset = size / 2;
        }
        
        // 计算内部尺寸（至少为1像素）
        innerSize = size - 2 * actualOffset;
        if (innerSize < 1) {
            innerSize = 1;
            actualOffset = (size - 1) / 2;
        }
        
        // 绘制内部填充框
        ArkUIDrawRBox(
            x + actualOffset, 
            y + actualOffset, 
            innerSize, 
            innerSize, 
            1, 
            r
        );
    }
}

/**
 * @brief 在图标四周画四个直角（L 型）作为角标
 * @param iconX 图标左上角x坐标
 * @param iconY 图标左上角y坐标
 * @param w 图标宽度
 * @param h 图标高度
 * @param pad 角标相对于图标边缘的偏移（正数表示外扩）
 * @param cornerLen 直角的臂长
 * @param cornerThk 直角臂的厚度（像素）
 */
void ArkUI_DrawCornerBrackets(int16_t iconX, int16_t iconY,
                                     int16_t w, int16_t h,
                                     int16_t pad, int16_t cornerLen, int16_t cornerThk)
{
    if (cornerLen <= 0 || cornerThk <= 0) return;

    int16_t fx = iconX - pad;
    int16_t fy = iconY - pad;
    int16_t fw = w + pad * 2;
    int16_t fh = h + pad * 2;

    // 左上：横 + 竖
    ArkUIDrawRBox(fx, fy, cornerLen, cornerThk, 1, 1);
    ArkUIDrawRBox(fx, fy, cornerThk, cornerLen, 1, 1);

    // 右上
    ArkUIDrawRBox(fx + fw - cornerLen, fy, cornerLen, cornerThk, 1, 1);
    ArkUIDrawRBox(fx + fw - cornerThk, fy, cornerThk, cornerLen, 1, 1);

    // 左下
    ArkUIDrawRBox(fx, fy + fh - cornerThk, cornerLen, cornerThk, 1, 1);
    ArkUIDrawRBox(fx, fy + fh - cornerLen, cornerThk, cornerLen, 1, 1);

    // 右下
    ArkUIDrawRBox(fx + fw - cornerLen, fy + fh - cornerThk, cornerLen, cornerThk, 1, 1);
    ArkUIDrawRBox(fx + fw - cornerThk, fy + fh - cornerLen, cornerThk, cornerLen, 1, 1);
}


/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
