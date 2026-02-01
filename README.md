# ArkUI

> 一个适用于嵌入式的轻量 UI 框架：  
> 用 `Page + Item + Layer(栈)` 表达菜单树，使用**链表**将所有page和item链在一起，用 **统一的插值光标 Cursor** 把所有页面变化、焦点与转场“连成一体。
>
> ![功能简要展示](功能简要展示.gif)

---

## 目录

- [0. 关于ArkUI](#0-关于ArkUI)
- [1. 硬件环境与设计目标](#1-硬件环境与设计目标)
- [2. 代码层级](#2-代码层级)
- [3. 部署ArkUI](#3-部署ArkUI)
- [4. 菜单注册](#4-菜单注册)
- [致谢](#致谢)
- [以后要改进的](#以后要改进的)

---

## 0. 关于ArkUI

本项目的雏形起源于我大一第一次参加电赛期间，当时就对能部署到单片机上的交互系统感兴趣，不仅方便直接在单片机上调参数而不依赖上位机修改代码也方便管理不同功能的模块。并且诸如稚晖君等大佬制作的各种拥有丝滑交互的开发板对我幼小心灵的震撼。于是就诞生了ArkUI，毕竟哪个工程师不想有一块又小又全能的板子呢是吧


---

## 1. 硬件环境与设计目标

### 1.1 基本硬件

- MCU：各种MPU（工程示例为 STM32F103c8t6，主频72MHz）
- 显示：一块屏幕（工程实例为SSD1317 96*96 SPI OLED）
- 输入：三个按键

### 1.2 ArkUI 解决的核心问题

1) **链表构成的菜单树表达**：`Page / Item` + `layer(栈)`  
2) **焦点一致性**：统一光标 `Cursor` 表达焦点，跨页面也连续  
3) **自定义事件扩展**：Event 能承载任意复杂功能界面（ark_enent.cpp中有示例的接砖块小游戏供于参考）  
4) **高实时系统可嵌入**：UI 不应阻塞控制环                                       

---

## 2. 代码层级


### 2.1 config（最底层依赖隔离层）

文件：
- `arkui_config.h / arkui_config.c`
- 依赖的物理层驱动：
  - OLED （工程实例：`dvc_arkoled.c / dvc_arkoled.h / dvc_oled_data.c / dvc_oled_data.h` 适配于SSD1317屏幕，也基本适配于SSD1306屏幕）
  - 按键 （工程实例：`dvc_arkey_refactor.cpp / dvc_arkey_refactor.h`模仿鼠标逻辑，支持最基本的按键上升沿、下降沿检测，也支持检测单击、多击、长按逻辑）

职责：
- **隔离 ArkUI 核心必须依赖的物理层**：绘图、刷新、按键状态、延时等。
- ArkUI 核心不直接调用硬件驱动，而是通过 config 的宏/封装函数调用。
- 在这里配置ArkUI的最基本参数，如屏幕宽高、动画速度、按键数量等

### 2.2 cursor（光标库）

文件：
- `arkui_cursor.h / arkui_cursor.cpp`

职责：
- 统一“焦点高亮”的视觉表达（矩形）
- 提供 `Goto / Goto_From / Update / Draw`，实现平滑插值与跨页面衔接
- **这是 ArkUI 的核心特点**：用光标把所有页面变化连起来

### 2.3 animation（动画库）

文件：
- `arkui_animation.h / arkui_animation.c`

职责：
- 页面过渡、背景模糊等动画效果

### 2.4 event（事件库）

文件：
- `arkui_event.h / arkui_event.cpp`

职责：
- 放自定义功能界面（item：参数修改、进度条、消息框、自定义小游戏等。page：icon类型页面）

### 2.5 register（注册库）

文件：
- `arkui_register.h / arkui_register.c`

职责：
- 注册整个菜单树（Page/Item 层级）
- 用 `ArkUIAddPage / ArkUIAddItem` 描述菜单结构

### 2.6 arkui（核心库）

文件：
- `arkui.h / arkui.c`

职责：
- 主循环（每帧）：渲染 + 输入分发 + 入栈/回退 + 事件模式切换
- List 页面渲染：位置计算、item 绘制、光标绘制、滚动条绘制

---

## 3. 部署ArkUI

### 3.1 需要实现的

1) OLED 驱动可用（`dvc_arkoled`）
2) 按键驱动可用（`dvc_arkey_refactor`）
3) `ArkUIInit()` 初始化一次
4) 周期调用：`ArkUIKeyActionMonitor()` + `ArkUI(dt_ms)`

### 3.2 推荐的主循环模板（示例的写法，当然肯定不是最好写法）

> 来自 `tsk_config_and_callback.cpp` 的模式：  
> **中断扫按键，前台 15ms 刷 UI**。

```cpp
// 1ms 定时中断里：扫按键 + 设置 UI 刷新标志
void TIM2_IRQHandler()
{
    //扫按键
	static unsigned char ark_Key_Cnt = 0;
    ark_Key_Cnt++;
    if(ark_Key_Cnt == 15){
        ark_Key_Cnt = 0;

        Class_ArkKey::ScanKeyState();
        Class_ArkKey::UserApp();
    }
    
	//设置UI刷新标志
    static unsigned char UI_Time = 0;
    UI_Time++;
    if(UI_Time == 15){
        UI_Time = 0; 
        
        uiUpdateFlag = 1;
    }
}

// 前台 loop：帧驱动 UI
void Task_Loop()
{
    if (uiUpdateFlag)
    {
        uiUpdateFlag = 0;

        ArkUIKeyActionMonitor();
        Class_ArkKey::ClearAllFlags();   // 关键：清瞬态标志要在 ArkUI() 前
        ArkUI(15);  // dt=15ms
    }
}
```

---

## 4. 菜单注册

### 4.1 注册入口：`ArkUI_RegisterMenu()`

在 `ArkUIInit()` 内部会调用：

```c
ArkUI_RegisterMenu();
```

只需要在 `arkui_register.c` 的 `ArkUI_RegisterMenu()` 里描述整个菜单树。

---

### 4.2 先注册 Page，再注册 Item

#### 4.2.1 注册 Page：`ArkUIAddPage(&page, type, ...)`

函数原型（见 `arkui.h`）：

```c
void ArkUIAddPage(ArkUIPage_t *page, ArkUIPage_e func, ...);
```

使用方式：

- **列表页**：
示例：
```c
ArkUIAddPage(&page, PAGE_LIST);
```
page是你事先定义好的一个ArkUIPage_t类型的变量。
**ArkUIAddPage(**在这里放入你事先定义好的那个ArkUIPage_t类型的变量的地址 **,**  这里就放PAGE_LIST这个枚举常量，它就代表了你注册的这个页是列表类型的页**);**

- **自定义页**：
示例：
```c
ArkUIAddPage(&page, PAGE_CUSTOM, IconPageEvent);
```
page是你事先定义好的一个ArkUIPage_t类型的变量。
 **ArkUIAddPage(**在这里放入你事先定义好的那个ArkUIPage_t类型的变量的地址   **,**    这里就放PAGE_CUSTOM这个枚举常量，它就代表了你注册的这个页是自定义类型的页    **,**    这里放自定义事件函数的地址，假设这个函数的地址就是IconPageEvent，那就放这个**);**



> `ArkUIAddPage` 会自动：
> 1) 为 page 分配 `id`（从 0 开始递增，越先被ArkUIAddPage的page变量所被分到的id编号就越靠前）  
> 2) 把 page 挂入 `pageHead/pageTail` 链表  
> 3) 初始化page变量自有的item 链表头尾指针

#### 4.2.2 注册 Item：`ArkUIAddItem(&page, &item, title, type, &args)`

函数原型（见 `arkui.h`）：

```c
void ArkUIAddItem(ArkUIPage_t *page,
                  ArkUIItem_t *item,
                  char *_title,
                  ArkUIItem_e func,
                  const ArkUIItemArgs_t *args);
```

其中 `ArkUIItemArgs_t` 是**统一参数包**：

```c
typedef struct
{
    const uint8_t *icon;     // 可选：图标
    const char *msg;         // 可选：message 文本
    bool *flag;              // 可选：switch/checkbox/radio 的状态指针
    paramType *param;        // 可选：change/progress 的参数指针
    uint8_t pageId;          // 可选：jump page 目标页
    void (*Event)(ArkUIItem_t *item); // 可选：事件回调
    void *userData;          // 可选：扩展槽
} ArkUIItemArgs_t;
```

使用方式：

实例：

```c
ArkUIItemArgs_t args; //先定义一个参数包变量，用于后面用它来传参数，就像stm32标准库初始化外设那样

memset(&args, 0, sizeof(args));//习惯性把这个变量清一下，防止脏数据，虽然不清一般也没事
args.pageId = page.id;
ArkUIAddItem(&page1, &item, "[xxx]", ITEM_JUMP_PAGE, &args);
```
item是你事先定义的一个ArkUIItem_t类型的变量。
假设你想让这个菜单项的作用当作是进入其它页的入口，那它的类型就是ITEM_JUMP_PAGE这个枚举量。
假设你想让它当作page页的入口，那就令**args.pageId =** page这个变量的id。

**ArkUIAddItem(**假设你这个item是在page1这个页中的，那这里就填page1的地址  **,**  在这里放入你事先定义好的那个ArkUIItem_t类型的变量item的地址  **, **  **"**[这里放你想让这个item展示的标题，用于显示在屏幕UI上]**"**   **, **  这里就放ITEM_JUMP_PAGE这个枚举常量，它就代表了你定义的这个列表是跳转类型的列表**, **  这放你定义那个参数包的地址**);**


---

### 4.3 Item 类型注册写法模板

#### A) 跳转到某页：`ITEM_JUMP_PAGE`

```c
memset(&args, 0, sizeof(args));
args.pageId = page2.id;   // 目标页 id
args.icon   = icon_param; // icon 页可选，假如说jump_to_p2这个列表是挂在图形化页面中的列表，那就给他一个图标，用于展示
ArkUIAddItem(&page0, &jump_to_p2, "[Value]", ITEM_JUMP_PAGE, &args);
```

#### B) 开关：`ITEM_SWITCH`

```c
memset(&args, 0, sizeof(args));
args.flag = &sw1;//假设你想让这个列表项的作用是控制某个bool类型的变量的真假值，这里就放那个bool变量的地址
ArkUIAddItem(&page1, &Sw_for_sw1, "Switch 1", ITEM_SWITCH, &args);
```

#### C) 单选：`ITEM_RADIO_BUTTON`

```c
memset(&args, 0, sizeof(args));
args.flag = &rb1;//假设你想让这个列表项的作用是控制某个bool类型的变量的真假值，这里就放那个bool变量的地址
ArkUIAddItem(&page4, &Rb_for_rb1, "button 1", ITEM_RADIO_BUTTON, &args);
```

#### D) 复选：`ITEM_CHECKBOX`

```c
memset(&args, 0, sizeof(args));
args.flag = &ch1;//假设你想让这个列表项的作用是控制某个bool类型的变量的真假值，这里就放那个bool变量的地址
ArkUIAddItem(&page5, &Ch_for_ch1, "Checkbox 1", ITEM_CHECKBOX, &args);
```

#### E) 修改参数：`ITEM_CHANGE_VALUE`

```c
memset(&args, 0, sizeof(args));
args.param = &Uint1;//假设你想让这个列表项的作用是控制某个变量的值，这里就放那个变量的地址。变量的类型可选，根据你挂的那个Event而定
args.Event = ArkUIEventChangeUint;//假设这个列表控制的是Uint类型的变量，那就挂这个能调节Uint变量的值的函数的地址
ArkUIAddItem(&page2, &par_for_Uint1, "uint1", ITEM_CHANGE_VALUE, &args);
```

#### F) 进度条：`ITEM_PROGRESS_BAR`

```c
memset(&args, 0, sizeof(args));
args.param = &Bar1;//假设你想展示某个变量的百分比，那这里就放那个变量的地址。一般这个变量的范围只有0-100，这样才符合百分比
args.Event = ArkFunctionExit;   // 这里用于“按退出返回”
ArkUIAddItem(&page3, &par_for_Bar1, "Bar1", ITEM_PROGRESS_BAR, &args);
```

#### G) 消息框：`ITEM_MESSAGE`

```c
memset(&args, 0, sizeof(args));
args.msg   = Massge1;//假设你想展示某个信息，那你就让某个变量储存这个信息，然后把这个变量的地址放在这里
args.Event = ArkFunctionExit;   // 同样用于退出返回
ArkUIAddItem(&page6, &mas_for_Massge1, "Mas", ITEM_MESSAGE, &args);
```

#### H) 自定义事件：`ITEM_CUSTOM`

```c
memset(&args, 0, sizeof(args));
args.Event = ArkUIEventCatchBrick;//放你想运行的那个自定义事件函数的地址
args.icon  = icon_CatchBrick;
ArkUIAddItem(&page7, &game_entry, "CatchBrick", ITEM_CUSTOM, &args);
```

---

##  致谢

- **感谢 B 站大佬 `ErBW_s`**：ArkUI 的整体框架思想与菜单模型参考/基于其开源的 **Easy-UI**。  
  ArkUI 在此基础上强化了“统一光标 Cursor 贯穿页面变化”的设计，并扩展了 Page/Item 类型与 Event 自定义能力。
- **感谢 江协科技**：本仓库的 `arkoled` 显示驱动库基于其开源 OLED 库改造而来。
  arkoled在此基础上添加了相关绘图函数，绘图模式等功能，并提高了代码整体的鲁棒性。
- **借鉴的开源项目与逻辑**：
  - B 站大佬 **“无理造物”** 的开源 **astra UI**
  - B 站大佬 **“怕火的凤凰”** 的开源 **WouoUI**
  - B 站大佬 **“看见请喊我去温书”** 的开源 **HOPE** 开发板
- **感谢麻神（TrojanGeneric）的文件架构的分层管理思想**

---

## 以后要改进的

- 目前少量动画仍有 Delay，把所有 Delay 干掉，让动画/转场/弹窗全部变成“逐帧推进、可打断、完全实时
- 把屏幕刷新抽出来，固定刷新率刷新屏幕，oled底层用上DMA
