/**
 * @file arkui_register.c
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief ArkUI注册函数
 * @version 1.0
 * @date 2026-1-26
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "arkui_register.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

ArkUIPage_t aa;
ArkUIItem_t b;
/* ---------Page-0---------- */
/**
 * @brief 启动页
 */
ArkUIPage_t page0;
ArkUIItem_t jump_to_p1,jump_to_p2,jump_to_p3,jump_to_p4,jump_to_p5,jump_to_p6,jump_to_p7,a;

/* ---------Page-1---------- */
/**
 * @brief switch功能展示页
 */
ArkUIPage_t page1;
ArkUIItem_t title_of_p1, Sw_for_sw1, Sw_for_sw2, Sw_for_sw3;
bool sw1 = false,sw2 = false, sw3 = true;

/* ---------Page-2---------- */
/**
 * @brief parameter功能展示页
 */
ArkUIPage_t page2;
ArkUIItem_t title_of_p2, par_for_Float1, par_for_Int1, par_for_Uint1, par_for_Uint2, par_for_Uint3, par_for_Uint4, par_for_Uint5;
double Float1 = 0.000000001, Int1 = -10000, Uint1 = 0,Uint2 = 10,Uint3 = 100,Uint4 = 10000,Uint5 = 10.3;

/* ---------Page-3---------- */
/**
 * @brief 进度条功能展示页
 */
ArkUIPage_t page3;
ArkUIItem_t title_of_p3, par_for_Bar1, par_for_Bar2, par_for_Bar3, par_for_Bar4;
double Bar1 = 0.01,Bar2 = 5,Bar3 = 50,Bar4 = 100;

/* ---------Page-4---------- */
/**
 * @brief 单选框功能展示页
 */
ArkUIPage_t page4;
ArkUIItem_t title_of_p4, Rb_for_rb1, Rb_for_rb2, Rb_for_rb3, Rb_for_rb4, Rb_for_rb5, Rb_for_rb6, Rb_for_rb7, Rb_for_rb8, Rb_for_rb9;
bool rb1 = false, rb2 = true, rb3 = false, rb4 = false, rb5 = false,rb6 = false, rb7 = false, rb8 = false, rb9 = false;

/* ---------Page-5---------- */
/**
 * @brief 复选框功能展示页
 */
ArkUIPage_t page5;
ArkUIItem_t title_of_p5, Ch_for_ch1, Ch_for_ch2, Ch_for_ch3, Ch_for_ch4;
bool ch1 = true, ch2 = true, ch3 = false, ch4 = true;

/* ---------Page-6---------- */
/**
 * @brief 消息功能展示页
 */
ArkUIPage_t page6;
ArkUIItem_t title_of_p6, mas_for_Massge1;
char * Massge1= "Hello_World!_My_name_is___ArkUI!!!_____I_made_by----Wang_FANGZHUO";

/* ---------Page-7---------- */
/**
 * @brief 自定义事件功能展示页
 */
ArkUIPage_t page7;
ArkUIItem_t title_of_p7,game_entry,title_of_par,par_for_fallspeed,par_for_paddlew,par_for_spawnrate,par_for_maxbricks;

/* Private function declarations ---------------------------------------------*/

/* Function definitions -------------------------------------------------------*/

void ArkUI_RegisterMenu(void)
{
    ArkUIItemArgs_t args;

    ArkUIAddPage(&aa, PAGE_LIST);
    /* -----------register page----------- */
    ArkUIAddPage(&page0, PAGE_CUSTOM, IconPageEvent);
    ArkUIAddPage(&page1, PAGE_LIST);
    ArkUIAddPage(&page2, PAGE_LIST);
    ArkUIAddPage(&page3, PAGE_LIST);
    ArkUIAddPage(&page4, PAGE_LIST);
    ArkUIAddPage(&page5, PAGE_LIST);
    ArkUIAddPage(&page6, PAGE_LIST);
    ArkUIAddPage(&page7, PAGE_LIST);

    memset(&args, 0, sizeof(args)); args.pageId = page0.id;args.Event = ArkUIEventEnterIconPage;
    ArkUIAddItem(&aa, &b, "[666]", ITEM_CUSTOM, &args);
    
    /* -----------register item----------- */
    /* ===== Page-0 ===== */
    memset(&args, 0, sizeof(args)); args.pageId = page1.id;args.icon = icon_Switch;
    ArkUIAddItem(&page0, &jump_to_p1, "[Switch]", ITEM_JUMP_PAGE, &args);

    memset(&args, 0, sizeof(args)); args.pageId = page2.id;args.icon = icon_param;
    ArkUIAddItem(&page0, &jump_to_p2, "[Value]", ITEM_JUMP_PAGE, &args);

    memset(&args, 0, sizeof(args)); args.pageId = page3.id;args.icon = icon_bar;
    ArkUIAddItem(&page0, &jump_to_p3, "[Bar]", ITEM_JUMP_PAGE, &args);

    memset(&args, 0, sizeof(args)); args.pageId = page4.id;args.icon = icon_Key;
    ArkUIAddItem(&page0, &jump_to_p4, "[Button]", ITEM_JUMP_PAGE, &args);

    memset(&args, 0, sizeof(args)); args.pageId = page5.id;args.icon = icon_Muscle;
    ArkUIAddItem(&page0, &jump_to_p5, "[Checkbox]", ITEM_JUMP_PAGE, &args);

    memset(&args, 0, sizeof(args)); args.pageId = page6.id;args.icon = icon_earth;
    ArkUIAddItem(&page0, &jump_to_p6, "[Massge]", ITEM_JUMP_PAGE, &args);

    memset(&args, 0, sizeof(args)); args.pageId = page7.id;args.icon = icon_terminal;
    ArkUIAddItem(&page0, &jump_to_p7, "[Custom]", ITEM_JUMP_PAGE, &args);

    memset(&args, 0, sizeof(args)); args.Event = ArkUIEventCatchBrick; args.icon = icon_CatchBrick;
    ArkUIAddItem(&page0, &a, "CatchBrick", ITEM_CUSTOM, &args);

    /* ===== Page-1 ===== */
    ArkUIAddItem(&page1, &title_of_p1, "[Switch]", ITEM_PAGE_DESCRIPTION, NULL);

    memset(&args, 0, sizeof(args)); args.flag = &sw1;
    ArkUIAddItem(&page1, &Sw_for_sw1, "Switch 1", ITEM_SWITCH, &args);

    memset(&args, 0, sizeof(args)); args.flag = &sw2;
    ArkUIAddItem(&page1, &Sw_for_sw2, "Switch 2", ITEM_SWITCH, &args);

    memset(&args, 0, sizeof(args)); args.flag = &sw3;
    ArkUIAddItem(&page1, &Sw_for_sw3, "Switch 3", ITEM_SWITCH, &args);

    /* ===== Page-2 ===== */
    ArkUIAddItem(&page2, &title_of_p2, "[Value]", ITEM_PAGE_DESCRIPTION, NULL);

    memset(&args, 0, sizeof(args)); args.param = &Float1; args.Event = ArkUIEventChangeFloat;
    ArkUIAddItem(&page2, &par_for_Float1, "float1", ITEM_CHANGE_VALUE, &args);

    memset(&args, 0, sizeof(args)); args.param = &Int1; args.Event = ArkUIEventChangeInt;
    ArkUIAddItem(&page2, &par_for_Int1, "int1", ITEM_CHANGE_VALUE, &args);

    memset(&args, 0, sizeof(args)); args.param = &Uint1; args.Event = ArkUIEventChangeUint_Cursor;
    ArkUIAddItem(&page2, &par_for_Uint1, "uint1", ITEM_CHANGE_VALUE, &args);

    memset(&args, 0, sizeof(args)); args.param = &Uint2; args.Event = ArkUIEventChangeUint;
    ArkUIAddItem(&page2, &par_for_Uint2, "uint2", ITEM_CHANGE_VALUE, &args);

    memset(&args, 0, sizeof(args)); args.param = &Uint3; args.Event = ArkUIEventChangeUint;
    ArkUIAddItem(&page2, &par_for_Uint3, "uint3", ITEM_CHANGE_VALUE, &args);

    memset(&args, 0, sizeof(args)); args.param = &Uint4; args.Event = ArkUIEventChangeUint;
    ArkUIAddItem(&page2, &par_for_Uint4, "uint4", ITEM_CHANGE_VALUE, &args);

    memset(&args, 0, sizeof(args)); args.param = &Uint5; args.Event = ArkUIEventChangeUint;
    ArkUIAddItem(&page2, &par_for_Uint5, "uint5", ITEM_CHANGE_VALUE, &args);

    /* ===== Page-3 ===== */
    ArkUIAddItem(&page3, &title_of_p3, "[Bar]", ITEM_PAGE_DESCRIPTION, NULL);

    memset(&args, 0, sizeof(args)); args.param = &Bar1; args.Event = ArkFunctionExit;
    ArkUIAddItem(&page3, &par_for_Bar1, "Bar1", ITEM_PROGRESS_BAR, &args);

    memset(&args, 0, sizeof(args)); args.param = &Bar2; args.Event = ArkFunctionExit;
    ArkUIAddItem(&page3, &par_for_Bar2, "Bar2", ITEM_PROGRESS_BAR, &args);

    memset(&args, 0, sizeof(args)); args.param = &Bar3; args.Event = ArkFunctionExit;
    ArkUIAddItem(&page3, &par_for_Bar3, "Bar3", ITEM_PROGRESS_BAR, &args);

    memset(&args, 0, sizeof(args)); args.param = &Bar4; args.Event = ArkFunctionExit;
    ArkUIAddItem(&page3, &par_for_Bar4, "Bar4", ITEM_PROGRESS_BAR, &args);

    /* ===== Page-4 ===== */
    ArkUIAddItem(&page4, &title_of_p4, "[Button]", ITEM_PAGE_DESCRIPTION, NULL);

    memset(&args, 0, sizeof(args)); args.flag = &rb1;
    ArkUIAddItem(&page4, &Rb_for_rb1, "button 1", ITEM_RADIO_BUTTON, &args);

    memset(&args, 0, sizeof(args)); args.flag = &rb2;
    ArkUIAddItem(&page4, &Rb_for_rb2, "button 2", ITEM_RADIO_BUTTON, &args);

    memset(&args, 0, sizeof(args)); args.flag = &rb3;
    ArkUIAddItem(&page4, &Rb_for_rb3, "button 3", ITEM_RADIO_BUTTON, &args);

    memset(&args, 0, sizeof(args)); args.flag = &rb4;
    ArkUIAddItem(&page4, &Rb_for_rb4, "button 4", ITEM_RADIO_BUTTON, &args);

    memset(&args, 0, sizeof(args)); args.flag = &rb5;
    ArkUIAddItem(&page4, &Rb_for_rb5, "button 5", ITEM_RADIO_BUTTON, &args);

    memset(&args, 0, sizeof(args)); args.flag = &rb6;
    ArkUIAddItem(&page4, &Rb_for_rb6, "button 6", ITEM_RADIO_BUTTON, &args);

    memset(&args, 0, sizeof(args)); args.flag = &rb7;
    ArkUIAddItem(&page4, &Rb_for_rb7, "button 7", ITEM_RADIO_BUTTON, &args);

    memset(&args, 0, sizeof(args)); args.flag = &rb8;
    ArkUIAddItem(&page4, &Rb_for_rb8, "button 8", ITEM_RADIO_BUTTON, &args);

    memset(&args, 0, sizeof(args)); args.flag = &rb9;
    ArkUIAddItem(&page4, &Rb_for_rb9, "button 9", ITEM_RADIO_BUTTON, &args);

    /* ===== Page-5 ===== */
    ArkUIAddItem(&page5, &title_of_p5, "[Checkbox]", ITEM_PAGE_DESCRIPTION, NULL);

    memset(&args, 0, sizeof(args)); args.flag = &ch1;
    ArkUIAddItem(&page5, &Ch_for_ch1, "Checkbox 1", ITEM_CHECKBOX, &args);

    memset(&args, 0, sizeof(args)); args.flag = &ch2;
    ArkUIAddItem(&page5, &Ch_for_ch2, "Checkbox 2", ITEM_CHECKBOX, &args);

    memset(&args, 0, sizeof(args)); args.flag = &ch3;
    ArkUIAddItem(&page5, &Ch_for_ch3, "Checkbox 3", ITEM_CHECKBOX, &args);

    memset(&args, 0, sizeof(args)); args.flag = &ch4;
    ArkUIAddItem(&page5, &Ch_for_ch4, "Checkbox 4", ITEM_CHECKBOX, &args);

    /* ===== Page-6 ===== */
    ArkUIAddItem(&page6, &title_of_p6, "[Mas]", ITEM_PAGE_DESCRIPTION, NULL);

    memset(&args, 0, sizeof(args)); args.msg = Massge1; args.Event = ArkFunctionExit;
    ArkUIAddItem(&page6, &mas_for_Massge1, "Mas", ITEM_MESSAGE, &args);

    /* ===== Page-7 ===== */
    ArkUIAddItem(&page7, &title_of_p7, "[Custom]", ITEM_PAGE_DESCRIPTION, NULL);

    memset(&args, 0, sizeof(args)); args.Event = ArkUIEventCatchBrick; args.icon = icon_CatchBrick;
    ArkUIAddItem(&page7, &game_entry, "CatchBrick", ITEM_CUSTOM, &args);
    
    ArkUIAddItem(&page7, &title_of_par, "[Parame]", ITEM_PAGE_DESCRIPTION, NULL);

    memset(&args, 0, sizeof(args)); args.param = &gGame_FallSpeed; args.Event = ArkUIEventChangeFloat;
    ArkUIAddItem(&page7, &par_for_fallspeed, "Fall-v", ITEM_CHANGE_VALUE, &args);

    memset(&args, 0, sizeof(args)); args.param = &gGame_PaddleWidth; args.Event = ArkUIEventChangeUint;
    ArkUIAddItem(&page7, &par_for_paddlew, "PaddleW", ITEM_CHANGE_VALUE, &args);
    
    memset(&args, 0, sizeof(args)); args.param = &gGame_SpawnRate; args.Event = ArkUIEventChangeFloat;
    ArkUIAddItem(&page7, &par_for_spawnrate, "GP", ITEM_CHANGE_VALUE, &args);

    memset(&args, 0, sizeof(args)); args.param = &gGame_MaxBricks; args.Event = ArkUIEventChangeUint;
    ArkUIAddItem(&page7, &par_for_maxbricks, "MaxBricks", ITEM_CHANGE_VALUE, &args);

}

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
