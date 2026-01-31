/**
 * @file main.c
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief 主函数源文件.
 * @version 1.0
 * @date 2026-1-23
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "tsk_config_and_callback.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/

/* Function definitions -------------------------------------------------------*/

int main(void){

    Task_Init();

    while(1){
        Task_Loop();	
    }

    return 0;
}

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
