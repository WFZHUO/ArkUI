/**
 * @file arkui.c
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief ArkUI主逻辑
 * @version 1.0
 * @date 2026-1-26
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "arkui.h"
#include "arkui_animation.h"
#include "arkui_event.h"
#include "arkui_register.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/**
 * @brief ArkUI页面链表头指针
 *        1. pageHead: 页面链表头指针
 *        2. pageTail: 页面链表尾指针
 */
ArkUIPage_t *pageHead = NULL, *pageTail = NULL;

/**
 * @brief ArkUI页面索引数组
 *        1. pageIndex: 页面索引数组,这一层是哪个页面。
 *        2. itemIndex: 项索引数组,这一层页面的光标在哪个 item。
 *        3. layer: 当前所在层
 */
uint8_t pageIndex[MAX_LAYER] = {0};
uint8_t itemIndex[MAX_LAYER] = {0};
uint8_t layer = 0;                  

/**
 * @brief ArkUI操作逻辑标志
 *        1. opnUp: 上键操作标志
 *        2. opnDown: 下键操作标志
 *        3. opnClick: 确认键操作标志
 *        4. opnExit: 退出键操作标志
 */
uint8_t opnUp,opnDown,opnClick,opnExit;

/**
 * @brief ArkUI 每次 tick 的周期（ms）,供 Custom Page / Cursor 等模块使用
 */
uint8_t g_arkui_tick_ms = 1;

/**
 * @brief ArkUI运行状态标志
 *        1. functionIsRunning: 函数是否正在运行
 */
bool functionIsRunning = false;

/**
 * @brief ArkUI列表循环标志
 *        1. listLoop: 是否循环显示列表,配置为false即不循环显示列表。
 */
bool listLoop = true;

/**
 * @brief ArkUI光标对象
 *        
 */
Class_ArkUI_Cursor Cursor;

/* Private function declarations ---------------------------------------------*/

static void ArkUIDisplayItem(ArkUIItem_t *item);
static void ArkUIGetItemPos(ArkUIPage_t *page, ArkUIItem_t *item, uint8_t index, uint8_t timer);
static void ArkUIDrawIndicator(ArkUIPage_t *page, uint8_t index, uint8_t timer, uint8_t status);

/* Function definitions -------------------------------------------------------*/

/**
 * @brief 初始化ArkUI
 * 
 */
void ArkUIInit(void)
{
    // Screen init
    ArkUIScreenInit();
    // Key init
    ArkUIKeyInit();
    //menu init
	ArkUI_RegisterMenu();
    // Cursor init
    Cursor.Init();

    // Display the welcome photo and info
    ArkUIDisplayBMP((SCREEN_WIDTH - WELOCOME_PICTURE_WIDTH) / 2, (SCREEN_HEIGHT - WELOCOME_PICTURE_HEIGHT) / 2, WELOCOME_PICTURE_WIDTH, WELOCOME_PICTURE_HEIGHT, Fonzo);
    //根据屏幕宽度显示不同的文字
    if (2 * SCREEN_WIDTH / 3 > (25 * FONT_WIDTH + 1))
    {
        ArkUIDisplayStr(SCREEN_WIDTH - 1 - 25 * FONT_WIDTH, SCREEN_HEIGHT - 1 - FONT_HEIGHT,"Powered by ArkUI(Fonzo)");
    }else if (SCREEN_WIDTH > (14 * FONT_WIDTH + 1))
    {
        ArkUIDisplayStr(SCREEN_WIDTH - 1 - 14 * FONT_WIDTH, SCREEN_HEIGHT - 1 - FONT_HEIGHT, "ArkUI(Fonzo)");
    }
    ArkUISendBuffer();
		
	Delay_ms(1000);
    ArkUITransitionAnim();
}

/**
 * @brief 处理ArkUI主循环
 * @param timer 定时器周期，单位：毫秒
 * 
 */
void ArkUI(uint8_t timer)
{
    //传入ArkUI每次 tick 的周期（ms）
    g_arkui_tick_ms = timer ? timer : 1;

    static uint8_t index = 0; //当前页面光标指向的item编号
    static uint8_t itemSum = 0; //当前页面的最大id
    static uint8_t lastLayer = 0xFF; //上一次所在层
    static uint8_t lastPageId = 0xFF; //上一次所在页面id

    //设置绘制模式为正常模式
    ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);

    //当层级变了或者页变了,需要同步index,展现出光标记忆效果
    if (lastLayer != layer || lastPageId != pageIndex[layer])
    {
        index = itemIndex[layer];
        lastLayer = layer;
        lastPageId = pageIndex[layer];
    }

    // Get current page by id
    ArkUIPage_t *page = pageHead;
    while (page->id != pageIndex[layer])
    {
        page = page->next;
    }

    //Quit UI to run function
    //If running function and hold the confirm button, quit the function
    if (functionIsRunning)
    {
        uint8_t cur = itemIndex[layer];     // 用“真实光标”，防止进Event自定义事件后index未及时更新
        uint8_t found = 0;

        //遍历当前页面的所有item来找到当前光标指向的item,然后执行其事件函数
        for (ArkUIItem_t *item = page->itemHead; item != NULL; item = item->next)
        {
            if (item->id != cur) continue;

            found = 1;

            if (item->Event) item->Event(item);
            else functionIsRunning = false; // 防呆：没有事件就别卡在 function 模式

            break;
        }

        if (!found) {
            functionIsRunning = false;      // 防呆：光标 id 在这个页不存在（切页/回退后常见）
        }

        // Clear the states of key to monitor next key action
        opnUp = opnDown = opnClick = opnExit = false;

        return;
    }

    ArkUIClearBuffer();

    // Custom page--------------------------------------------------------------------------------
    if (page->funcType == PAGE_CUSTOM)
    {
        page->Event(page);

        // Clear the states of key to monitor next key action
        opnUp = opnDown = opnClick = false;

        if (layer == 0)
        {
            opnExit = false;
            ArkUISendBuffer();
            return;
        }

        if (opnExit)
        {
            opnExit = false;
            pageIndex[layer] = 0;
            itemIndex[layer--] = 0;
            index = itemIndex[layer];
            ArkUITransitionAnim();
        }

        ArkUISendBuffer();
        return;
    }
    // -------------------------------------------------------------------------------------------

    // List page----------------------------------------------------------------------------------
    for (ArkUIItem_t *item = page->itemHead; item != NULL; item = item->next)
    {
        ArkUIGetItemPos(page, item, index, timer);
        ArkUIDisplayItem(item);
    }
    // Draw indicator and scroll bar
    ArkUIDrawIndicator(page, index, timer, 0);

    // Operation move reaction
    itemSum = page->itemTail->id;
    if (opnDown)
    {
        if (index < itemSum)
            index++;
        else if (listLoop)
            index = 0;
    }
    if (opnUp)
    {
        if (index > 0)
            index--;
        else if (listLoop)
            index = itemSum;
    }
    if (opnClick)
    {
        for (ArkUIItem_t *item = page->itemHead; item != NULL; item = item->next)
        {
            if (item->id != index)
            {
                continue;
            }

            ArkUIItemOperationResponse(page, item, &index);
            break;
        }
    }

    //更新当前光标指向的item编号
    itemIndex[layer] = index;

    // Clear the states of key to monitor next key action
    opnUp = opnDown = opnClick = false;

    if (layer == 0)
    {
        opnExit = false;
        ArkUISendBuffer();
        return;
    }
    if (opnExit)
    {
        opnExit = false;
        pageIndex[layer] = 0;
        itemIndex[layer--] = 0;
        index = itemIndex[layer];
        //这一页的item的位置都设为0,下次再进来这页的时候,所有item能表现出从上面滑下来的效果
        for (ArkUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
        {
            itemTmp->position = 0;
            itemTmp->posForCal = 0;
        }
        ArkUITransitionAnim();
    }
    // -------------------------------------------------------------------------------------------

    ArkUISendBuffer();

}

/** 
 * @brief 将按键事件标志转化为ArkUI操作逻辑标志
 * @note 每次 UI tick(在 main loop 中、调用 ArkUI() 之前)调用一次
 */
void ArkUIKeyActionMonitor(void)
{
    static uint8_t confirmHoldLatched = 0;  // 本次是否已触发过长按（用来吞掉松手click/up),长按后就吞掉长按后松手时的up

    // 为了增强代码鲁棒性,每个 UI tick 先清空一次性操作标志以防万一
    opnUp = opnDown = opnClick = opnExit = 0;

    if (keyUp.isPressed)   opnUp   = 1;
    if (keyDown.isPressed) opnDown = 1;

    // --- 仿鼠标确认键状态机 ---
    // 1) 按下：并不触发opnClick,松手时才触发,模仿电脑鼠标逻辑
    if (keyConfirm.isPressed) {
        confirmHoldLatched = 0;   // 新一轮按键从未长按
    }

    // 2) 长按：发出退出,且一次长按只能触发一次opnExit,下次再触发就要等到松手后的再按
    if (keyConfirm.isHold && !confirmHoldLatched) {
        opnExit = 1;
        confirmHoldLatched = 1;
    }

    // 3) 松手：只有在没有长按过的情况下，才视为一次点击
    if (keyConfirm.isUp) {
        if (!confirmHoldLatched) {
            opnClick = 1;
        }
        // 无论如何，结束本轮
        confirmHoldLatched = 0;
    }

    // 备注：如果 arkey 会让 isPressed 保持多周期，
    // 请在主循环里本函数调用后执行 ArkKeyClearFlags() 清掉按键瞬态标志
}

/**
 * @brief 添加一个ArkUI Item
 * @param page 指向ArkUIPage_t结构体的指针，用于指定要添加项目的页面
 * @param item 指向ArkUIItem_t结构体的指针，用于指定要添加的项目
 * @param _title 项目的标题，用于显示在页面上
 * @param func 项目的功能类型，用于指定项目的行为
 * @param args 指向ArkUIItemArgs_t结构体的指针，用于指定项目的参数
 * @note 定义参数的时候自觉点，无符整型就别让初值等于小数或则和负数；有符整型同理
 */
void ArkUIAddItem(ArkUIPage_t *page, ArkUIItem_t *item, char *_title,ArkUIItem_e func, const ArkUIItemArgs_t *args)
{
    /* 0) 清零，避免脏数据 */
    memset(item, 0, sizeof(*item));

    /* 1) 基本字段 */
    item->title   = _title;
    item->funcType = func;
    item->next = NULL;

    /* 2) 从 args 拷贝通用属性（与 page 类型无关） */
    if (args)
    {
        item->icon     = args->icon;
        item->msg      = args->msg;
        item->flag     = args->flag;
        item->param    = args->param;
        item->pageId   = args->pageId;
        item->Event    = (void (*)(struct ArkUI_item *))args->Event;
        item->userData = args->userData;
    }

    /* 3) 针对不同类型做默认值/备份 */
    switch (item->funcType)
    {
    case ITEM_CHECKBOX:
    case ITEM_RADIO_BUTTON:
    case ITEM_SWITCH:
        item->flagDefault = (item->flag) ? (*item->flag) : false;
        break;

    case ITEM_PROGRESS_BAR:
    case ITEM_CHANGE_VALUE:
        if (item->param)
        {
            item->paramBackup  = *item->param;
            item->paramDefault = *item->param;
        }
        else
        {
            item->paramBackup  = 0;
            item->paramDefault = 0;
        }
        break;

    case ITEM_JUMP_PAGE:
    case ITEM_MESSAGE:
    case ITEM_CUSTOM:
    default:
        break;
    }

    /* 4) 挂到 page 的 item 链表尾部 */
    if (page->itemHead == NULL)
    {
        item->id = 0;
        page->itemHead = item;
        page->itemTail = item;
    }
    else
    {
        item->id = page->itemTail->id + 1;
        page->itemTail->next = item;
        page->itemTail = item;
    }

    /* 5) 初始化位置 */
    item->lineId = item->id;
    item->posForCal = 0;
    item->step = 0;
    item->position = 0;
}

/**
 * @brief 添加一个ArkUI Page
 * @param page 指向ArkUIPage_t结构体的指针，用于指定要添加页面的指针
 * @param func 页面的功能类型，用于指定页面的行为
 * @param ... 可变参数，根据页面的功能类型不同，参数的数量和类型也不同
 */
void ArkUIAddPage(ArkUIPage_t *page, ArkUIPage_e func, ...)
{
    // 初始化页面的事件处理函数为 NULL
    page->Event = NULL;

    // 初始化可变参数列表
    va_list variableArg;
    va_start(variableArg, func);

    // 初始化页面的项目头指针、项目尾指针和下一个页面指针为 NULL
    page->itemHead = NULL;
    page->itemTail = NULL;
    page->next = NULL;

    // 设置页面的类型
    page->funcType = func;

    // 如果页面类型为 PAGE_CUSTOM，从可变参数中获取事件处理函数
    if (page->funcType == PAGE_CUSTOM)
    {
        page->Event = va_arg(variableArg, void (*)(ArkUIPage_t * ));
    }

    // 结束可变参数的使用
    va_end(variableArg);

    // 如果页面列表为空，将该页面设为列表的第一个页面
    if (pageHead == NULL)
    {
        page->id = 0;
        pageHead = page;
        pageTail = page;
    } 
    // 否则，将该页面添加到列表的尾部
    else
    {
        page->id = pageTail->id + 1;
        pageTail->next = page;
        pageTail = page;
    }
}

/** 
 * @brief 处理 ArkUI 项操作响应
 * @param page ArkUI页面指针
 * @param item ArkUI Item指针
 * @param index ArkUI Item索引指针
 * @return 无
*/
void ArkUIItemOperationResponse(ArkUIPage_t *page, ArkUIItem_t *item, uint8_t *index)
{
    switch (item->funcType)
    {
    case ITEM_JUMP_PAGE:
        if (layer == MAX_LAYER - 1)
            break;

        itemIndex[layer++] = *index;
        pageIndex[layer] = item->pageId;
        *index = 0;
        for (ArkUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
        {
            if (itemTmp->lineId < 0)
                continue;

            //除去屏幕上面的item,剩下的item的位置都重置一些,保证回退回来时,item能表现出从上面滑下来的效果
            itemTmp->position = 0;
            itemTmp->posForCal = 0;
        }
        ArkUITransitionAnim();
        break;
    case ITEM_CHECKBOX:
    case ITEM_SWITCH:
        *item->flag = !*item->flag;
        break;
    case ITEM_RADIO_BUTTON:
        for (ArkUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
        {
            if (itemTmp->funcType == ITEM_RADIO_BUTTON && itemTmp->id != item->id)
                *itemTmp->flag = false;
        }
        *item->flag = !*item->flag;
        break;
    case ITEM_PROGRESS_BAR:
        itemIndex[layer] = *index; 
        functionIsRunning = true;
        ArkUIBackgroundBlur();
        ArkUIDrawProgressBar(item);
        break;
    case ITEM_CHANGE_VALUE:
        itemIndex[layer] = *index; 
        functionIsRunning = true;
        ArkUIBackgroundBlur();
        break;
    case ITEM_MESSAGE:
        itemIndex[layer] = *index; 
        functionIsRunning = true;
        ArkUIDrawMsgBox(item->msg);
        break;
    case ITEM_CUSTOM:
        functionIsRunning = true;
        break;
    default:
        break;
    }
}

/**
 * @brief 内部函数,显示一个ArkUI Item
 * @param item 指向ArkUIItem_t结构体的指针，用于指定要显示的项
 * @note 标题长度不要太长
 */
static void ArkUIDisplayItem(ArkUIItem_t *item)
{
    switch (item->funcType)
    {
    case ITEM_JUMP_PAGE:
        ArkUIDisplayStr(2, item->position, "+");
        ArkUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
        break;
    case ITEM_PAGE_DESCRIPTION:
        ArkUIDisplayStr(2, item->position, item->title);
        break;
    case ITEM_CHECKBOX:
    case ITEM_RADIO_BUTTON:
        ArkUIDisplayStr(2, item->position, "-");
        ArkUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
        ArkUIDrawCheckbox(SCREEN_WIDTH - 7 - SCROLL_BAR_WIDTH - ITEM_HEIGHT + 2,
                           item->position - (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1, ITEM_HEIGHT - 2, CHECK_BOX_OFFSET,
                           *item->flag, 1);
        break;
    case ITEM_SWITCH:
        ArkUIDisplayStr(2, item->position, "-");
        ArkUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
        if (*item->flag)
            ArkUIDisplayStr(SCREEN_WIDTH - 7 - 3 * FONT_WIDTH - SCROLL_BAR_WIDTH, item->position, "ON");
        else
            ArkUIDisplayStr(SCREEN_WIDTH - 7 - 3 * FONT_WIDTH - SCROLL_BAR_WIDTH, item->position, "OFF");
        break;
    case ITEM_PROGRESS_BAR:
        ArkUIDisplayStr(2, item->position, "-");
        ArkUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
        if(*item->param < 10 && *item->param > 0){
            ArkUIDisplayFloat(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 7*FONT_WIDTH, item->position,
                               *item->param, 3, 2);
						ArkUISetDrawColor(OLED_DRAWMODE_XOR);
						ArkUIDisplayStr(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 7*FONT_WIDTH,item->position,"+");
						ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
						ArkUIDisplayStr(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 2*FONT_WIDTH,item->position,"%");
        }else if(*item->param < 100 && *item->param >= 10){
            ArkUIDisplayFloat(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 8*FONT_WIDTH, item->position,
                               *item->param, 4, 2);
						ArkUISetDrawColor(OLED_DRAWMODE_XOR);
						ArkUIDisplayStr(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 8*FONT_WIDTH,item->position,"+");
						ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
						ArkUIDisplayStr(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 2*FONT_WIDTH,item->position,"%");
        }else if(*item->param == 100){
						ArkUIDisplayStr(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 5*FONT_WIDTH,item->position,"100%");
				}else if(*item->param == 0){
						ArkUIDisplayStr(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 3*FONT_WIDTH,item->position,"0%");
				}else{// Hide because it's < 0 or > 100
						ArkUIDisplayStr(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"***");
				}
        break;
    case ITEM_CHANGE_VALUE:
        ArkUIDisplayStr(2, item->position, "-");
        ArkUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
		
			if(item->Event == &ArkUIEventChangeUint){
                if(*item->param < 10 && *item->param >= 0){
                    ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 2*FONT_WIDTH,item->position,"%d",(unsigned int)(*item->param));
                }else if(*item->param < 100 && *item->param >= 10){
                    ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 3*FONT_WIDTH,item->position,"%d",(unsigned int)(*item->param));
                }else if(*item->param < 1000 && *item->param >= 100){
                    ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"%d",(unsigned int)(*item->param));
                }else if(*item->param < 10000 && *item->param >= 1000){
                    ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 5*FONT_WIDTH,item->position,"%d",(unsigned int)(*item->param));
                }else if(*item->param >= 10000){// Hide because it's too long
                    ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"...");
                }else{// Hide because it's < 0
                    ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"***");
                }
			}else if(item->Event == &ArkUIEventChangeInt){
                if(*item->param >= 0){
                    if(*item->param < 10 && *item->param >= 0){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 2*FONT_WIDTH,item->position,"%d",(int)(*item->param));
                    }else if(*item->param < 100 && *item->param >= 10){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 3*FONT_WIDTH,item->position,"%d",(int)(*item->param));
                    }else if(*item->param < 1000 && *item->param >= 100){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"%d",(int)(*item->param));
                    }else if(*item->param < 10000 && *item->param >= 1000){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 5*FONT_WIDTH,item->position,"%d",(int)(*item->param));
                    }else if(*item->param >= 10000){// Hide because it's too long
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"...");
                    }
                }else{
                    if(*item->param > -10 && *item->param <= 0){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 3*FONT_WIDTH,item->position,"%d",(int)(*item->param));
                    }else if(*item->param > -100 && *item->param <= -10){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"%d",(int)(*item->param));
                    }else if(*item->param > -1000 && *item->param <= -100){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 5*FONT_WIDTH,item->position,"%d",(int)(*item->param));
                    }else if(*item->param > -10000 && *item->param <= -1000){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 6*FONT_WIDTH,item->position,"%d",(int)(*item->param));
                    }else if(*item->param <= -10000){// Hide because it's too long
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"...");
                    }
                }
			}else if(item->Event == &ArkUIEventChangeFloat){
                if(*item->param > 0){
                    if(*item->param >= 10000){// Hide because it's too long
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"...");
                    }else if(*item->param < 10 && *item->param > 0){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 6*FONT_WIDTH,item->position,"%.3f",*item->param);
                    }else if(*item->param < 100 && *item->param >= 10){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 6*FONT_WIDTH,item->position,"%.2f",*item->param);
                    }else if(*item->param < 1000 && *item->param >= 100){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 6*FONT_WIDTH,item->position,"%.1f",*item->param);
                    }else if(*item->param < 10000 && *item->param >= 1000){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 5*FONT_WIDTH,item->position,"%.0f",*item->param);
                    }
                }else if(*item->param < 0){
                    if(*item->param <= -10000){// Hide because it's too long
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 4*FONT_WIDTH,item->position,"...");
                    }else if(*item->param > -10 && *item->param < 0){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 7*FONT_WIDTH,item->position,"%.3f",*item->param);
                    }else if(*item->param > -100 && *item->param <= -10){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 7*FONT_WIDTH,item->position,"%.2f",*item->param);
                    }else if(*item->param > -1000 && *item->param <= -100){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 7*FONT_WIDTH,item->position,"%.1f",*item->param);
                    }else if(*item->param > -10000 && *item->param <= -1000){
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 6*FONT_WIDTH,item->position,"%.0f",*item->param);
                    }
                }else{
                        ArkUIPrintf(SCREEN_WIDTH - SCROLL_BAR_WIDTH - 2*FONT_WIDTH,item->position,"0");
                }
			}
        break;
    default:
        ArkUIDisplayStr(2, item->position, "-");
        ArkUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
        break;
    }
}

/**
 * @brief 给“列表页”的每个 item 计算它这一帧应该画在屏幕上的 Y 坐标（带平滑动画），并在光标走到屏幕外时自动滚动整页。
 * @param page ArkUI页面指针
 * @param item ArkUI Item指针
 * @param index ArkUI Item索引
 * @param timer 定时器周期
 */
static void ArkUIGetItemPos(ArkUIPage_t *page, ArkUIItem_t *item, uint8_t index, uint8_t timer)
{
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2;
    static uint16_t time = 0;
    static int16_t move = 0, target = 0;//要移动的行数和目标位置
    static uint8_t lastIndex = 0;
    uint8_t speed = ITEM_MOVE_TIME / timer;//动画帧数

    // Item need to move or not
    for (ArkUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
    {
        //找到当前光标指向的item
        if(itemTmp->id == index)
        {
            //当前选中项跑到屏幕外，待会lineId就挪动move行
            if(itemTmp->lineId < 0)
            {
                move = itemTmp->lineId;
                break;
            }else if(itemTmp->lineId > ITEM_LINES - 1)
            {
                move = itemTmp->lineId - (ITEM_LINES - 1);
                break;
            }
        }
    }

    // Change the item lineId and get target position
    //整体移动该页所有item来把选中的item拉回屏幕可见范围内
    for (ArkUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
    {
        itemTmp->lineId -= move;
    }
    move = 0;
    //把行号转变为像素坐标
    target = itemHeightOffset + item->lineId * ITEM_HEIGHT;

    // Calculate current position
    if (time == 0 || index != lastIndex)//动画刚开始或者光标位置换了，重新计算每帧步长
    {
        item->step = ((float) target - (float) item->position) / (float) speed;
    }
    //接着每帧推进
    if(time < ITEM_MOVE_TIME)//没到总时长，每帧加step，慢慢推过去
    {
        item->posForCal += item->step;
    }else//到了，直接贴到target，防止误差
    {
        item->posForCal = target;
    }        
    item->position = (int16_t) item->posForCal;//posForCal就是浮点数版本的position，前面用浮点数算来防止每次步进取整抖动

    lastIndex = index;

    // Time counter
    if (item->next == NULL)
    {
        if (target == item->position)
            time = 0;
        else
            time += timer;
    }
}

/**
 * @brief 内部函数，绘制ArkUI Item光标
 * @param page ArkUI页面指针
 * @param index ArkUI Item索引
 * @param timer 定时器周期
 */
void ArkUIDrawCursor(ArkUIPage_t *page, uint8_t index, uint8_t timer)
{
    //光标终点位置及尺寸
    int16_t tx = 0;
    int16_t ty = 0;
    int16_t tw = 0;
    int16_t th = (float)ITEM_HEIGHT;

    //更新对象
    uint32_t object = ((uint32_t)page->id << 16) | ((uint32_t)index << 8) | 0x00;
    uint32_t lastObject = Cursor.Get_Object();

    // 初始化光标位置为屏幕左下角,宽度为第一个 item 的标题长度,高度为0
    Struct_ArkUI_Rect nowCursor = Cursor.Get_Now_Cursor();
    if ((int)nowCursor.w == 0 && page->itemHead != NULL)
    {
        if (page->itemHead->funcType == ITEM_PAGE_DESCRIPTION)
            Cursor.Set_Now(0,SCREEN_HEIGHT,(float)(strlen(page->itemHead->title)) * FONT_WIDTH + 5,0,1);
        else
            Cursor.Set_Now(0,SCREEN_HEIGHT,(float)(strlen(page->itemHead->title) + 1) * FONT_WIDTH + 8,0,1);
        nowCursor = Cursor.Get_Now_Cursor();
    }

    // 找到当前 index 对应的 item -> 计算目标长度/目标 y
    for (ArkUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
    {
        if (index == itemTmp->id)
        {
            if (itemTmp->funcType == ITEM_PAGE_DESCRIPTION)
                tw = (uint16_t)(strlen(itemTmp->title)) * FONT_WIDTH + 5;
            else
                tw = (uint16_t)(strlen(itemTmp->title) + 1) * FONT_WIDTH + 8;

            ty = (uint16_t)(itemTmp->lineId * ITEM_HEIGHT);

            uint8_t lastIndex = (uint8_t)((lastObject >> 8) & 0xFF);
            //只有当对象变了的时候才更新目标位置
            if(object != lastObject)
            {
                //先正常更新位置
                Cursor.Goto(object, tx, ty, tw, th, INDICATOR_MOVE_TIME, 1);
                //要是索引变了,且item在相邻项之间切换 ,而不是从列表开头跳到结尾
                if(index != lastIndex && abs(index - lastIndex) < page->itemTail->id)
                {
                    //而且下一个光标要移动到的item在屏幕外面,就贴边一下
                    float edgeY = 0;
                    if (itemTmp->position < 0)
                    {
                        edgeY = (float)3 * ITEM_HEIGHT / 4;
                        Cursor.Goto_From(object, nowCursor.x, edgeY,nowCursor.w,ITEM_HEIGHT,tx, ty, tw, th,INDICATOR_MOVE_TIME, 1);
                    }
                    else if (itemTmp->position >= (ITEM_LINES) * ITEM_HEIGHT)
                    {
                        edgeY = (ITEM_LINES - 2) * ITEM_HEIGHT + (float)ITEM_HEIGHT / 4;
                        Cursor.Goto_From(object, nowCursor.x, edgeY,nowCursor.w,ITEM_HEIGHT,tx, ty, tw, th,INDICATOR_MOVE_TIME, 1);
                    }
                }
            }

            break;
        }
    }
    
    Cursor.Update(timer);
    Cursor.Draw_Cursor();
}

/**
 * @brief 内部函数，绘制ArkUI滚动条指示器
 * @param page ArkUI页面指针
 * @param index ArkUI Item索引
 * @param timer 定时器周期
 * @param status 指示器状态（0：隐藏，1：活跃）
 */
void ArkUIDrawScrollBarIndicator(ArkUIPage_t *page, uint8_t index, uint8_t timer, uint8_t status)
{
    if (page->funcType != PAGE_LIST)
        return;

        // ---------------- 可调参数 ----------------------------------------
        const uint16_t TRACK_SHOW_MS      = 700;   // 交互后轨道保持可见时间（ms）
        const uint8_t  SMOOTH_ACTIVE_Q16  = 2;    // 活跃时缓动强度（分子，分母=16）
        const uint8_t  SMOOTH_IDLE_Q16    = 2;     // 空闲时缓动强度（更慢）
        const int16_t  THUMB_MIN_H_FACTOR = 1;     // 最小高度 = ITEM_HEIGHT / THUMB_MIN_H_FACTOR
        // -----------------------------------------------------------------

        // 额外静态状态
        static int32_t thumbY_fp = 0;   // Q8 固定点
        static int32_t thumbH_fp = 0;   // Q8
        static int32_t thumbW_fp = 0;   // Q8
        static uint16_t trackVisibleTimer = 0; // ms
        static uint16_t lastTopIndex = 0xffff;
        static uint8_t  lastSelIndex = 0;
    //    static uint8_t  lastTotalItems = 0;
        static uint16_t prevTrackVisibleTimer = 0; // 用于检测 "刚从隐藏变为活跃"

        // 1) 统计总条目数
        uint16_t totalItems = 0;
        if (page->itemTail != NULL) {
            totalItems = (uint16_t)page->itemTail->id + 1;
        }

        // 2) 计算 topIndex（与之前保持一致）
        int16_t topIndex = 0;
        uint8_t foundTop = 0;
        for (ArkUIItem_t *it = page->itemHead; it != NULL; it = it->next) {
            if (it->lineId == 0) {
                topIndex = it->id;
                foundTop = 1;
                break;
            }
        }
        if (!foundTop) {
            int16_t minNonNegLineId = 0x7FFF;
            uint8_t idAtMin = 0;
            for (ArkUIItem_t *it = page->itemHead; it != NULL; it = it->next) {
                if (it->lineId >= 0 && it->lineId < minNonNegLineId) {
                    minNonNegLineId = it->lineId;
                    idAtMin = it->id;
                }
            }
            if (minNonNegLineId != 0x7FFF) {
                if (idAtMin >= (uint8_t)minNonNegLineId) {
                    topIndex = (int16_t)idAtMin - (int16_t)minNonNegLineId;
                } else {
                    topIndex = 0;
                }
            } else {
                topIndex = 0;
            }
        }

        // 3) 轨道参数
        const int16_t trackX = SCREEN_WIDTH - SCROLL_BAR_WIDTH;
        const int16_t trackY = 0;
        const int16_t trackH = SCREEN_HEIGHT;

        // 4) 若不足一屏 -> 收起（高度趋向 0）
        if (totalItems <= ITEM_LINES) {
            int32_t targetH_fp = 0;
            int32_t targetY_fp = (trackY << 8);
            int32_t targetW_fp = ((SCROLL_BAR_WIDTH - 1) << 8);
            uint8_t smooth = SMOOTH_IDLE_Q16;

            if (thumbH_fp == 0 && thumbY_fp == 0 && thumbW_fp == 0) {
                thumbH_fp = targetH_fp;
                thumbY_fp = targetY_fp;
                thumbW_fp = targetW_fp;
            }
            thumbH_fp += ((targetH_fp - thumbH_fp) * (int32_t)smooth) / 16;
            thumbY_fp += ((targetY_fp - thumbY_fp) * (int32_t)smooth) / 16;
            thumbW_fp += ((targetW_fp - thumbW_fp) * (int32_t)smooth) / 16;

            if (trackVisibleTimer > timer) trackVisibleTimer -= timer;
            else trackVisibleTimer = 0;

            if (trackVisibleTimer > 0 || (thumbH_fp >> 8) > 0) {
                int16_t drawH = (int16_t)(thumbH_fp >> 8);
                if (drawH > 0) {
                    int16_t drawW = (int16_t)(thumbW_fp >> 8);
                    if (drawW < 1) drawW = 1;
                    int16_t drawX = trackX + (SCROLL_BAR_WIDTH - drawW) / 2;
                    int16_t drawY = (int16_t)(thumbY_fp >> 8);
                    int16_t r = drawW / 2;
                    if (r > drawH / 2) r = drawH / 2;
                    if (r > 4) r = 4;
                    ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
                    ArkUIDrawRBox(drawX, drawY, drawW, drawH, 1, (uint8_t)r);
                }
            }

            lastTopIndex = (uint16_t)topIndex;
            lastSelIndex = index;
            prevTrackVisibleTimer = trackVisibleTimer;
            return;
        }

        // 5) 轨道可见计时器
        if (index != lastSelIndex || lastTopIndex != (uint16_t)topIndex || status) {
            trackVisibleTimer = TRACK_SHOW_MS;
        } else {
            if (trackVisibleTimer > timer) trackVisibleTimer -= timer;
            else trackVisibleTimer = 0;
        }

        // 检测是否刚刚由隐藏 -> 活跃（用于立即横向填满）
        uint8_t becameActive = (prevTrackVisibleTimer == 0 && trackVisibleTimer > 0) ? 1 : 0;

        // 6) 计算活跃目标几何
        int32_t thumbH_target = (int32_t)(((uint32_t)ITEM_LINES * (uint32_t)trackH + (totalItems / 2)) / totalItems);
        int32_t thumbMinH = ITEM_HEIGHT / THUMB_MIN_H_FACTOR;
        if (thumbMinH < 2) thumbMinH = 2;
        if (thumbH_target < thumbMinH) thumbH_target = thumbMinH;
        if (thumbH_target > trackH) thumbH_target = trackH;

        int16_t maxTopIndex = (int16_t)totalItems - (int16_t)ITEM_LINES;
        if (maxTopIndex < 1) maxTopIndex = 1;
        if (topIndex < 0) topIndex = 0;
        if (topIndex > maxTopIndex) topIndex = maxTopIndex;
        uint32_t numerator   = (uint32_t)topIndex * (uint32_t)(trackH - (int)thumbH_target);
        uint32_t denominator = (uint32_t)maxTopIndex;
        int32_t thumbY_active = (int32_t)((numerator + denominator / 2) / denominator) + trackY;

        int32_t thumbW_max = SCROLL_BAR_WIDTH;
        int32_t thumbW_min = (SCROLL_BAR_WIDTH > 2) ? (SCROLL_BAR_WIDTH - 2) : 1;
        int32_t thumbW_active = (trackVisibleTimer > 0 || lastTopIndex != (uint16_t)topIndex || index != lastSelIndex || status) ? thumbW_max : thumbW_min;

        int32_t thumbH_target_final = thumbH_target;
        int32_t thumbY_target_final = thumbY_active;
        int32_t thumbW_target_final = thumbW_active;

        if (trackVisibleTimer == 0 && index == lastSelIndex && lastTopIndex == (uint16_t)topIndex && !status) {
            thumbH_target_final = 0;
            thumbY_target_final = trackY;
            thumbW_target_final = thumbW_min;
        }

        // 7) 平滑速率
        uint8_t smooth = (trackVisibleTimer > 0 || index != lastSelIndex || lastTopIndex != (uint16_t)topIndex) ? SMOOTH_ACTIVE_Q16 : SMOOTH_IDLE_Q16;
        if (smooth > 16) smooth = 16;

        // 8) 转 Q8
        int32_t targetY_fp = (thumbY_target_final << 8);
        int32_t targetH_fp = (thumbH_target_final << 8);
        int32_t targetW_fp = (thumbW_target_final << 8);

        if (thumbH_fp == 0 && thumbY_fp == 0 && thumbW_fp == 0) {
            thumbH_fp = targetH_fp;
            thumbY_fp = targetY_fp;
            thumbW_fp = targetW_fp;
        }

        // 当刚变为活跃时，横向直接填满
        // 这里做“刚变活跃时宽度立即设置为目标宽度（不做缓动）”，高度仍然平滑动画：
        if (becameActive) {
            // 直接把宽度设为目标（Q8），这样绘制时看上去“横向已满，向下拉伸”
            thumbW_fp = targetW_fp;
        } else {
            // 正常缓动宽度
            thumbW_fp += ((targetW_fp - thumbW_fp) * (int32_t)smooth) / 16;
        }

        // 高度与 Y 使用缓动（始终）
        thumbY_fp += ((targetY_fp - thumbY_fp) * (int32_t)smooth) / 16;
        thumbH_fp += ((targetH_fp - thumbH_fp) * (int32_t)smooth) / 16;

        // 9) 转回整数并夹紧
        int16_t drawY = (int16_t)(thumbY_fp >> 8);
        int16_t drawH = (int16_t)(thumbH_fp >> 8);
        int16_t drawW = (int16_t)(thumbW_fp >> 8);

        if (drawH < 0) drawH = 0;
        if (drawW < 1) drawW = 1;
        if (drawY < trackY) drawY = trackY;
        if (drawY + drawH > trackY + trackH) drawY = trackY + trackH - drawH;
        if (drawY < trackY) drawY = trackY;

        // 10) 绘制轨道与滑块
        if (trackVisibleTimer > 0) {
            ArkUIDrawRFrame(trackX, trackY, SCROLL_BAR_WIDTH, trackH, 1, 1);
        }

        if (drawH > 0) {
            int16_t drawX = trackX + (SCROLL_BAR_WIDTH - drawW) / 2;
    //        uint8_t r = (uint8_t)(drawW / 2);
    //        if (r > drawH / 2) r = drawH / 2;
    //        if (r > 5) r = 5;
            ArkUISetDrawColor(OLED_DRAWMODE_NORMAL);
            ArkUIDrawRBox(drawX + 1, drawY+1, drawW - 1, drawH, 1, /*(uint8_t)r*/0);
        }
        // 11) 更新缓存索引与计数器
        lastTopIndex = (uint16_t)topIndex;
        lastSelIndex = index;
        prevTrackVisibleTimer = trackVisibleTimer;
}

/**
 * @brief 内部函数，绘制ArkUI 指示器
 * @param page ArkUI页面指针
 * @param index ArkUI Item索引
 * @param timer 定时器周期
 * @param status 指示器状态（0：隐藏，1：活跃）
 */
static void ArkUIDrawIndicator(ArkUIPage_t *page, uint8_t index, uint8_t timer, uint8_t status)
{
    if (page->funcType != PAGE_LIST)
        return;

    ArkUIDrawCursor(page, index, timer);

    ArkUIDrawScrollBarIndicator(page, index, timer, status);
}


/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
