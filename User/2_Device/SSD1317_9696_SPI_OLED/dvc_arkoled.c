/**
 * @file dvc_arkoled.c
 * @author Wang Fangzhuo (WeChat: ncasjifa)
 * @brief Ark OLED 驱动实现. 兼容 C/C++ 工程.
 * @version 1.0
 * @date 2026-01-25
 * @copyright Wang Fangzhuo (c) 2026
 */

/**
  * 坐标轴定义：
  * 左上角为(0, 0)点
  * 横向向右为X轴，取值范围：0~95
  * 纵向向下为Y轴，取值范围：0~95
  * 
  *       0             X轴       95 
  *      .--------------------------->
  *    0 |
  *      |
  *      |
  *      |
  *  Y轴 |
  *      |
  *      |
  *      |
  *   95 |
  *      v
  * 
 **/

/* Includes ------------------------------------------------------------------*/

#include "dvc_arkoled.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/**
  * @brief OLED显存数组
  * 所有的显示函数，都只是对此显存数组进行读写
  * 随后调用OLED_Update函数或OLED_UpdateArea函数
  * 才会将显存数组的数据发送到OLED硬件，进行显示
  */
uint8_t OLED_DisplayBuf[OLED_PAGES][OLED_WIDTH];
static OLED_DrawMode oled_draw_mode = OLED_DRAWMODE_NORMAL;

/* Private function declarations ---------------------------------------------*/

/* Function definitions -------------------------------------------------------*/


/*----------------------引脚配置----------------------*/

/**
  * @brief OLED写D0（CLK）高低电平
  * @param BitValue 要写入D0的电平值，范围：0/1
  * @note 当上层函数需要写D0时，此函数会被调用
  *       用户需要根据参数传入的值，将D0置为高电平或者低电平
  *       当参数传入0时，置D0为低电平，当参数传入1时，置D0为高电平
  */
void OLED_W_D0(uint8_t BitValue)
{
	/*根据BitValue的值，将D0置高电平或者低电平*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)BitValue);
}

/**
  * @brief OLED写D1（MOSI）高低电平
  * @param BitValue 要写入D1的电平值，范围：0/1
  * @note 当上层函数需要写D1时，此函数会被调用
  *       用户需要根据参数传入的值，将D1置为高电平或者低电平
  *       当参数传入0时，置D1为低电平，当参数传入1时，置D1为高电平
  */
void OLED_W_D1(uint8_t BitValue)
{
	/*根据BitValue的值，将D1置高电平或者低电平*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)BitValue);
}

/**
  * @brief OLED写RES高低电平
  * @param BitValue 要写入RES的电平值，范围：0/1
  * @note 当上层函数需要写RES时，此函数会被调用
  *       用户需要根据参数传入的值，将RES置为高电平或者低电平
  *       当参数传入0时，置RES为低电平，当参数传入1时，置RES为高电平
  */
void OLED_W_RES(uint8_t BitValue)
{
	/*根据BitValue的值，将RES置高电平或者低电平*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, (BitAction)BitValue);
}

/**
  * @brief OLED写DC高低电平
  * @param BitValue 要写入DC的电平值，范围：0/1
  * @note 当上层函数需要写DC时，此函数会被调用
  *       用户需要根据参数传入的值，将DC置为高电平或者低电平
  *       当参数传入0时，置DC为低电平，当参数传入1时，置DC为高电平
  */
void OLED_W_DC(uint8_t BitValue)
{
	/*根据BitValue的值，将DC置高电平或者低电平*/
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)BitValue);
}

/**
  * @brief OLED写CS高低电平
  * @param BitValue 要写入CS的电平值，范围：0/1
  * @note 当上层函数需要写CS时，此函数会被调用
  *       用户需要根据参数传入的值，将CS置为高电平或者低电平
  *       当参数传入0时，置CS为低电平，当参数传入1时，置CS为高电平
  */
void OLED_W_CS(uint8_t BitValue)
{
	/*根据BitValue的值，将CS置高电平或者低电平*/
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)BitValue);
}

/**
  * @brief OLED引脚初始化
  * @param 无
  * @note 当上层函数需要初始化时，此函数会被调用
  *       用户需要将D0、D1、RES、DC和CS引脚初始化为推挽输出模式
  */
void OLED_GPIO_Init(void)
{
	uint32_t i, j;
	
	/*在初始化前，加入适量延时，待OLED供电稳定*/
	for (i = 0; i < 1000; i ++)
	{
		for (j = 0; j < 1000; j ++);
	}
	
	/*将D0、D1、RES、DC和CS引脚初始化为推挽输出模式*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*置引脚默认电平*/
	OLED_W_D0(0);
	OLED_W_D1(1);
	OLED_W_RES(1);
	OLED_W_DC(1);
	OLED_W_CS(1);
}

/*----------------------通信协议----------------------*/

/**
  * @brief SPI发送一个字节
  * @param Byte 要发送的一个字节数据，范围：0x00~0xFF
  */
void OLED_SPI_SendByte(uint8_t Byte)
{
	uint8_t i;
	
	/*循环8次，主机依次发送数据的每一位*/
	for (i = 0; i < 8; i++)
	{
		/*使用掩码的方式取出Byte的指定一位数据并写入到D1线*/
		/*两个!的作用是，让所有非零的值变为1*/
		OLED_W_D1(!!(Byte & (0x80 >> i)));
		OLED_W_D0(1);	//拉高D0，从机在D0上升沿读取SDA
		OLED_W_D0(0);	//拉低D0，主机开始发送下一位数据
	}
}

/**
  * @brief OLED写命令
  * @param Command 要写入的命令值，范围：0x00~0xFF
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_W_CS(0);					//拉低CS，开始通信
	OLED_W_DC(0);					//拉低DC，表示即将发送命令
	OLED_SPI_SendByte(Command);		//写入指定命令
	OLED_W_CS(1);					//拉高CS，结束通信
}

/**
  * @brief OLED写数据
  * @param Data 要写入数据的起始地址
  * @param Count 要写入数据的数量
  */
void OLED_WriteData(uint8_t *Data, uint8_t Count)
{
	uint8_t i;
	
	OLED_W_CS(0);					//拉低CS，开始通信
	OLED_W_DC(1);					//拉高DC，表示即将发送数据
	/*循环Count次，进行连续的数据写入*/
	for (i = 0; i < Count; i ++)
	{
		OLED_SPI_SendByte(Data[i]);	//依次发送Data的每一个数据
	}
	OLED_W_CS(1);					//拉高CS，结束通信
}

/*----------------------通信协议----------------------*/


/*----------------------硬件配置----------------------*/

/**
  * @brief OLED初始化
  * @param 无
  * @note 使用屏幕前，需要调用此初始化函数
  */
void OLED_Init(void)
{
    OLED_GPIO_Init();			//先调用底层的端口初始化

    /*写入一系列的命令，对OLED进行初始化配置*/
    OLED_WriteCommand(0xAE); // 关屏指令

    OLED_WriteCommand(0xD5); // Set display clock divide ratio/oscillator freq
    OLED_WriteCommand(0xE1); // recommended value for SSD1317 reference

    OLED_WriteCommand(0xA8); // Set multiplex ratio
    OLED_WriteCommand(0x5F); // 95 -> 96 rows

    OLED_WriteCommand(0xD3); // Set display offset
    OLED_WriteCommand(0x00); // 0

    OLED_WriteCommand(0x40); // Set display start line = 0

    OLED_WriteCommand(0xA1); // 0xA1 = column 地址反向映射（SEG127 → column 0），即水平翻转显示。
    OLED_WriteCommand(0xC0); // 控制 COM 输出的扫描（上下翻转）方向。0xC0 或 0xC8

    OLED_WriteCommand(0xDA); // COM pins hardware configuration
    OLED_WriteCommand(0x12); // recommended

    OLED_WriteCommand(0x81); // 对比度调节指令
    OLED_WriteCommand(0x40); // 0x00 - 0xFF

    OLED_WriteCommand(0xD9); // Pre-charge period
    OLED_WriteCommand(0xF1); // recommended

    OLED_WriteCommand(0xDB); // VCOMH deselect level
    OLED_WriteCommand(0x30); // recommended

    OLED_WriteCommand(0xA4); // 0xA4 = 显示 GDDRAM 的内容（正常）；0xA5 = 强制点亮（忽略 GDDRAM）。
    OLED_WriteCommand(0xA6); // 0xA6 正常（0 = off, 1 = on），0xA7 反色（0→on，1→off）

    OLED_WriteCommand(0x8D); // 充电泵
    OLED_WriteCommand(0x14);

    OLED_Clear();
    OLED_WriteCommand(0xAF); // 开屏指令
}

/**
  * @brief SSD1317命令函数，控制硬件使屏幕正常或反色显示
  * @param invert 1：反色显示，0：正常显示
  */
void OLED_InvertDisplay(uint8_t invert) {
    if (invert) {
        OLED_WriteCommand(0xA7);  // 反色显示
    } else {
        OLED_WriteCommand(0xA6);  // 正常显示
    }
}

/*----------------------光标与更新----------------------*/
/**
  * @brief 设置页 (page) 与列 (X)
  * @param Page 页地址，范围：0-11
  * @param X_user 用户指定的列地址，范围：0-95
  * @note SSD1317 使用： 0xB0 | page
  *                     col_high = ((X & 0xF0) >> 4) + 0x11
  *                     col_low  = (X & 0x0F) + 0x00
  */
void OLED_SetCursor(uint8_t Page, uint8_t X_user)
{
		const int col_offset = 16; // 模组硬件向右偏移16列 硬件屏幕的0列-95列物理连接到GDDRAM的16列-111列
		int col = X_user + col_offset;        // 映射到 GDDRAM 列
		if (col > 127) col = 127;             // 强制在 GDDRAM 范围内
		OLED_WriteCommand(0xB0 | (Page & 0x0F));     // 页选择 "& 0x0F"的目的是防止越界溢出变成别的命令
    OLED_WriteCommand(0x10 | ((col >> 4) & 0x0F)); // 高 4bit
    OLED_WriteCommand(0x00 | (col & 0x0F));        // 低 4bit
}

/*----------------------硬件配置----------------------*/


/*----------------------工具函数(工具函数仅供内部部分函数使用)----------------------*/

/**
  * @brief 次方函数
  * @param X 底数
  * @param Y 指数
  * @return 等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//结果默认为1
	while (Y --)			//累乘Y次
	{
		Result *= X;		//每次把X累乘到结果上
	}
	return Result;
}

/**
  * @brief 判断指定点是否在指定多边形内部
  * @param nvert 多边形的顶点数
  * @param vertx verty 包含多边形顶点的x和y坐标的数组
  * @param testx testy 测试点的X和y坐标
  * @return 指定点是否在指定多边形内部，1：在内部，0：不在内部
  */
uint8_t OLED_pnpoly(uint8_t nvert, int16_t *vertx, int16_t *verty, int16_t testx, int16_t testy)
{
	int16_t i, j, c = 0;
	
	/*此算法由W. Randolph Franklin提出*/
	/*参考链接：https://wrfranklin.org/Research/Short_Notes/pnpoly.html*/
	for (i = 0, j = nvert - 1; i < nvert; j = i++)
	{
		if (((verty[i] > testy) != (verty[j] > testy)) &&
			(testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
		{
			c = !c;
		}
	}
	return c;
}

/**
  * @brief 判断指定点是否在指定角度内部
  * @param X Y 指定点的坐标
  * @param StartAngle EndAngle 起始角度和终止角度，范围：-180~180
  *           水平向右为0度，水平向左为180度或-180度，下方为正数，上方为负数，顺时针旋转
  * @return 指定点是否在指定角度内部，1：在内部，0：不在内部
  */
uint8_t OLED_IsInAngle(int16_t X, int16_t Y, int16_t StartAngle, int16_t EndAngle)
{
	int16_t PointAngle;
	PointAngle = atan2((double)Y, (double)X) / 3.14 * 180;	//计算指定点的弧度，并转换为角度表示
	if (StartAngle < EndAngle)	//起始角度小于终止角度的情况
	{
		/*如果指定角度在起始终止角度之间，则判定指定点在指定角度*/
		if (PointAngle >= StartAngle && PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	else			//起始角度大于于终止角度的情况
	{
		/*如果指定角度大于起始角度或者小于终止角度，则判定指定点在指定角度*/
		if (PointAngle >= StartAngle || PointAngle <= EndAngle)
		{
			return 1;
		}
	}
	return 0;		//不满足以上条件，则判断判定指定点不在指定角度
}

/*----------------------工具函数----------------------*/


/*----------------------功能函数----------------------*/

/**
  * @brief 将OLED显存数组更新到OLED屏幕,全屏更新：按页写入，每页 OLED_WIDTH 字节
  * @param 无
  * @return 无
  * @note 所有的显示函数，都只是对OLED显存数组进行读写
  *       随后调用OLED_Update函数或OLED_UpdateArea函数
  *       才会将显存数组的数据发送到OLED硬件，进行显示
  *       故调用显示函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_Update(void)
{
    uint8_t j;
    for (j = 0; j < OLED_PAGES; j++)
    {
        OLED_SetCursor(j, 0);
        OLED_WriteData(OLED_DisplayBuf[j], OLED_WIDTH);
    }
}

/**
  * @brief 将OLED显存数组部分更新到OLED屏幕,局部更新（自动向上取整页）
  * @param X 指定区域左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定区域左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Width 指定区域的宽度，范围：0~128
  * @param Height 指定区域的高度，范围：0~64
  * @return 无
  * @note 此函数会至少更新参数指定的区域
  *           如果更新区域Y轴只包含部分页，则同一页的剩余部分会跟随一起更新
  * @note 所有的显示函数，都只是对OLED显存数组进行读写
  *       随后调用OLED_Update函数或OLED_UpdateArea函数
  *       才会将显存数组的数据发送到OLED硬件，进行显示
  *       故调用显示函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_UpdateArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
    int16_t Page, Page1;
    int16_t j;

    Page = Y / 8;
    Page1 = (Y + Height - 1) / 8 + 1;
    if (Y < 0) { Page -= 1; Page1 -= 1; }

    for (j = Page; j < Page1; j++)
    {
        if (X >= 0 && X < OLED_WIDTH && j >= 0 && j < OLED_PAGES)
        {
            OLED_SetCursor((uint8_t)j, (uint8_t)X);
            OLED_WriteData(&OLED_DisplayBuf[j][X], Width);
        }
    }
}

/**
  * @brief 将OLED显存(缓存)数组全部清零
  * @param 无
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_Clear(void)
{
    uint8_t page, col;
    for (page = 0; page < OLED_PAGES; page++)
    {
        for (col = 0; col < OLED_WIDTH; col++)
        {
            OLED_DisplayBuf[page][col] = 0x00;
        }
    }
}
/**
  * @brief 将OLED显存(缓存)数组部分清零
  * @param X 指定区域左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定区域左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Width 指定区域的宽度，范围：0~128
  * @param Height 指定区域的高度，范围：0~64
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_ClearArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
    int16_t i, j;
    for (j = Y; j < Y + Height; j++)
    {
        for (i = X; i < X + Width; i++)
        {
            if (i >= 0 && i < OLED_WIDTH && j >= 0 && j < OLED_HEIGHT)
            {
                OLED_DisplayBuf[j / 8][i] &= ~(0x01 << (j % 8));
            }
        }
    }
}
/**
  * @brief 将OLED显存(缓存)数组全部取反
  * @param 无
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_Reverse(void)
{
    uint8_t p, c;
    for (p = 0; p < OLED_PAGES; p++)
    {
        for (c = 0; c < OLED_WIDTH; c++)
        {
            OLED_DisplayBuf[p][c] ^= 0xFF;
        }
    }
}	
/**
  * @brief 将OLED显存(缓存)数组部分取反
  * @param X 指定区域左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定区域左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Width 指定区域的宽度，范围：0~128
  * @param Height 指定区域的高度，范围：0~64
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_ReverseArea(int16_t X, int16_t Y, uint8_t Width, uint8_t Height)
{
    int16_t i, j;
    for (j = Y; j < Y + Height; j++)
    {
        for (i = X; i < X + Width; i++)
        {
            if (i >= 0 && i < OLED_WIDTH && j >= 0 && j < OLED_HEIGHT)
            {
                OLED_DisplayBuf[j / 8][i] ^= (0x01 << (j % 8));
            }
        }
    }
}


//设置绘图模式
/**
  * @brief 设置OLED绘图模式
  * @param mode 绘图模式: OLED_DRAWMODE_NORMAL 或 OLED_DRAWMODE_XOR
  * @return 无
  */
void OLED_SetDrawMode(OLED_DrawMode mode) {
    oled_draw_mode = mode;
}

/**
  * @brief OLED在指定位置画一个点
  * @param X 指定点的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定点的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param operation 操作指令:
  *                 - NORMAL模式: 1=设置为白色, 0=设置为黑色
  *                 - XOR模式: 1=翻转像素状态, 0=强制设置为黑色
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_DrawPoint(int16_t x, int16_t y, uint8_t operation) {
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT)
        return;
    
    uint8_t page = y / 8;
    uint8_t bit  = y % 8;
    uint8_t mask = 1 << bit;

    switch (oled_draw_mode) {
        case OLED_DRAWMODE_NORMAL:
            if (operation) {
                OLED_DisplayBuf[page][x] |= mask;   // 画白点
            } else {
                OLED_DisplayBuf[page][x] &= ~mask;  // 清黑点
            }
            break;

        case OLED_DRAWMODE_XOR:
            if (operation) {
                OLED_DisplayBuf[page][x] ^= mask;   // 翻转像素
            } else {
                OLED_DisplayBuf[page][x] &= ~mask;  // 强制清零
            }
            break;
    }
}

/**
  * @brief OLED显示一个字符
  * @param X 指定字符左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定字符左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Char 指定要显示的字符，范围：ASCII码可见字符
  * @param FontSize 指定字体大小
  *           范围：OLED_8X16		宽8像素，高16像素
  *                 OLED_6X8		宽6像素，高8像素
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  * @note 此函数只作用了字符取模时被点亮的像素，而不是整个6x8或8x16区域的所有像素。具体来说：
  *       函数从字体数组（如OLED_F6x8或OLED_F8x16）中读取字符的位图数据。
  *       位图数据中的每一位（bit）对应一个像素：如果位为1，则调用OLED_DrawPoint绘制像素（使用opration=1）；如果位为0，则不做任何操作，因此背景像素保持不变。
  *       这意味着只有字符形状所需的像素会被绘制，区域中的其他像素不会被修改，从而保持屏幕原有的背景色。
  *       意味着在NORMAL模式下，字符形状所需的像素会被点亮，而在XOR模式下，字符形状所需的像素会被异或操作。
  */
void OLED_ShowChar(int16_t X, int16_t Y, char Char, uint8_t FontSize)
{
    uint8_t i, j;
    
    switch (FontSize)
    {
    case OLED_8X16:
        for (i = 0; i < 8; i++) // 遍历8列
        {
            // 获取字符上半部分和下半部分的字模数据
            uint8_t temp_top = OLED_F8x16[Char - ' '][i];
            uint8_t temp_bottom = OLED_F8x16[Char - ' '][i + 8];
            
            // 绘制上半部分（8个像素）
            for (j = 0; j < 8; j++)
            {
                if (temp_top & 0x01) // 检查最低位是否为1
                {
                    OLED_DrawPoint(X + i, Y + j, 1); // 绘制像素
                }
                temp_top >>= 1; // 右移一位，检查下一个像素
            }
            
            // 绘制下半部分（8个像素）
            for (j = 0; j < 8; j++)
            {
                if (temp_bottom & 0x01) // 检查最低位是否为1
                {
                    OLED_DrawPoint(X + i, Y + j + 8, 1); // 绘制像素
                }
                temp_bottom >>= 1; // 右移一位，检查下一个像素
            }
        }
        break;
        
    case OLED_6X8:
        for (i = 0; i < 6; i++) // 遍历6列
        {
            // 获取字符的字模数据
            uint8_t temp = OLED_F6x8[Char - ' '][i];
            
            // 绘制8个像素
            for (j = 0; j < 8; j++)
            {
                if (temp & 0x01) // 检查最低位是否为1
                {
                    OLED_DrawPoint(X + i, Y + j, 1); // 绘制像素
                }
                temp >>= 1; // 右移一位，检查下一个像素
            }
        }
        break;
        
    default:
        break;
    }
}
/**
  * @brief OLED显示字符串（纯 ASCII）
  * @param X 指定字符串左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定字符串左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param String 指定要显示的字符串，范围：ASCII码可见字符
  * @param FontSize 指定字体大小
  *           范围：OLED_8X16		宽8像素，高16像素
  *                 OLED_6X8		宽6像素，高8像素
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数;
  *       在NORMAL模式下，字符形状所需的像素会被点亮，而在XOR模式下，字符形状所需的像素会被异或操作。
**/
void OLED_ShowString(int16_t X, int16_t Y, char *String, uint8_t FontSize)
{
    uint16_t i = 0;
    uint16_t XOffset = 0;
    while (String[i] != '\0')
    {
        OLED_ShowChar(X + XOffset, Y, String[i], FontSize);
        XOffset += FontSize;
        i++;
    }
}
/**
  * @brief OLED显示数字（十进制，正整数）
  * @param X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Number 指定要显示的数字，范围：0~4294967295
  * @param Length 指定数字的长度，范围：0~10
  * @param FontSize 指定字体大小
  *           范围：OLED_8X16		宽8像素，高16像素
  *                 OLED_6X8		宽6像素，高8像素
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数;
  *       在NORMAL模式下，字符形状所需的像素会被点亮，而在XOR模式下，字符形状所需的像素会被异或操作。
  */
void OLED_ShowNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        char ch = Number / OLED_Pow(10, Length - i - 1) % 10 + '0';
        OLED_ShowChar(X + i * FontSize, Y, ch, FontSize);
    }
}
/**
  * @brief OLED显示有符号数字（十进制，整数）
  * @param X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Number 指定要显示的数字，范围：-2147483648~2147483647
  * @param Length 指定数字的长度，范围：0~10
  * @param FontSize 指定字体大小
  *           范围：OLED_8X16		宽8像素，高16像素
  *                 OLED_6X8		宽6像素，高8像素
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数;
  *       在NORMAL模式下，字符形状所需的像素会被点亮，而在XOR模式下，字符形状所需的像素会被异或操作。
  */
void OLED_ShowSignedNum(int16_t X, int16_t Y, int32_t Number, uint8_t Length, uint8_t FontSize)
{
    uint32_t Number1;
    if (Number >= 0)
    {
        OLED_ShowChar(X, Y, '+', FontSize);
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(X, Y, '-', FontSize);
        Number1 = (uint32_t)(-Number);
    }
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        char ch = Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0';
        OLED_ShowChar(X + (i + 1) * FontSize, Y, ch, FontSize);
    }
}

/**
  * @brief OLED显示十六进制数字（十六进制，正整数）
  * @param X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Number 指定要显示的数字，范围：0x00000000~0xFFFFFFFF
  * @param Length 指定数字的长度，范围：0~8
  * @param FontSize 指定字体大小
  *           范围：OLED_8X16		宽8像素，高16像素
  *                 OLED_6X8		宽6像素，高8像素
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数;
  *       在NORMAL模式下，字符形状所需的像素会被点亮，而在XOR模式下，字符形状所需的像素会被异或操作。
  */
void OLED_ShowHexNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)		//遍历数字的每一位
	{
		/*以十六进制提取数字的每一位*/
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		
		if (SingleNumber < 10)			//单个数字小于10
		{
			/*调用OLED_ShowChar函数，显示此数字*/
			/*+ '0' 可将数字转换为字符格式*/
			OLED_ShowChar(X + i * FontSize, Y, SingleNumber + '0', FontSize);
		}
		else							//单个数字大于10
		{
			/*调用OLED_ShowChar函数，显示此数字*/
			/*+ 'A' 可将数字转换为从A开始的十六进制字符*/
			OLED_ShowChar(X + i * FontSize, Y, SingleNumber - 10 + 'A', FontSize);
		}
	}
}

/**
  * @brief OLED显示二进制数字（二进制，正整数）
  * @param X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Number 指定要显示的数字，范围：0x00000000~0xFFFFFFFF
  * @param Length 指定数字的长度，范围：0~16
  * @param FontSize 指定字体大小
  *           范围：OLED_8X16		宽8像素，高16像素
  *                 OLED_6X8		宽6像素，高8像素
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数;
  *       在NORMAL模式下，字符形状所需的像素会被点亮，而在XOR模式下，字符形状所需的像素会被异或操作。
  */
void OLED_ShowBinNum(int16_t X, int16_t Y, uint32_t Number, uint8_t Length, uint8_t FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)		//遍历数字的每一位	
	{
		/*调用OLED_ShowChar函数，依次显示每个数字*/
		/*Number / OLED_Pow(2, Length - i - 1) % 2 可以二进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
		OLED_ShowChar(X + i * FontSize, Y, Number / OLED_Pow(2, Length - i - 1) % 2 + '0', FontSize);
	}
}

/**
  * @brief OLED显示浮点数字（十进制，小数）
  * @param X 指定数字左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定数字左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Number 指定要显示的数字，范围：-4294967295.0~4294967295.0
  * @param IntLength 指定数字的整数位长度，范围：0~10
  * @param FraLength 指定数字的小数位长度，范围：0~9，小数进行四舍五入显示
  * @param FontSize 指定字体大小
  *           范围：OLED_8X16		宽8像素，高16像素
  *                 OLED_6X8		宽6像素，高8像素
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数;
  *       在NORMAL模式下，字符形状所需的像素会被点亮，而在XOR模式下，字符形状所需的像素会被异或操作。
  */
void OLED_ShowFloatNum(int16_t X, int16_t Y, double Number, uint8_t IntLength, uint8_t FraLength, uint8_t FontSize)
{
	uint32_t PowNum, IntNum, FraNum;
	
	if (Number >= 0)						//数字大于等于0
	{
		OLED_ShowChar(X, Y, '+', FontSize);	//显示+号
	}
	else									//数字小于0
	{
		OLED_ShowChar(X, Y, '-', FontSize);	//显示-号
		Number = -Number;					//Number取负
	}
	
	/*提取整数部分和小数部分*/
	IntNum = Number;						//直接赋值给整型变量，提取整数
	Number -= IntNum;						//将Number的整数减掉，防止之后将小数乘到整数时因数过大造成错误
	PowNum = OLED_Pow(10, FraLength);		//根据指定小数的位数，确定乘数
	FraNum = round(Number * PowNum);		//将小数乘到整数，同时四舍五入，避免显示误差
	IntNum += FraNum / PowNum;				//若四舍五入造成了进位，则需要再加给整数
	
	/*显示整数部分*/
	OLED_ShowNum(X + FontSize, Y, IntNum, IntLength, FontSize);
	
	/*显示小数点*/
	OLED_ShowChar(X + (IntLength + 1) * FontSize, Y, '.', FontSize);
	
	/*显示小数部分*/
	OLED_ShowNum(X + (IntLength + 2) * FontSize, Y, FraNum, FraLength, FontSize);
}

/**
  * @brief OLED使用printf函数打印格式化字符串
  * @param X 指定格式化字符串左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定格式化字符串左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param FontSize 指定字体大小
  *           范围：OLED_8X16		宽8像素，高16像素
  *                 OLED_6X8		宽6像素，高8像素
  * @param format 指定要显示的格式化字符串，范围：ASCII码可见字符组成的字符串
  * @param ... 格式化字符串参数列表
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数;
  *       在NORMAL模式下，字符形状所需的像素会被点亮，而在XOR模式下，字符形状所需的像素会被异或操作。
  */
void OLED_Printf(int16_t X, int16_t Y, uint8_t FontSize, char *format, ...)
{
	char String[256];						//定义字符数组
	va_list arg;							//定义可变参数列表数据类型的变量arg
	va_start(arg, format);					//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);			//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);							//结束变量arg
	OLED_ShowString(X, Y, String, FontSize);//OLED显示字符数组（字符串）
}

/**
  * @brief OLED获取指定位置点的值
  * @param X 指定点的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定点的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @return 指定位置点是否处于点亮状态，1：点亮，0：熄灭
  */
uint8_t OLED_GetPoint(int16_t X, int16_t Y)
{
    if (X >= 0 && X < OLED_WIDTH && Y >= 0 && Y < OLED_HEIGHT)
    {
        if (OLED_DisplayBuf[Y / 8][X] & (0x01 << (Y % 8))) return 1;
    }
    return 0;
}

/**
  * @brief OLED画线
  * @param X0 指定一个端点的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y0 指定一个端点的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param X1 指定另一个端点的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y1 指定另一个端点的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param IncludeEndpoints 是否包含端点（0:不包含，1:包含）
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_DrawLine(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, uint8_t IncludeEndpoints, uint8_t operation)
{
    // 特殊处理：水平线
    if (Y0 == Y1)
    {
        int16_t start = X0, end = X1;
        if (start > end) { int16_t temp = start; start = end; end = temp; }
        
        if (!IncludeEndpoints) { start++; end--; }
        
        for (int16_t x = start; x <= end; x++)
        {
            OLED_DrawPoint(x, Y0, operation);
        }
        return;
    }
    
    // 特殊处理：垂直线
    if (X0 == X1)
    {
        int16_t start = Y0, end = Y1;
        if (start > end) { int16_t temp = start; start = end; end = temp; }
        
        if (!IncludeEndpoints) { start++; end--; }
        
        for (int16_t y = start; y <= end; y++)
        {
            OLED_DrawPoint(X0, y, operation);
        }
        return;
    }
    
    // 通用情况：斜线（使用Bresenham算法）
    int16_t x, y, dx, dy, d, incrE, incrNE, temp;
    int16_t x0 = X0, y0 = Y0, x1 = X1, y1 = Y1;
    uint8_t yflag = 0, xyflag = 0;
    
    if (x0 > x1)
    {
        temp = x0; x0 = x1; x1 = temp;
        temp = y0; y0 = y1; y1 = temp;
    }
    
    if (y0 > y1)
    {
        y0 = -y0;
        y1 = -y1;
        yflag = 1;
    }
    
    if (y1 - y0 > x1 - x0)
    {
        temp = x0; x0 = y0; y0 = temp;
        temp = x1; x1 = y1; y1 = temp;
        xyflag = 1;
    }
    
    dx = x1 - x0;
    dy = y1 - y0;
    incrE = 2 * dy;
    incrNE = 2 * (dy - dx);
    d = 2 * dy - dx;
    x = x0;
    y = y0;
    
    // 绘制起始点（如果包含端点）
    if (IncludeEndpoints)
    {
        if (yflag && xyflag){OLED_DrawPoint(y, -x, operation);}
        else if (yflag)        {OLED_DrawPoint(x, -y, operation);}
        else if (xyflag)    {OLED_DrawPoint(y, x, operation);}
        else                {OLED_DrawPoint(x, y, operation);}
    }
    
    while (x < x1)
    {
        x++;
        if (d < 0)
        {
            d += incrE;
        }
        else
        {
            y++;
            d += incrNE;
        }
        
        // 跳过最后一个点（如果不包含端点）
        if (!IncludeEndpoints && x == x1) break;
        
        // 绘制点
        if (yflag && xyflag){OLED_DrawPoint(y, -x, operation);}
        else if (yflag)        {OLED_DrawPoint(x, -y, operation);}
        else if (xyflag)    {OLED_DrawPoint(y, x, operation);}
        else                {OLED_DrawPoint(x, y, operation);}
    }
}

/**
  * @brief OLED矩形
  * @param X 指定矩形左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定矩形左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Width 指定矩形的宽度，范围：0~128
  * @param Height 指定矩形的高度，范围：0~64
  * @param IsFilled 指定矩形是否填充
  *           范围：OLED_UNFILLED		不填充
  *                 OLED_FILLED			填充
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_DrawRectangle(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, uint8_t IsFilled, uint8_t operation)
{
    int16_t i, j;
    if (!IsFilled)
    {
        // 绘制上边（包含端点）
        OLED_DrawLine(X, Y, X + Width - 1, Y, 1, operation);
        // 绘制下边（包含端点）
        OLED_DrawLine(X, Y + Height - 1, X + Width - 1, Y + Height - 1, 1, operation);
        // 绘制左边（不包含端点，避免重复绘制角点）
        OLED_DrawLine(X, Y, X, Y + Height - 1, 0, operation);
        // 绘制右边（不包含端点，避免重复绘制角点）
        OLED_DrawLine(X + Width - 1, Y, X + Width - 1, Y + Height - 1, 0, operation);
    }
    else
    {
        for (i = X; i < X + Width; i++)
        {
            for (j = Y; j < Y + Height; j++)
            {
                OLED_DrawPoint(i, j, operation);
            }
        }
    }
}

/**
  * @brief OLED三角形
  * @param X0 指定第一个端点的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y0 指定第一个端点的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param X1 指定第二个端点的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y1 指定第二个端点的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param X2 指定第三个端点的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y2 指定第三个端点的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param IsFilled 指定三角形是否填充
  *           范围：OLED_UNFILLED		不填充
  *                 OLED_FILLED			填充
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
void OLED_DrawTriangle(int16_t X0, int16_t Y0, int16_t X1, int16_t Y1, int16_t X2, int16_t Y2, uint8_t IsFilled, uint8_t operation)
{
    int16_t minx = X0, miny = Y0, maxx = X0, maxy = Y0;
    int16_t i, j;
    int16_t vx[] = {X0, X1, X2};
    int16_t vy[] = {Y0, Y1, Y2};
    
    if (!IsFilled)            //指定三角形不填充
    {
        // 使用自定义的线段绘制方法，避免顶点重复绘制
        // 绘制第一条边（从X0,Y0到X1,Y1），不包含终点
        OLED_DrawLine(X0, Y0, X1, Y1, 0, operation);
        // 绘制第二条边（从X0,Y0到X2,Y2），不包含终点
        OLED_DrawLine(X0, Y0, X2, Y2, 0, operation);
        // 绘制第三条边（从X1,Y1到X2,Y2），包含两个端点
        OLED_DrawLine(X1, Y1, X2, Y2, 1, operation);
    }
    else                    //指定三角形填充
    {
        /*找到三个点最小的X、Y坐标*/
        if (X1 < minx) {minx = X1;}
        if (X2 < minx) {minx = X2;}
        if (Y1 < miny) {miny = Y1;}
        if (Y2 < miny) {miny = Y2;}
        
        /*找到三个点最大的X、Y坐标*/
        if (X1 > maxx) {maxx = X1;}
        if (X2 > maxx) {maxx = X2;}
        if (Y1 > maxy) {maxy = Y1;}
        if (Y2 > maxy) {maxy = Y2;}
        
        /*最小最大坐标之间的矩形为可能需要填充的区域*/
        /*遍历此区域中所有的点*/
        /*遍历X坐标*/        
        for (i = minx; i <= maxx; i ++)
        {
            /*遍历Y坐标*/    
            for (j = miny; j <= maxy; j ++)
            {
                /*调用OLED_pnpoly，判断指定点是否在指定三角形之中*/
                /*如果在，则画点，如果不在，则不做处理*/
                if (OLED_pnpoly(3, vx, vy, i, j)) {OLED_DrawPoint(i, j, operation);}
            }
        }
    }
}

/**
  * @brief OLED画圆
  * @param X 指定圆的圆心横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定圆的圆心纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Radius 指定圆的半径，范围：0~255
  * @param IsFilled 指定圆是否填充
  *           范围：OLED_UNFILLED		不填充
  *                 OLED_FILLED			填充
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数;在XOR模式下，无论operation传入0或1，都执行1的逻辑.
  */
// ---------- 辅助函数：在一次迭代内只画“唯一像素”的八对称点（避免同一次调用里重复画同一点） ----------
static inline void plot8_unique(int16_t cx, int16_t cy, int16_t x, int16_t y, uint8_t operation)
{
    // 第一组 (±x, ±y)
    OLED_DrawPoint(cx + x, cy + y, operation);
    if (x != 0)               OLED_DrawPoint(cx - x, cy + y, operation);
    if (y != 0)               OLED_DrawPoint(cx + x, cy - y, operation);
    if (x != 0 && y != 0)     OLED_DrawPoint(cx - x, cy - y, operation);

    // 第二组 (±y, ±x) —— 仅当 x != y 时需要（避免与第一组重复）
    if (x != y) {
        OLED_DrawPoint(cx + y, cy + x, operation);
        if (y != 0)           OLED_DrawPoint(cx - y, cy + x, operation);
        if (x != 0)           OLED_DrawPoint(cx + y, cy - x, operation);
        if (x != 0 && y != 0) OLED_DrawPoint(cx - y, cy - x, operation);
    }
}

// ---------- 主函数：可靠、对称、XOR 安全的圆 （替换原来版本） ----------
void OLED_DrawCircle(int16_t X, int16_t Y, uint8_t Radius, uint8_t IsFilled, uint8_t operation)
{
    if (Radius == 0) {
        OLED_DrawPoint(X, Y, operation);
        return;
    }

    // ---------- NORMAL 模式：保持高效 Bresenham（空心）与水平线填充（实心） ----------
    if (oled_draw_mode == OLED_DRAWMODE_NORMAL) {
        int16_t x = 0, y = Radius;
        int16_t d = 1 - Radius;

        while (x <= y) {
            if (!IsFilled) {
                // 空心：直接画八对称（可能会在不同迭代出现重复像素，但 NORMAL 模式下 |= 幂等）
                plot8_unique(X, Y, x, y, operation);
            } else {
                // 实心：用水平线（更快也保证对称）
                OLED_DrawLine(X - x, Y + y, X + x, Y + y,1, operation);
                OLED_DrawLine(X - x, Y - y, X + x, Y - y,1, operation);
                OLED_DrawLine(X - y, Y + x, X + y, Y + x,1, operation);
                OLED_DrawLine(X - y, Y - x, X + y, Y - x,1, operation);
            }

            if (d < 0) d += 2 * x + 3;
            else { d += 2 * (x - y) + 5; y--; }
            x++;
        }
        return;
    }

    // ---------- XOR 模式：关键在于“每个像素只操作一次” ----------
    // 在 XOR 模式下，operation 参数 should be 1 to mean "toggle".  (如果传 operation==0 将保持行为为 "clear")
    if (oled_draw_mode == OLED_DRAWMODE_XOR) {
        uint8_t tog = 1; // 翻转操作

        // -------- 空心圆（Bresenham）: 用 plot8_unique 避免在同一次迭代里重复绘点 --------
        if (!IsFilled) {
            int16_t x = 0, y = Radius;
            int16_t d = 1 - Radius;

            while (x <= y) {
                // plot8_unique 保证在 x==0 或 x==y 等情况下不重复调用 DrawPoint 导致双重 XOR
                plot8_unique(X, Y, x, y, tog);

                if (d < 0) d += 2 * x + 3;
                else { d += 2 * (x - y) + 5; y--; }
                x++;
            }
            return;
        }

        // -------- 实心圆（扫描线填充）: 第一步 - 扫描线给内部每个像素都 XOR 一次 --------
        // 用“四舍五入取 dx”保证上/下对称性
        for (int16_t dy = - (int16_t)Radius; dy <= (int16_t)Radius; ++dy) {
            int32_t t = (int32_t)Radius * Radius - (int32_t)dy * dy;
            if (t < 0) continue;
            int16_t dx = (int16_t)(sqrtf((float)t) + 0.5f); // round for symmetry
            for (int16_t xx = -dx; xx <= dx; ++xx) {
                OLED_DrawPoint(X + xx, Y + dy, tog);
            }
        }

        // -------- 第二步 - 边界修正：用 Bresenham 产生的边界点来补画“扫描线未包含”的边界像素 --------
        // 对每个边界点 (bx,by) 计算该行的 dx_round，并仅在 |bx| > dx_round 时再做一次 XOR（即那些扫描线漏掉的边界像素）
        // 同时在单次迭代里避免重复绘制（用类似 plot8_unique 的分组逻辑）
        int16_t x = 0, y = Radius;
        int16_t d = 1 - Radius;

        while (x <= y) {
            // 第一组 (±x, ±y)
            {
                int16_t bx = x, by = y;
                int32_t tt = (int32_t)Radius * Radius - (int32_t)by * by;
                if (tt >= 0) {
                    int16_t dx_line = (int16_t)(sqrtf((float)tt) + 0.5f);
                    if (abs(bx) > dx_line) OLED_DrawPoint(X + bx, Y + by, tog);
                    if (abs(-bx) > dx_line && bx != 0) OLED_DrawPoint(X - bx, Y + by, tog);

                    by = -y;
                    tt = (int32_t)Radius * Radius - (int32_t)by * by;
                    dx_line = (int16_t)(sqrtf((float)tt) + 0.5f);
                    if (abs(bx) > dx_line) OLED_DrawPoint(X + bx, Y + by, tog);
                    if (abs(-bx) > dx_line && bx != 0) OLED_DrawPoint(X - bx, Y + by, tog);
                }
            }

            // 第二组 (±y, ±x) —— 仅当 x != y
            if (x != y) {
                int16_t bx = y, by = x;
                int32_t tt = (int32_t)Radius * Radius - (int32_t)by * by;
                if (tt >= 0) {
                    int16_t dx_line = (int16_t)(sqrtf((float)tt) + 0.5f);
                    if (abs(bx) > dx_line) OLED_DrawPoint(X + bx, Y + by, tog);
                    if (abs(-bx) > dx_line && bx != 0) OLED_DrawPoint(X - bx, Y + by, tog);

                    by = -x;
                    tt = (int32_t)Radius * Radius - (int32_t)by * by;
                    dx_line = (int16_t)(sqrtf((float)tt) + 0.5f);
                    if (abs(bx) > dx_line) OLED_DrawPoint(X + bx, Y + by, tog);
                    if (abs(-bx) > dx_line && bx != 0) OLED_DrawPoint(X - bx, Y + by, tog);
                }
            }

            if (d < 0) d += 2 * x + 3;
            else { d += 2 * (x - y) + 5; y--; }
            x++;
        }

        return;
    }

    // 如果有其它模式（防御）
    return;
}

/**
  * @brief OLED画椭圆
  * @param X 指定椭圆的圆心横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定椭圆的圆心纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param A 指定椭圆的横向半轴长度，范围：0~255
  * @param B 指定椭圆的纵向半轴长度，范围：0~255
  * @param IsFilled 指定椭圆是否填充
  *           范围：OLED_UNFILLED		不填充
  *                 OLED_FILLED			填充
  * @return 无
  * @note 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数;在XOR模式下，无论operation传入0或1，都执行1的逻辑.
  */
// ---------- 辅助函数：在一次迭代内只画“唯一像素”的四对称点（避免同一次调用里重复画同一点） ----------
static inline void plot4_unique(int16_t cx, int16_t cy, int16_t x, int16_t y, uint8_t col)
{
    // (±x, ±y)
    OLED_DrawPoint(cx + x, cy + y, col);
    if (x != 0)              OLED_DrawPoint(cx - x, cy + y, col);
    if (y != 0)              OLED_DrawPoint(cx + x, cy - y, col);
    if (x != 0 && y != 0)    OLED_DrawPoint(cx - x, cy - y, col);
}

// ---------- 辅助函数：计算给定垂直偏移 |dy| 时，扫描线应覆盖的最大水平半宽 dx（四舍五入） ----------
static inline int16_t ellipse_scanline_dx(int16_t A, int16_t B, int16_t dy_abs)
{
    if (B == 0) return 0;
    float ratio = ((float)dy_abs * (float)dy_abs) / ((float)B * (float)B);
    if (ratio >= 1.0f) return 0;
    return (int16_t)( (float)A * sqrtf(1.0f - ratio) + 0.5f );
}

// ---------- 辅助函数：边界修正时对一组对称点按行判断是否需要补画 ----------
// 若边界点 (bx, by) 的 |bx| > scanline_dx(|by|)，说明该点不在扫描填充区域内，需补画。
// 这个函数会按四对称把需要补的点都画出来（一次迭代里避免重复）。
static inline void maybe_plot4_boundary(int16_t cx, int16_t cy,
                                        int16_t bx, int16_t by,
                                        int16_t A, int16_t B, uint8_t col)
{
    int16_t dy_abs = (int16_t) ( (by < 0) ? -by : by );
    int16_t dx_line = ellipse_scanline_dx(A, B, dy_abs);

    // 只有当 abs(bx) > dx_line 时，这个边界点是被扫描线漏掉的，需要补画
    if ( (bx < 0 ? -bx : bx) > dx_line ) {
        // 四对称（±bx, ±by），但使用条件避免重复
        OLED_DrawPoint(cx + bx, cy + by, col);
        if (bx != 0)              OLED_DrawPoint(cx - bx, cy + by, col);
        if (by != 0)              OLED_DrawPoint(cx + bx, cy - by, col);
        if (bx != 0 && by != 0)   OLED_DrawPoint(cx - bx, cy - by, col);
    }
}

// ---------- 主函数：OLED_DrawEllipse ----------
// A = 半长轴 (x 方向), B = 半短轴 (y 方向)
// IsFilled = 0 -> 空心, !=0 -> 实心
// operation 在 NORMAL 模式里直接写入；在 XOR 模式里我们把每次写入当作 "toggle"（翻转）操作（使用 col=1）
void OLED_DrawEllipse(int16_t X, int16_t Y, uint8_t A, uint8_t B, uint8_t IsFilled, uint8_t operation)
{
    // 处理退化情况
    if (A == 0 && B == 0) {
        OLED_DrawPoint(X, Y, operation);
        return;
    }
    if (A == 0) {
        // 竖线
        if (IsFilled) {
            OLED_DrawLine(X, Y - B, X, Y + B, 1,operation);
        } else {
            OLED_DrawPoint(X, Y - B, operation);
            OLED_DrawPoint(X, Y + B, operation);
        }
        return;
    }
    if (B == 0) {
        // 横线
        if (IsFilled) {
            OLED_DrawLine(X - A, Y, X + A, Y, 1,operation);
        } else {
            OLED_DrawPoint(X - A, Y, operation);
            OLED_DrawPoint(X + A, Y, operation);
        }
        return;
    }

    // ---------------- NORMAL 模式 ----------------
    if (oled_draw_mode == OLED_DRAWMODE_NORMAL) {
        // 空心：中点椭圆算法（整数） + plot4_unique
        // 实心：按行扫描（对称） 用 OLED_DrawLine（更快）
        int32_t a2 = (int32_t)A * A;
        int32_t b2 = (int32_t)B * B;
        int32_t x = 0, y = B;
        int32_t dx = 2 * b2 * x;
        int32_t dy = 2 * a2 * y;
        int32_t d1 = b2 - (int32_t)a2 * B + (a2 >> 2); // b^2 - a^2*B + a^2/4  (approx int)
        // region 1
        while (dx < dy) {
            if (!IsFilled) {
                plot4_unique(X, Y, (int16_t)x, (int16_t)y, operation);
            } else {
                // 对应四个镜像行，用线段填充更高效
                OLED_DrawLine(X - (int16_t)x, Y + (int16_t)y, X + (int16_t)x, Y + (int16_t)y, 1,operation);
                OLED_DrawLine(X - (int16_t)x, Y - (int16_t)y, X + (int16_t)x, Y - (int16_t)y, 1,operation);
            }

            if (d1 < 0) {
                x++;
                dx += 2 * b2;
                d1 += dx + b2;
            } else {
                x++; y--;
                dx += 2 * b2;
                dy -= 2 * a2;
                d1 += dx - dy + b2;
            }
        }
        // region 2
        float xf = x + 0.5f;
        float yf = y - 1.0f;
        int32_t d2 = (int32_t)(b2 * xf * xf + a2 * yf * yf - a2 * b2);
        while (y >= 0) {
            if (!IsFilled) {
                plot4_unique(X, Y, (int16_t)x, (int16_t)y, operation);
            } else {
                OLED_DrawLine(X - (int16_t)x, Y + (int16_t)y, X + (int16_t)x, Y + (int16_t)y, 1,operation);
                OLED_DrawLine(X - (int16_t)x, Y - (int16_t)y, X + (int16_t)x, Y - (int16_t)y,1, operation);
            }

            if (d2 > 0) {
                y--;
                dy -= 2 * a2;
                d2 += a2 - dy;
            } else {
                y--; x++;
                dx += 2 * b2;
                dy -= 2 * a2;
                d2 += dx - dy + a2;
            }
        }
        return;
    }

    // ---------------- XOR 模式 ----------------
    if (oled_draw_mode == OLED_DRAWMODE_XOR) {
        uint8_t tog = 1; // 在 XOR 下，把每次绘制视为“翻转”（调用 OLED_DrawPoint(...,1)）
        // 空心（hollow）: 用中点椭圆 + plot4_unique（一次迭代内保证不重复写同一点）
        if (!IsFilled) {
            int32_t a2 = (int32_t)A * A;
            int32_t b2 = (int32_t)B * B;
            int32_t x = 0, y = B;
            int32_t dx = 2 * b2 * x;
            int32_t dy = 2 * a2 * y;
            int32_t d1 = b2 - (int32_t)a2 * B + (a2 >> 2);
            while (dx < dy) {
                plot4_unique(X, Y, (int16_t)x, (int16_t)y, tog);
                if (d1 < 0) {
                    x++;
                    dx += 2 * b2;
                    d1 += dx + b2;
                } else {
                    x++; y--;
                    dx += 2 * b2;
                    dy -= 2 * a2;
                    d1 += dx - dy + b2;
                }
            }
            int32_t d2 = (int32_t)(b2 * (x + 0.5f) * (x + 0.5f) + a2 * (y - 1) * (y - 1) - a2 * b2);
            while (y >= 0) {
                plot4_unique(X, Y, (int16_t)x, (int16_t)y, tog);
                if (d2 > 0) {
                    y--;
                    dy -= 2 * a2;
                    d2 += a2 - dy;
                } else {
                    y--; x++;
                    dx += 2 * b2;
                    dy -= 2 * a2;
                    d2 += dx - dy + a2;
                }
            }
            return;
        }

        // 实心（filled）: 1) 扫描线填充（每像素只写一次）  2) 边界修正用 mid-point 边界补那些扫描线没覆盖的点
        // 1) 扫描线填充
        for (int16_t dy = -(int16_t)B; dy <= (int16_t)B; ++dy) {
            int16_t dx = ellipse_scanline_dx((int16_t)A, (int16_t)B, (int16_t)(dy < 0 ? -dy : dy));
            for (int16_t xx = -dx; xx <= dx; ++xx) {
                OLED_DrawPoint(X + xx, Y + dy, tog);
            }
        }

        // 2) 边界修正：遍历 mid-point 椭圆边界点，补画扫描线漏掉的点
        {
            int32_t a2 = (int32_t)A * A;
            int32_t b2 = (int32_t)B * B;
            int32_t x = 0, y = B;
            int32_t dx = 2 * b2 * x;
            int32_t dy = 2 * a2 * y;
            int32_t d1 = b2 - (int32_t)a2 * B + (a2 >> 2);
            // region1
            while (dx < dy) {
                // maybe_plot4_boundary 只在扫描线未包含时补画对应对称点
                maybe_plot4_boundary(X, Y, (int16_t)x, (int16_t)y, (int16_t)A, (int16_t)B, tog);
                if (d1 < 0) {
                    x++;
                    dx += 2 * b2;
                    d1 += dx + b2;
                } else {
                    x++; y--;
                    dx += 2 * b2;
                    dy -= 2 * a2;
                    d1 += dx - dy + b2;
                }
            }
            int32_t d2 = (int32_t)(b2 * (x + 0.5f) * (x + 0.5f) + a2 * (y - 1) * (y - 1) - a2 * b2);
            while (y >= 0) {
                maybe_plot4_boundary(X, Y, (int16_t)x, (int16_t)y, (int16_t)A, (int16_t)B, tog);
                if (d2 > 0) {
                    y--;
                    dy -= 2 * a2;
                    d2 += a2 - dy;
                } else {
                    y--; x++;
                    dx += 2 * b2;
                    dy -= 2 * a2;
                    d2 += dx - dy + a2;
                }
            }
        }

        return;
    }

    // 其它不支持的绘制模式（防御）
    return;
}

/**
  * @brief OLED画圆弧
  * @param X 指定圆弧的圆心横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定圆弧的圆心纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Radius 指定圆弧的半径，范围：0~255
  * @param StartAngle 指定圆弧的起始角度，范围：-180~180
  * @param EndAngle 指定圆弧的终止角度，范围：-180~180
  * @param IsFilled 指定圆弧是否填充，填充后为扇形
  *           范围：OLED_UNFILLED		不填充
  *                     OLED_FILLED			填充
  * @return 无
  * @note 在XOR模式下，无论operation传入0或1，都执行1的逻辑.
  *					StartAngle, EndAngle
  *					角度定义：
  *					数学角度系：以 +X 轴（屏幕向右）为 0°，逆时针方向角度增加。
  *					支持任意整数，包括负数或大于 360°。
  *					会自动归一化到 [0, 360)：“不断加减 360，直到落在 [0,360)”。
  *					例：-90 → 270；450 → 90。
  *					扫过角度：函数会绘制从 StartAngle → EndAngle 的逆时针扇区。
  *					例如：
  *						(0, 180) → 上半圆。
  *						(90, 270) → 左半圆。
  *						(0, 360) → 整圆。
  *					包含规则（避免重叠/加粗）：
  *					包含起始边（StartAngle）上的像素。
  *					不包含结束边（EndAngle）上的像素。
  *					这样相邻的 (0,90) + (90,180) 不会重叠。
  *					调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数
  */
#ifndef RAD_PER_DEG
#define RAD_PER_DEG 0.017453292519943295f   // π/180
#endif

// 小常量：浮点比较容差
static const float EPS_ARC = 1e-6f;

// 绝对值
static inline int16_t iabs16(int16_t a){ return (a < 0) ? (int16_t)-a : a; }

/*----------辅助函数[计算某行的水平半宽(四舍五入，尽量平滑)]----------------*/
static inline int16_t circle_scan_dx(uint8_t R, int16_t dy_abs){
    int32_t t = (int32_t)R * (int32_t)R - (int32_t)dy_abs * (int32_t)dy_abs;
    if (t <= 0) return 0;
    return (int16_t)(sqrtf((float)t) + 0.5f);
}

/*----------辅助函数[精确处理轴向角度（0/90/180/270），其它角度用 cos/sin]----------------*/
static inline void angle_to_unit_exact(int ang_deg, float *ux, float *uy){
    int a = ang_deg % 360; if (a < 0) a += 360;
    switch (a){
        case 0:   *ux = 1.0f;  *uy = 0.0f;  break;
        case 90:  *ux = 0.0f;  *uy = 1.0f;  break;
        case 180: *ux = -1.0f; *uy = 0.0f;  break;
        case 270: *ux = 0.0f;  *uy = -1.0f; break;
        default: {
            float r = (float)a * RAD_PER_DEG;
            *ux = cosf(r);
            *uy = sinf(r);
        } break;
    }
}

/*----------辅助函数[精确扇区判定（含起点、排除终点），使用 epsilon 避免浮点误差]----------------*/
/*
 精确扇区判定（含起点、排除终点），使用 epsilon 避免浮点误差：
 - 输入 dx,dy 为像素相对坐标（整数）；屏幕坐标下 y 向下，
   转换到数学坐标 p = (dx, -dy) 做叉积判定。
 - sx,sy / ex,ey 为起点/终点单位方向向量（数学坐标），且轴向值已精确处理。
 - sweep_deg = 扫过角度（0..359），若为0表示整圆（full circle）。
 判定规则（和之前保持一致）：
 - 若点在起始射线上（cross≈0 且 dot>=0） -> 包含
 - 若点在结束射线上（cross≈0 且 dot>=0） -> 排除
 - 否则根据 sweep<=180 / >180 使用交集/补集判定（并使用 EPS）
*/
static inline int in_wedge_precise(int16_t dx, int16_t dy,
                                   float sx, float sy, float ex, float ey,
                                   int sweep_deg)
{
    if (sweep_deg == 0) return 1; // full circle

    // 数学坐标 p = (dx, -dy)
    float px = (float)dx;
    float py = (float)(-dy);

    // cross 和 dot
    float cs = sx * py - sy * px;   // s x p
    float ce = px * ey - py * ex;   // p x e
    float ds = sx * px + sy * py;   // s · p
    float de = ex * px + ey * py;   // e · p

    int on_start = (fabsf(cs) <= EPS_ARC && ds >= -EPS_ARC);
    int on_end   = (fabsf(ce) <= EPS_ARC && de >= -EPS_ARC);

    if (sweep_deg <= 180) {
        if (on_start) return 1;
        if (on_end)   return 0;
        return (cs > EPS_ARC && ce > EPS_ARC);
    } else {
        if (on_start) return 1;
        if (on_end)   return 0;
        return !(cs < -EPS_ARC && ce < -EPS_ARC);
    }
}

/*----------辅助函数[在一轮 (x,y) 上，把 8 对称点去重后逐个测试角度并绘制（用于空心或补边界）]----------------*/
static inline void plot_octants_unique_arc(int16_t cx, int16_t cy,
                                           int16_t x, int16_t y,
                                           float sx, float sy, float ex, float ey,
                                           int sweep_deg, uint8_t op)
{
    int16_t candx[8] = { (int16_t)x, (int16_t)-x, (int16_t)x, (int16_t)-x, (int16_t)y, (int16_t)-y, (int16_t)y, (int16_t)-y };
    int16_t candy[8] = { (int16_t)y, (int16_t)y, (int16_t)-y, (int16_t)-y, (int16_t)x, (int16_t)x, (int16_t)-x, (int16_t)-x };

    for (int i = 0; i < 8; ++i){
        int dup = 0;
        for (int k = 0; k < i; ++k){
            if (candx[i] == candx[k] && candy[i] == candy[k]) { dup = 1; break; }
        }
        if (dup) continue;

        if (in_wedge_precise(candx[i], candy[i], sx, sy, ex, ey, sweep_deg)){
            OLED_DrawPoint(cx + candx[i], cy + candy[i], op);
        }
    }
}

/* ---------------- 主函数：OLED_DrawArc（替换用） ----------------
   X,Y: 圆心；Radius: 半径
   StartAngle, EndAngle: 数学角度（+X=0°, 逆时针）
   IsFilled: 0=空心（画弧）；非0=实心（扇形）
   color: NORMAL 下颜色；XOR 下忽略，使用翻转 op=1
*/
void OLED_DrawArc(int16_t X, int16_t Y,
                  uint8_t Radius,
                  int16_t StartAngle, int16_t EndAngle,
                  uint8_t IsFilled, uint8_t color)
{
    if (Radius == 0) {
        if (IsFilled) OLED_DrawPoint(X, Y, (oled_draw_mode==OLED_DRAWMODE_XOR)?1:color);
        return;
    }

    // 角度归一化到 [0,360)
    int a0 = StartAngle % 360; if (a0 < 0) a0 += 360;
    int a1 = EndAngle   % 360; if (a1 < 0) a1 += 360;

    // 扫角（逆时针），a1==a0 视为整圆
    int sweep = a1 - a0; if (sweep < 0) sweep += 360;

    // 单位向量（轴向角度精确）
    float sx, sy, ex, ey;
    angle_to_unit_exact(a0, &sx, &sy);
    angle_to_unit_exact(a1, &ex, &ey);

    // XOR 模式
    if (oled_draw_mode == OLED_DRAWMODE_XOR) {
        uint8_t op = 1; // toggle

        if (!IsFilled) {
            // 空心：midpoint circle + 去重 + 精确扇区判定
            int16_t x = 0, y = Radius;
            int16_t d = 1 - (int16_t)Radius;
            while (x <= y) {
                plot_octants_unique_arc(X, Y, x, y, sx, sy, ex, ey, sweep, op);
                if (d < 0) d += 2*x + 3;
                else { d += 2*(x - y) + 5; y--; }
                x++;
            }
            return;
        }

        // 实心扇形：扫描线填充（每像素只写一次），再做外圈补点（只补扫描线未覆盖的点）
        for (int16_t dy = -(int16_t)Radius; dy <= (int16_t)Radius; ++dy) {
            int16_t dx = circle_scan_dx(Radius, iabs16(dy));
            for (int16_t xx = -dx; xx <= dx; ++xx) {
                if (in_wedge_precise(xx, dy, sx, sy, ex, ey, sweep)) {
                    OLED_DrawPoint(X + xx, Y + dy, op);
                }
            }
        }

        // 外圈补点（补扫描线没覆盖的边界点）
        {
            int16_t x = 0, y = Radius;
            int16_t d = 1 - (int16_t)Radius;
            while (x <= y) {
                int16_t candx[8] = { (int16_t)x, (int16_t)-x, (int16_t)x, (int16_t)-x, (int16_t)y, (int16_t)-y, (int16_t)y, (int16_t)-y };
                int16_t candy[8] = { (int16_t)y, (int16_t)y, (int16_t)-y, (int16_t)-y, (int16_t)x, (int16_t)x, (int16_t)-x, (int16_t)-x };
                for (int i = 0; i < 8; ++i) {
                    int dup = 0;
                    for (int k = 0; k < i; ++k) {
                        if (candx[i]==candx[k] && candy[i]==candy[k]) { dup=1; break; }
                    }
                    if (dup) continue;

                    if (!in_wedge_precise(candx[i], candy[i], sx, sy, ex, ey, sweep)) continue;

                    int16_t yabs = iabs16(candy[i]);
                    int16_t dx_line = circle_scan_dx(Radius, yabs);
                    if (iabs16(candx[i]) > dx_line) {
                        OLED_DrawPoint(X + candx[i], Y + candy[i], op);
                    }
                }
                if (d < 0) d += 2*x + 3;
                else { d += 2*(x - y) + 5; y--; }
                x++;
            }
        }

        return;
    }

    // NORMAL 模式
    if (!IsFilled) {
        int16_t x = 0, y = Radius;
        int16_t d = 1 - (int16_t)Radius;
        while (x <= y) {
            plot_octants_unique_arc(X, Y, x, y, sx, sy, ex, ey, sweep, color);
            if (d < 0) d += 2*x + 3;
            else { d += 2*(x - y) + 5; y--; }
            x++;
        }
    } else {
        for (int16_t dy = -(int16_t)Radius; dy <= (int16_t)Radius; ++dy) {
            int16_t dx = circle_scan_dx(Radius, iabs16(dy));
            for (int16_t xx = -dx; xx <= dx; ++xx) {
                if (in_wedge_precise(xx, dy, sx, sy, ex, ey, sweep)) {
                    OLED_DrawPoint(X + xx, Y + dy, color);
                }
            }
        }
    }
}

/**
  * @brief 绘制圆角矩形
  * @param x, y: 矩形左上角坐标
  * @param width, height: 矩形的宽度和高度
  * @param radius: 圆角半径（像素）
  * @param isFilled: 是否填充 (OLED_FILLED 或 OLED_UNFILLED)
  * @note 实现原理：
  *       实心模式：绘制四个角的填充圆形
  *               绘制中间的矩形区域填充剩余部分
  *       空心模式：绘制四个角的圆弧
  *               绘制四条直线连接圆弧
  *
  * @note 边界处理：
  *       自动限制圆角半径不超过矩形尺寸的一半
  *       确保所有坐标在屏幕范围内
  */
/*----------辅助函数[画水平线（含端点），会自动处理 x0>x1 的情况]----------------*/
static inline void _hline(int16_t x0, int16_t x1, int16_t y, uint8_t op)
{
    if (x1 < x0) { int16_t t = x0; x0 = x1; x1 = t; }
    for (int16_t x = x0; x <= x1; ++x) OLED_DrawPoint(x, y, op);
}

/*----------辅助函数[画垂直线（含端点），会自动处理 y0>y1 的情况]----------------*/
static inline void _vline(int16_t x, int16_t y0, int16_t y1, uint8_t op)
{
    if (y1 < y0) { int16_t t = y0; y0 = y1; y1 = t; }
    for (int16_t y = y0; y <= y1; ++y) OLED_DrawPoint(x, y, op);
}

/*----------辅助函数[计算某一行相对圆心的垂直偏移（绝对值）对应的水平半宽（四舍五入）]----------------*/
static inline int16_t _dx_round(uint8_t r, int16_t dy_abs)
{
    int32_t t = (int32_t)r * (int32_t)r - (int32_t)dy_abs * (int32_t)dy_abs;
    if (t <= 0) return 0;
    return (int16_t)(sqrtf((float)t) + 0.5f);
}

/*-----------------主函数-------------------------*/
/**
 * 圆角矩形（round rect）
 * x, y: 左上角
 * width, height: 宽高（>=1）
 * radius: 圆角半径（自动夹到 [0, min(width,height)/2]）
 * isFilled: 0=空心，非0=实心
 * operation: NORMAL 模式下的颜色；XOR 模式将忽略该值，使用翻转（1）
 *
 * 依赖：新版 OLED_DrawArc(cx, cy, r, a0, a1, isFilled, color)
 * 角度：数学角度（+X为0°，逆时针增加），起始边“包含”，结束边“不包含”
 */
void OLED_DrawRoundRect(int16_t x, int16_t y,
                        uint16_t width, uint16_t height,
                        uint8_t radius,
                        uint8_t isFilled, uint8_t operation)
{
    if (width == 0 || height == 0) return;

    // 半径夹紧
    uint16_t minwh = (width < height) ? width : height;
    if ((uint16_t)radius * 2U > minwh) radius = (uint8_t)(minwh / 2U);

    // r=0 → 普通矩形
    if (radius == 0) {
        if (isFilled) {
            for (int16_t yy = y; yy < (int16_t)(y + height); ++yy) {
                _hline(x, x + (int16_t)width - 1, yy,
                       (oled_draw_mode == OLED_DRAWMODE_XOR) ? 1 : operation);
            }
        } else {
            uint8_t op = (oled_draw_mode == OLED_DRAWMODE_XOR) ? 1 : operation;
            _hline(x, x + (int16_t)width - 1, y, op);
            _hline(x, x + (int16_t)width - 1, y + (int16_t)height - 1, op);
            _vline(x, y, y + (int16_t)height - 1, op);
            _vline(x + (int16_t)width - 1, y, y + (int16_t)height - 1, op);
        }
        return;
    }

    // ------------ 实心：扫描线一遍搞定（XOR下每像素只写一次） ------------
    if (isFilled) {
        uint8_t op = (oled_draw_mode == OLED_DRAWMODE_XOR) ? 1 : operation;
        int16_t xL = (int16_t)x;
        int16_t xR = (int16_t)x + (int16_t)width - 1;
        int16_t y0 = (int16_t)y;
        int16_t y1 = (int16_t)y + (int16_t)height - 1;

        int16_t cxL_top  = x + radius;                 // 左上圆心 X
        int16_t cy_top   = y + radius;                 // 左上/右上 圆心 Y
        int16_t cxR_top  = x + (int16_t)width - radius - 1;

        int16_t cxL_bot  = x + radius;                 // 左下圆心 X
        int16_t cy_bot   = y + (int16_t)height - radius - 1;
        int16_t cxR_bot  = x + (int16_t)width - radius - 1;

        for (int16_t yy = y0; yy <= y1; ++yy) {
            int16_t xmin = xL;
            int16_t xmax = xR;

            // 顶部圆角行
            if (yy < cy_top) {
                int16_t dy = cy_top - yy;                 // 相对上圆心的 |dy|
                int16_t dx = _dx_round(radius, dy);
                xmin = cxL_top - dx;
                xmax = cxR_top + dx;
            }
            // 底部圆角行
            else if (yy > cy_bot) {
                int16_t dy = yy - cy_bot;                 // 相对下圆心的 |dy|
                int16_t dx = _dx_round(radius, dy);
                xmin = cxL_bot - dx;
                xmax = cxR_bot + dx;
            }
            // 中间直边行：xmin/xmax 已经是整行

            if (xmin < xL) xmin = xL;
            if (xmax > xR) xmax = xR;
            if (xmin <= xmax) _hline(xmin, xmax, yy, op);
        }
        return;
    }

    // ------------ 空心：四个 1/4 圆 + 四条直边（避免 XOR 重翻转） ------------
    // 四角圆心
    int16_t cx_tl = x + radius;                       // top-left
    int16_t cy_tl = y + radius;
    int16_t cx_tr = x + (int16_t)width - radius - 1;  // top-right
    int16_t cy_tr = y + radius;
    int16_t cx_bl = x + radius;                       // bottom-left
    int16_t cy_bl = y + (int16_t)height - radius - 1;
    int16_t cx_br = x + (int16_t)width - radius - 1;  // bottom-right
    int16_t cy_br = y + (int16_t)height - radius - 1;

    uint8_t arc_col = (oled_draw_mode == OLED_DRAWMODE_XOR) ? 1 : operation;

    // 四个圆角：角度按数学定义（+X=0°，逆时针增长）；起始边含、结束边不含
    // 左上：  90..180
    OLED_DrawArc(cx_tl, cy_tl, radius, 90, 180, 0, arc_col);
    // 右上：   0..90
    OLED_DrawArc(cx_tr, cy_tr, radius, 0, 90, 0, arc_col);
    // 左下： 180..270
    OLED_DrawArc(cx_bl, cy_bl, radius, 180, 270, 0, arc_col);
    // 右下： 270..360
    OLED_DrawArc(cx_br, cy_br, radius, 270, 360, 0, arc_col);

    // 直边：为避免 XOR 与圆角像素重叠翻转，端点做“谁含谁不含”的对齐：
    // 规则（与 Arc 一致：含起始/不含结束）：
    // - 顶边：左端(90°)由左上弧“包含”，右端(90°)由右上弧“不包含”
    //   → 顶边画 [x+r+1 .. x+w-r-1]
    // - 底边：右端(270°)由右下弧“包含”，左端(270°)由左下弧“不包含”
    //   → 底边画 [x+r .. x+w-r-2]
    // - 左边：底端(180°)由左下弧“包含”，顶端(180°)由左上弧“不包含”
    //   → 左边画 [y+r .. y+h-r-2]
    // - 右边：顶端(0°)由右上弧“包含”，底端(0°)由右下弧“不包含”
    //   → 右边画 [y+r+1 .. y+h-r-1]
    uint8_t edge_col = (oled_draw_mode == OLED_DRAWMODE_XOR) ? 1 : operation;

    // 顶边 y = y
    {
        int16_t xs = x + (int16_t)radius + 1;
        int16_t xe = x + (int16_t)width - (int16_t)radius - 1;
        if (xs <= xe) _hline(xs, xe, y, edge_col);
    }
    // 底边 y = y+height-1
    {
        int16_t xs = x + (int16_t)radius;
        int16_t xe = x + (int16_t)width - (int16_t)radius - 2;
        int16_t yy = y + (int16_t)height - 1;
        if (xs <= xe) _hline(xs, xe, yy, edge_col);
    }
    // 左边 x = x
    {
        int16_t ys = y + (int16_t)radius;
        int16_t ye = y + (int16_t)height - (int16_t)radius - 2;
        if (ys <= ye) _vline(x, ys, ye, edge_col);
    }
    // 右边 x = x+width-1
    {
        int16_t ys = y + (int16_t)radius + 1;
        int16_t ye = y + (int16_t)height - (int16_t)radius - 1;
        int16_t xx = x + (int16_t)width - 1;
        if (ys <= ye) _vline(xx, ys, ye, edge_col);
    }
}


/**
  * @brief OLED显示图像
  * @param X 指定图像左上角的横坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Y 指定图像左上角的纵坐标，范围：-32768~32767，屏幕区域：0~95
  * @param Width 指定图像的宽度，范围：0~96
  * @param Height 指定图像的高度，范围：0~96
  * @param Image 指定要显示的图像
  * @param IsMix 若传入0，则图像取模中未被点亮的的像素也会强制熄灭，不管该点像素之前是否点亮；若传入1，则图像取模中未被点亮的的像素不会对其操作，保留该点像素之前的状态。
  * @return 无
  * @brief 调用此函数后，要想真正地呈现在屏幕上，还需调用更新函数。oled_draw_mode与operation的值对其无效，该函数只遵循IsMix参数选择的模式的效果。
  *        算法介绍：https://www.bilibili.com/video/BV1EN41177Pc/?spm_id_from=333.1391.0.0&p=5&vd_source=3270faada43d444eec68d30700fef9b9  第19分钟开始
  * @note：Width与Height必须与图像参数一致（注意！是与图像的实际像素参数一致，而不是与给图像取模的像素一致，比如图像参数为宽10*高10，
  *       但是由于取模时高度必须为8的整数，所以取模的像素为宽10*高16，但是这里要传入的Width是10，Height是10），否则无法正常显示图像
  **/
void OLED_ShowImage(int16_t X, int16_t Y, uint8_t Width, uint8_t Height, const uint8_t *Image,unsigned char IsMix)
{
    uint8_t i = 0, j = 0;
    int16_t Page, Shift;
		
		if(!IsMix){
			/* 清空对应区域的显存位（产生覆盖效果）*/
			OLED_ClearArea(X, Y, Width, Height);
		}
		
    for (j = 0; j < (Height - 1) / 8 + 1; j++)
    {
        for (i = 0; i < Width; i++)
        {
            if (X + i >= 0 && X + i < OLED_WIDTH)//数组越界处理
            {
                Page = Y / 8;
                Shift = Y % 8;
                if (Y < 0) { Page -= 1; Shift += 8; }

                if (Page + j >= 0 && Page + j < OLED_PAGES)//数组越界处理
                {
                    OLED_DisplayBuf[Page + j][X + i] |= ((uint8_t)Image[j * Width + i]) << (Shift);
                }

                if (Page + j + 1 >= 0 && Page + j + 1 < OLED_PAGES)//数组越界处理
                {
                    OLED_DisplayBuf[Page + j + 1][X + i] |= ((uint8_t)Image[j * Width + i]) >> (8 - Shift);
                }
            }
        }
    }
}

/*------------------------功能函数------------------------*/

/************************ COPYRIGHT(C) 2026 WANG FANGZHUO **************************/
