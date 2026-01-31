/**
 * @file arkui_config.c
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief ArkUI参数配置以及ArkUI调用到的API的配置,阻断ArkUI与底层驱动的耦合,方便使用其它底层驱动
 * @version 1.0
 * @date 2026-1-23
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "arkui_config.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/**
 * @brief ArkUI按键配置,配置三个物理按键作为ArkUI的上、下、确认键
 * 
 */
Class_ArkKey keyUp, keyDown, keyConfirm;

/* Private function declarations ---------------------------------------------*/

/* Function definitions -------------------------------------------------------*/

/**
 * @brief 初始化ArkUI按键,根据对应GPIO引脚配置
 * 
 */
void ArkUIKeyInit(void)
{
    keyUp.Init(GPIOA,GPIO_Pin_1);
    keyDown.Init(GPIOB,GPIO_Pin_7);
    keyConfirm.Init(GPIOA,GPIO_Pin_7);
}

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
