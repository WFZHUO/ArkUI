/**
 * @file tsk_config_and_callback.c
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief 任务配置与回调函数(当成main.c来用就行了，不在main中直接写的原因是,main是整个程序的入口,尽量保持它干净).
          此项目于wfz大一暑假编写绝大数功能均已实现,由于后期添加新功能导致程序出现较大bug且大二开始带队新乡学院RM,就没有立即完善,搁置至2026年1月23日重构.
 * @version 1.0
 * @date 2026-1-23
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "tsk_config_and_callback.h"

#include "tim.h"
#include "arkui.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

uint8_t uiUpdateFlag = 0; // 添加UI更新标志

/* Private function declarations ---------------------------------------------*/

/* Function definitions -------------------------------------------------------*/

/**
 * @brief TIM2任务中断函数,1ms
 *
 */
extern "C" void TIM2_IRQHandler(void){
    if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET){
        /* USER CODE BEGIN  */
        static unsigned char ark_Key_Cnt = 0;
        ark_Key_Cnt++;
        if(ark_Key_Cnt == UPDATE_KEY_STATE_MS){
            ark_Key_Cnt = 0;
            
            Class_ArkKey::ScanKeyState();
            Class_ArkKey::UserApp();
            uiUpdateFlag = 1;
        }

		static unsigned char UI_Time = 0;
        UI_Time++;
        if(UI_Time == 15){
            UI_Time = 0; 
            uiUpdateFlag = 1;
        }

        /* USER CODE END  */
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    }
}

/**
 * @brief 初始化任务
 *
 */
void Task_Init()
{
    TIM2_Init();
    ArkUIInit();
}

/**
 * @brief 前台循环任务
 *
 */
void Task_Loop()
{
    if(uiUpdateFlag){
        uiUpdateFlag = 0;
        /*
        ArkKeyClearFlags(&keyUp);等...需要在ArkUI前调用，而不能在它之后调用，
        因为在它之后调用就会出现 ArkUI是很占整个main函数的进程比例的，所以当按下按键时，
        大部分会落入ArkUI正在进行的区间，而当ArkUI结束时ArkKeyClearFlags已经将isPress清除了，
        下一循环ArkUIKeyActionMonitor因此也检测不到，进而ArkUI检测不到
        */
        ArkUIKeyActionMonitor();
        Class_ArkKey::ClearAllFlags();
    
        ArkUI(15);
    }	
}

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
