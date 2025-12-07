# Sherlock-13 游戏 / Sherlock-13 Game

## 项目简介 / Project Overview

**Sherlock-13** 是一个基于福尔摩斯主题的多人推理游戏，使用C语言和SDL2图形库实现。游戏采用客户端-服务器架构，支持4名玩家同时在线对战。游戏的目标是通过推理找出13个角色中的嫌疑人。

**Sherlock-13** is a Sherlock Holmes-themed multiplayer deduction game implemented in C with SDL2 graphics library. It uses a client-server architecture supporting 4 players simultaneously. The goal is to deduce which of the 13 characters is the culprit.

---

## 游戏规则 / Game Rules

### 角色和物品 / Characters and Objects

**13个角色 / 13 Characters:**
1. Sebastian Moran (塞巴斯蒂安·莫兰)
2. irene Adler (艾琳·艾德勒)
3. inspector Lestrade (雷斯垂德警官)
4. inspector Gregson (葛雷森警官)
5. inspector Baynes (贝恩斯警官)
6. inspector Bradstreet (布拉德斯特里特警官)
7. inspector Hopkins (霍普金斯警官)
8. Sherlock Holmes (夏洛克·福尔摩斯)
9. John Watson (约翰·华生)
10. Mycroft Holmes (麦考夫·福尔摩斯)
11. Mrs. Hudson (赫德森太太)
12. Mary Morstan (玛丽·摩斯坦)
13. James Moriarty (詹姆斯·莫里亚蒂)

**8个物品符号 / 8 Object Symbols:**
0. Pipe (烟斗)
1. Light bulb (灯泡)
2. Fist (拳头)
3. Crown (王冠)
4. Notebook (笔记本)
5. Necklace (项链)
6. Eye (眼睛)
7. Skull (骷髅)

### 游戏机制 / Game Mechanics

1. **游戏设置 / Setup:**
   - 13张角色卡被分配：每位玩家获得3张卡，最后1张是"罪犯"（保密）
   - 每张角色卡关联特定的物品符号
   - 玩家可以看到自己手中的3张卡对应的物品数量

2. **玩家行动 / Player Actions:**
   玩家每回合可以选择以下三种行动之一：
   
   a) **询问物品 (O命令) / Symbol Query:**
      - 询问其他玩家是否拥有某个特定物品
      - 如果某玩家没有该物品，所有人都能看到结果
      - 如果某玩家有该物品，只有该玩家自己能看到确切数量
   
   b) **询问玩家物品数 (S命令) / Count Query:**
      - 询问特定玩家拥有某物品的具体数量
      - 所有玩家都能看到结果
   
   c) **指认嫌疑人 (G命令) / Make Accusation:**
      - 指认某个角色为罪犯
      - 如果正确，该玩家获胜；如果错误，该玩家被淘汰

3. **胜利条件 / Win Conditions:**
   - 正确指认出罪犯的玩家获胜
   - 如果3名玩家被淘汰，剩余玩家自动获胜

---

## 技术架构 / Technical Architecture

### 系统结构 / System Architecture

```
┌─────────────┐      ┌─────────────┐
│   Client 1  │      │   Client 2  │
│  (sh13.c)   │      │  (sh13.c)   │
└──────┬──────┘      └──────┬──────┘
       │                    │
       │    TCP/IP          │
       └────────┬───────────┘
                │
         ┌──────▼──────┐
         │   Server    │
         │ (server.c)  │
         └──────┬──────┘
                │
       ┌────────┴────────┐
       │                 │
┌──────▼──────┐   ┌──────▼──────┐
│   Client 3  │   │   Client 4  │
│  (sh13.c)   │   │  (sh13.c)   │
└─────────────┘   └─────────────┘
```

### 服务器端 (server.c) / Server Side

- **功能 / Functions:**
  - 管理4个客户端连接
  - 洗牌并分配角色卡
  - 处理游戏状态转换
  - 广播游戏信息给所有玩家
  - 验证玩家行动的合法性

- **主要协议消息 / Protocol Messages:**
  - `C`: 客户端连接请求 / Client connection
  - `I`: 分配玩家ID / Assign player ID
  - `L`: 玩家列表 / Player list
  - `D`: 分发卡牌 / Distribute cards
  - `M`: 当前回合玩家 / Current player turn
  - `V`: 表格值更新 / Table value update
  - `F`: 游戏结束 / Game finish
  - `G`: 指认嫌疑人 / Guess culprit
  - `O`: 询问物品 / Object query
  - `S`: 询问计数 / Specific count query

### 客户端 (sh13.c) / Client Side

- **技术栈 / Technology Stack:**
  - SDL2: 图形渲染
  - SDL2_image: 图像加载
  - SDL2_ttf: 字体渲染
  - pthread: 多线程TCP通信

- **界面功能 / UI Features:**
  - 显示4x8的物品数量表格
  - 显示13个角色及其关联物品
  - 玩家选择区域（高亮显示）
  - Go按钮（轮到玩家时启用）
  - Connect按钮（游戏开始前连接）

- **线程模型 / Threading Model:**
  - 主线程：SDL图形渲染和用户交互
  - TCP服务器线程：接收服务器消息
  - 使用互斥锁和同步标志进行线程通信

---

## 安装和运行 / Installation and Running

### 1. 安装依赖 / Install Dependencies

```bash
sudo apt update
sudo apt install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```

### 2. 编译 / Compilation

```bash
cd sh13_etu

# 编译客户端 / Compile client
gcc -o sh13 sh13.c -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread

# 编译服务器 / Compile server
gcc -o server server.c
```

### 3. 运行 / Running

**启动服务器 / Start Server:**
```bash
./server 12345
```

**启动客户端（4个玩家）/ Start Clients (4 players):**
```bash
# 玩家1 / Player 1
./sh13 127.0.0.1 12345 127.0.0.1 20001 Player1

# 玩家2 / Player 2
./sh13 127.0.0.1 12345 127.0.0.1 20002 Player2

# 玩家3 / Player 3
./sh13 127.0.0.1 12345 127.0.0.1 20003 Player3

# 玩家4 / Player 4
./sh13 127.0.0.1 12345 127.0.0.1 20004 Player4
```

**参数说明 / Parameters:**
- `参数1 / Arg 1`: 服务器IP地址 / Server IP address
- `参数2 / Arg 2`: 服务器端口 / Server port
- `参数3 / Arg 3`: 客户端IP地址（本机）/ Client IP address (localhost)
- `参数4 / Arg 4`: 客户端端口（每个客户端不同）/ Client port (unique per client)
- `参数5 / Arg 5`: 玩家名称 / Player name

---

## 项目文件结构 / Project File Structure

```
sh13_etu/
├── server.c              # 服务器源代码 / Server source code
├── sh13.c                # 客户端源代码 / Client source code
├── server                # 编译后的服务器 / Compiled server
├── sh13                  # 编译后的客户端 / Compiled client
├── SH13_0.png            # 角色卡图片 (13张: SH13_0.png to SH13_12.png)
├── SH13_1.png            # Character card images
├── ...                   # 
├── SH13_12.png           # 
├── SH13_pipe_120x120.png     # 烟斗图标 / Pipe icon
├── SH13_ampoule_120x120.png  # 灯泡图标 / Light bulb icon
├── SH13_poing_120x120.png    # 拳头图标 / Fist icon
├── SH13_couronne_120x120.png # 王冠图标 / Crown icon
├── SH13_carnet_120x120.png   # 笔记本图标 / Notebook icon
├── SH13_collier_120x120.png  # 项链图标 / Necklace icon
├── SH13_oeil_120x120.png     # 眼睛图标 / Eye icon
├── SH13_crane_120x120.png    # 骷髅图标 / Skull icon
├── SH13_grille.png       # 游戏界面背景 / Game UI background
├── gobutton.png          # Go按钮 / Go button
├── connectbutton.png     # 连接按钮 / Connect button
├── sans.ttf              # 字体文件 / Font file
├── readme.txt            # 原始说明 / Original readme
└── OSuser_Yulin.pdf      # 项目文档 / Project documentation
```

---

## 游戏界面 / Game Interface

### 界面布局 / UI Layout

```
┌─────────────────────────────────────────────┐
│ [Connect]  [Pipe][Bulb][Fist]...[物品行]   │
│                                             │
│ Player1    [  物品数量表格 4x8  ]          │
│ Player2    [  每个玩家对应一行  ]          │
│ Player3    [  每个物品对应一列  ]          │
│ Player4                                     │
│                                             │
│            ┌─────────────┐                  │
│            │  角色列表   │    [Go]         │
│            │  13个角色   │   按钮          │
│            │  及其物品   │                  │
│            └─────────────┘                  │
└─────────────────────────────────────────────┘
```

### 交互说明 / Interaction Guide

1. **点击Connect按钮**: 连接到服务器
2. **点击玩家名称**: 选择要询问的玩家
3. **点击物品图标**: 选择要询问的物品
4. **点击角色名称**: 选择要指认的嫌疑人
5. **点击Go按钮**: 确认行动（仅在轮到自己时可用）

---

## 开发信息 / Development Information

- **编程语言 / Language:** C
- **图形库 / Graphics:** SDL2, SDL2_image, SDL2_ttf
- **网络通信 / Network:** TCP/IP Socket
- **并发模型 / Concurrency:** POSIX Threads (pthread)
- **平台 / Platform:** Linux (WSL compatible)

---

## 注意事项 / Notes

1. 所有4个客户端必须连接后游戏才会开始
2. 每个客户端需要使用不同的端口号
3. 游戏需要图形界面支持（X11或WSL with X server）
4. 确保所有PNG和TTF资源文件在运行目录下

---

## 许可证 / License

本项目为教育项目。
This is an educational project.

---

## 贡献者 / Contributors

- Original implementation by Yulin (see OSuser_Yulin.pdf)
