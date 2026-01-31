/**
 * @file dvc_arkey.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief Ark按键驱动
		使用模板例子：
		#include "stm32f10x.h"                  // Device header
		#include "Delay.h"
		#include "OLED.h"
		#include "dvc_arkey.h"
		#include "Timer2.h"

		ArkKey_t key_a,key_b,key_c;

		unsigned int a = 0,b = 0,c = 0;

		int main(void){
			
			Timer2_Init();
			OLED_Init();
			ArkKeyInit(&key_a,GPIOA,GPIO_Pin_1);
			ArkKeyInit(&key_b,GPIOA,GPIO_Pin_3);
			ArkKeyInit(&key_c,GPIOA,GPIO_Pin_5);
			OLED_Clear();

			while(1){
				
				if (key_a.isMultiClick) {
						a--;
				} 
				if (key_a.isPressed) {
						a++;
				} 
				if (key_a.isHold) {
						a = 0;
				}
				
			if(key_b.isPressed){
			b++;
			}else if(key_b.isHold){
			b+=10;
			}
				
			if(key_c.isPressed){
			c++;
			}else if(key_c.isHold){
			c+=10;
			}
				ArkKeyClearFlags(&key_a);
				ArkKeyClearFlags(&key_b);
				ArkKeyClearFlags(&key_c);
				
				OLED_ClearArea(0,0,48,24);
				OLED_Printf(0,0,OLED_6X8,"%d",a);
				OLED_Printf(0,8,OLED_6X8,"%d",b);
				OLED_Printf(0,16,OLED_6X8,"%d",c);
				
				OLED_Update();
			}
			
			return 0;
		
		}

		void TIM2_IRQHandler(void){//1 ms
			static unsigned char ark_Key_Cnt = 0;
			if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET){
				ark_Key_Cnt++;
				if(ark_Key_Cnt == UPDATE_KEY_STATE_MS){
					ark_Key_Cnt = 0;
					
					ArkKeyScanKeyState();
					ArkKeyUserApp();
				}
				
				TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
			}
		}
 * @version 1.0
 * @date 2026-1-23
 * @copyright Wang Fangzhuo (c) 2026
 */

#ifndef DVC_ARKEY_H
#define DVC_ARKEY_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>                         //为了用NULL
#include "main.h"

/* Exported macros -----------------------------------------------------------*/

#define UPDATE_KEY_STATE_MS     15       // 按键状态更新间隔，单位毫秒
#define HOLD_THRESHOLD_MS       1000     // 长按判定阈值，持续按下时间超过此值被认为是长按，单位毫秒
#define INTERVAL_THRESHOLD_MS   150     // 连击判定阈值，两次触发间隔小于此值被认为是连击，单位毫秒

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Ark按键状态机
 *
 */
typedef enum {                      // 按键状态机
	down,      // 按下瞬间
	up,        // 释放瞬间
	pressed,   // 持续按下状态
	released   // 持续释放状态
} ArkKey_State_t;

/*
 * @brief Ark按键对象
 *
 * 说明:
 * - 每个对象对应一个实体按键(一个GPIO+PIN)
 * - 内部使用静态链表统一管理所有对象, 以便批量扫描
 * - 外部使用 isPressed/isUp/isHold/isMultiClick/isReleased/clickState 等标志位
 *
 */
typedef struct ArkKey_typedef{
    // 内部状态管理
    uint8_t value;  // 当前GPIO电平值/缓存值（0=按下 1=未按）
    uint8_t preValue;           // 前次扫描值
    struct ArkKey_typedef *next; // 链表指针（用于多按键管理）
    uint32_t holdTime;          // 长按判定时间(ms)
    uint32_t intervalTime;      // 连击间隔时间(ms)	
    uint8_t clickCount;         // 点击次数

	GPIO_TypeDef * gpio; 
	uint16_t pin;
	
	ArkKey_State_t state;
		
    // 用户接口
    bool isPressed;     // 按下瞬间触发标志
    bool isHold;        // 长按事件标志
    bool isMultiClick;  // 连击事件标志
	bool isUp;        // 按键释放瞬间
	bool isReleased;  // 持续释放状态
    uint8_t clickState; // 点击状态（0=无操作 1=单击 2=双击...）
} ArkKey_t;

/* Exported variables --------------------------------------------------------*/

/* Exported function declarations --------------------------------------------*/

void ArkKeyInit(ArkKey_t *key,GPIO_TypeDef * GPIOX,uint16_t GPIO_Pin_x);
void ArkKeyScanKeyState(void);// 放在定时器中，定期执行 10ms

void ArkKeyUserApp(void);//按照目前的写法，也要放在定时器中伴随ArkKeyScanKeyState进行循环
void ArkKeyClearFlags(ArkKey_t *key);//要紧接着主循环中的按键状态执行放

#ifdef __cplusplus
}
#endif

#endif

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/

