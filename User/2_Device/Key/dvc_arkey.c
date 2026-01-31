/**
 * @file dvc_arkey.c
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief Ark按键驱动.模仿电脑鼠标按键逻辑,自带GPIO初始化配置
 * @version 1.0
 * @date 2026-1-23
 * @copyright Wang Fangzhuo (c) 2026
 */

/* Includes ------------------------------------------------------------------*/

#include "dvc_arkey.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

// 链表头尾指针
ArkKey_t *head = NULL, *tail = NULL;

/* Private function declarations ---------------------------------------------*/

/* Function definitions -------------------------------------------------------*/

/**
 * @brief 根据状态机产生用户事件标志(与ScanKeyState同周期调用)
 * @note 按下只是按下，你要怎么解释这个按下是应用层的事。
 *       要想真正实现双击或者长按，就要在应用层对按键的互动进行设计，若为长按/双击，则撤销单击效果。
 *
 */
 void ArkKeyUserApp(void){  // 与 ArkKeyScanKeyState 搭配，每 10ms 调用一次
	for(ArkKey_t *key = head; key != NULL; key = key->next){
		
        // ---- 按下瞬间 ----
        if (key->state == down) {
            key->isPressed = true;     // 立即按下事件
            key->holdTime = 0;		// holdTime清零并开始计holdTime
            
            // 如果是第一次点击的按下，则先判定为单击
            if (key->clickCount == 0) {
                key->clickState = 1;   // 先认为是单击
                key->clickCount = 1;   // 记录点击次数
            }
            // 如果已经是等待第二次点击，升级为双击
            else if (key->clickCount == 1 && key->intervalTime <= INTERVAL_THRESHOLD_MS) {
                key->isMultiClick = true;
                key->clickState = 2; // 双击
                key->clickCount = 0; // 双击后重置点击次数
            }
        }

        // ---- 持续按下 ----
        if (key->state == pressed) {
            key->holdTime += UPDATE_KEY_STATE_MS;// 持续按下时增加holdTime
            if (key->holdTime >= HOLD_THRESHOLD_MS) {
                key->isHold = true;    // 触发长按事件,如果一直按那就一直触发长按事件
                key->clickCount = 0;   // 长按不参与多击
            }
        }

        // ---- 松手瞬间 ----
        if (key->state == up) {
			key->isUp = true;    // 设置释放标志
            // 如果是第一次点击的松手则等待可能的双击
            if (key->clickCount == 1) {
                key->intervalTime = 0; // intervalTime清零并开始计intervalTime
            }
        }

        // ---- 完全释放 ----
        if (key->state == released) {
			key->isReleased = true; // 设置持续释放标志
            if (key->clickCount > 0) {
                key->intervalTime += UPDATE_KEY_STATE_MS;// 持续释放时增加intervalTime

                // 超时后确认单击（已经发过单击标志，这里只是结束等待）
                if (key->intervalTime > INTERVAL_THRESHOLD_MS) {
                    key->clickCount = 0; // 连击等待结束
                    key->intervalTime = 0;//超时后此值无意义，不再代表Up1-Down2之间的时间，清零
                }
            }
        }
		
	}
}

/**
 * @brief 清除按键标志位
 * @note  在这里清除isHold和isReleased其实就是个摆设,因为下一周期它们还会true回来，但是本着要清就全清的原则，还是清了
 * @param key 按键对象指针
 */
void ArkKeyClearFlags(ArkKey_t *key) {
    key->isPressed = false;
    key->isHold = false;
    key->isMultiClick = false;
	key->isReleased = false;
	key->isUp = false;
	key->clickState = 0;
}

/**
 * @brief 初始化按键对象
 *
 * @param key 按键对象指针
 * @param GPIOX GPIO端口
 * @param GPIO_Pin_x GPIO引脚
 */
void ArkKeyInit(ArkKey_t *key,GPIO_TypeDef * GPIOX,uint16_t GPIO_Pin_x){
	key->value = 1;
	key->preValue = 1;
	key->next = NULL;
	key->holdTime = 0;
	key->intervalTime = 0;
	key->clickCount = 0;
	key->state = released;
	key->isPressed = false;
	key->isHold = false;
	key->isMultiClick = false;
	key->isUp = false;
	key->isReleased = false;
	key->clickState = 0;
	key->gpio = GPIOX;
	key->pin = GPIO_Pin_x;
	if(head == NULL){
		head = key;
		tail = key;
	}else{
		tail->next = key;
		tail = key;
	}
	
	if(GPIOX == GPIOA){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	}else if(GPIOX == GPIOB){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(GPIOB,&GPIO_InitStructure);
	}
	
}

/**
 * @brief 防抖滤波
 * @note  使用时间采样法滤去高频抖动,仅能滤除抖动持续时间小于UPDATE_KEY_STATE_MS的抖动(顶多按键反馈会延时UPDATE_KEY_STATE_MS),
 *        如果抖动时间大于UPDATE_KEY_STATE_MS,则有概率触发多次按下等情况
 *        且连续主观按下间隔要大于2*UPDATE_KEY_STATE_MS(这个值不是严密推导得出的),否则会漏变化沿导致后续逻辑错误，应该没有超人般手速的人吧...
 *
 * @param key 按键对象指针
 */
void DebounceFilter(){
	for (ArkKey_t *key = head; key != NULL; key = key->next){
		key->preValue = key->value;
		key->value = GPIO_ReadInputDataBit(key->gpio,key->pin);
	}
}

/**
 * @brief 扫描按键状态
 *
 * @param key 按键对象指针
 */
void ArkKeyScanKeyState(){// 放在定时器中，定期执行 10ms
	DebounceFilter();

	for (ArkKey_t *key = head; key != NULL; key = key->next){
		// 状态迁移判断
		if (key->preValue > key->value)    // 下降沿（按下）
				key->state = down;
		else if (key->preValue < key->value) // 上升沿（释放）
				key->state = up;
		else if (key->value == 0)          // 持续低电平
				key->state = pressed;
		else                              // 持续高电平
				key->state = released;
	}
}

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
