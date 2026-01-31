/**
 * @file dvc_oled_data.h
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief Ark OLED 字库/图像数据声明. 兼容 C/C++ 工程.
 * @version 1.0
 * @date 2026-01-25
 * @copyright Wang Fangzhuo (c) 2026
 */

#ifndef DVC_OLED_DATA_H
#define DVC_OLED_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

//ASCII字模数据声明
extern const unsigned char OLED_F8x16[][16];
extern const unsigned char OLED_F6x8[][6];

//图像数据声明
extern const unsigned char Fonzo[];
extern const unsigned char icon_Key[];
extern const unsigned char icon_Muscle[];
extern const unsigned char icon_apple[];
extern const unsigned char icon_earth[];
extern const unsigned char icon_terminal[];
extern const unsigned char icon_flag[];
extern const unsigned char icon_ps[];
extern const unsigned char icon_CatchBrick[];
extern const unsigned char icon_Switch[];
extern const unsigned char icon_param[];
extern const unsigned char icon_bar[];

/*按照上面的格式，在这个位置加入新的图像数据声明*/
//...

/* Exported function declarations --------------------------------------------*/

#ifdef __cplusplus
} // extern "C"
#endif

#endif

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
