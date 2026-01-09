# 單手鍵盤韌體（Pro Micro ATmega32U4）

本專案為國際科展展示用之單手鍵盤韌體，採用 PlatformIO + Arduino 架構，
以 Pro Micro（ATmega32U4）為主控。韌體包含 7x8 鍵盤矩陣掃描、四層鍵位
（兩組基底層 + 各自 FN 層）、旋鈕滾輪與中鍵點擊功能，並透過 USB HID
輸出鍵盤與滑鼠事件。

以下內容完全依照目前專案程式（`src/main.cpp`）整理，包含矩陣索引、
功能鍵位置與每一層的鍵位表，便於展示與檢查。

## 專案目的

- 提供單手操作的完整輸入方案，整合鍵盤與滑鼠功能。
- 以多層鍵位設計降低單手操作負擔。
- 展示嵌入式韌體與人機互動整合的應用。

## 硬體與接線摘要

- 開發板：Pro Micro Dev Board（ATmega32U4）
- 鍵盤矩陣：7 行 x 8 列（共 56 鍵位索引）
- 旋鈕：DT/CLK = 0/1、按鍵 SW = 15
- 行列接腳（程式中定義）
  - 行（ROWS）：9, 8, 7, 6, 14, 16, 10
  - 列（COLS）：A0, A1, A2, A3, 2, 3, 4, 5

## 軟體架構與模組

- `src/main.cpp`：主程式（矩陣掃描、層切換、HID 輸出、旋鈕處理）
- `platformio.ini`：平台與依賴庫設定

使用的第三方函式庫（由 PlatformIO 管理）：

- RotaryEncoder：旋鈕方向/位置偵測
- HID-Project：USB 鍵盤/滑鼠 HID 功能
- Keypad：矩陣鍵盤掃描
- Bounce2：旋鈕按鍵防彈跳

## 層級（Layer）設計與切換規則

系統共 4 層：

- Layer 0：一般層
- Layer 1：一般層的 FN 層（按住 FN 暫時進入）
- Layer 2：注音層
- Layer 3：注音層的 FN 層（按住 FN 暫時進入）

切換規則（依程式現況）：

- `LAYER_KEY`：在 Layer 0 與 Layer 2 之間切換（固定切換）
- `FN_KEY`：在 0<->1 或 2<->3 之間暫時切換（按下進入、放開返回）

## 矩陣索引說明

鍵盤矩陣以 7 行 x 8 列掃描，索引範圍 0~55。索引由左到右、由上到下遞增：


| Row/Col | C0 | C1 | C2 | C3 | C4 | C5 | C6 | C7 |
| ------- | -- | -- | -- | -- | -- | -- | -- | -- |
| R0      | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  |
| R1      | 8  | 9  | 10 | 11 | 12 | 13 | 14 | 15 |
| R2      | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
| R3      | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
| R4      | 32 | 33 | 34 | 35 | 36 | 37 | 38 | 39 |
| R5      | 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 |
| R6      | 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 |

## 功能鍵位置（依 keyID 常數）

以下為程式中特殊功能鍵對應的索引位置：


| 功能              | keyID | 位置 |
| ----------------- | ----: | ---- |
| FN_KEY            |    40 | R5C0 |
| CTRL_KEY          |    46 | R5C6 |
| LAYER_KEY         |    47 | R5C7 |
| MOUSE_LEFT        |    48 | R6C0 |
| MOUSE_RIGHT       |    49 | R6C1 |
| HASHTAG_KEY (#)   |     9 | R1C1 |
| AND_KEY (&)       |    10 | R1C2 |
| STAR_KEY (*)      |    11 | R1C3 |
| PLUS_KEY (+)      |    13 | R1C5 |
| L_BRACKET_KEY ([) |     2 | R0C2 |
| R_BRACKET_KEY (]) |     3 | R0C3 |
| CARET_KEY (^)     |    22 | R2C6 |
| SEMICOLON_KEY (;) |    25 | R3C1 |
| QUOTE_KEY (')     |    26 | R3C2 |
| COMMA_KEY (,)     |    27 | R3C3 |
| PERIOD_KEY (.)    |    28 | R3C4 |

## Layer 0（一般層）鍵位表

`0` 代表空鍵位，`0x07` 為 HID 原始鍵碼（保持原程式設定）。


| Row/Col | C0       | C1      | C2      | C3  | C4     | C5     | C6      | C7          |
| ------- | -------- | ------- | ------- | --- | ------ | ------ | ------- | ----------- |
| R0      | 0        | `~`     | `[`     | `]` | `-`    | `=`    | `;`     | `\`         |
| R1      | `ESC`    | `6`     | `7`     | `8` | `9`    | `0`    | `,`     | `.`         |
| R2      | `CAPS`   | `1`     | `2`     | `3` | `4`    | `5`    | `/`     | `'`         |
| R3      | `TAB`    | `Y`     | `Q`     | `W` | `E`    | `R`    | `T`     | `BACKSPACE` |
| R4      | `LSHIFT` | `H`     | `A`     | `S` | `0x07` | `F`    | `G`     | `ENTER`     |
| R5      | 0        | `Z`     | `X`     | `C` | `V`    | `LALT` | `LCTRL` | 1           |
| R6      | `LEFT`   | `RIGHT` | `SPACE` | 0   | 0      | 0      | 0       | 0           |

## Layer 1（一般層 FN）鍵位表


| Row/Col | C0       | C1     | C2     | C3      | C4    | C5    | C6    | C7       |
| ------- | -------- | ------ | ------ | ------- | ----- | ----- | ----- | -------- |
| R0      | 0        | 0      | 0      | 0       | 0     | 0     | 0     | 0        |
| R1      | `LWIN`   | `F7`   | `F8`   | `F9`    | `F10` | `F11` | `F12` | 0        |
| R2      | `CAPS`   | `F1`   | `F2`   | `F3`    | `F4`  | `F5`  | `F6`  | 0        |
| R3      | `TAB`    | 0      | `UP`   | `U`     | `I`   | `O`   | `P`   | `DELETE` |
| R4      | `LSHIFT` | `LEFT` | `DOWN` | `RIGHT` | `J`   | `K`   | `L`   | `ENTER`  |
| R5      | 0        | 0      | `B`    | `N`     | `M`   | 0     | 1     | 1        |
| R6      | 0        | 0      | 0      | 0       | 0     | 0     | 0     | 0        |

## Layer 2（注音層）鍵位表


| Row/Col | C0       | C1      | C2      | C3  | C4  | C5  | C6      | C7          |
| ------- | -------- | ------- | ------- | --- | --- | --- | ------- | ----------- |
| R0      | 0        | `1`     | `S`     | `F` | `Y` | `8` | `L`     | `/`         |
| R1      | `ESC`    | `Q`     | `X`     | `V` | `H` | `I` | `.`     | `-`         |
| R2      | `CAPS`   | `A`     | `E`     | `5` | `N` | `K` | `0`     | `~`         |
| R3      | `TAB`    | `Z`     | `D`     | `T` | `U` | `,` | `P`     | `BACKSPACE` |
| R4      | `LSHIFT` | `2`     | `C`     | `G` | `J` | `9` | `;`     | `ENTER`     |
| R5      | 0        | `W`     | `R`     | `B` | `M` | `O` | `LCTRL` | 1           |
| R6      | `LEFT`   | `RIGHT` | `SPACE` | 0   | 0   | 0   | 0       | 0           |

## Layer 3（注音的 FN）鍵位表

此層多為功能鍵，搭配程式內的「特殊按鍵輸出」邏輯使用。


| Row/Col | C0       | C1      | C2      | C3  | C4  | C5  | C6 | C7       |
| ------- | -------- | ------- | ------- | --- | --- | --- | -- | -------- |
| R0      | 0        | 0       | 1       | 1   | 0   | 0   | 0  | 0        |
| R1      | `LWIN`   | 1       | 1       | 1   | `=` | 1   | 0  | 0        |
| R2      | `CAPS`   | `SPACE` | `6`     | `3` | `4` | `7` | 1  | 0        |
| R3      | `TAB`    | 1       | 1       | 1   | 1   | 0   | 0  | `DELETE` |
| R4      | `LSHIFT` | 0       | 0       | 0   | 0   | 0   | 0  | 0        |
| R5      | 0        | 0       | 0       | 0   | 0   | 0   | 1  | 1        |
| R6      | `LEFT`   | `RIGHT` | `SPACE` | 0   | 0   | 0   | 0  | 0        |

## 特殊按鍵輸出行為（Layer 3）

當 `currentLayer == 3` 時，以下 keyID 會輸出組合鍵：

- `CTRL_KEY_ID`：送出 `WIN + SPACE`
- `L_BRACKET_KEY_ID`：送出 `CTRL + [`
- `R_BRACKET_KEY_ID`：送出 `CTRL + ]`
- `HASHTAG_KEY_ID`：送出 `SHIFT + 3`
- `AND_KEY_ID`：送出 `SHIFT + 7`
- `STAR_KEY_ID`：送出 `SHIFT + 8`
- `PLUS_KEY_ID`：送出 `SHIFT + =`
- `CARET_KEY_ID`：送出 `SHIFT + 6`
- `SEMICOLON_KEY_ID`：送出 `CTRL + ;`
- `QUOTE_KEY_ID`：送出 `CTRL + '`
- `COMMA_KEY_ID`：送出 `CTRL + ,`
- `PERIOD_KEY_ID`：送出 `CTRL + .`

## 滑鼠功能

- 左鍵/右鍵：`MOUSE_LEFT_KEY_ID` 與 `MOUSE_RIGHT_KEY_ID`，任何層都可使用。
- 旋鈕滾動：偵測旋鈕方向後，送出滑鼠滾輪事件。
- 旋鈕中鍵：按下旋鈕開關觸發中鍵點擊。

## 編譯與上傳

使用 PlatformIO：

- 編譯：`pio run -e sparkfun_promicro16`
- 上傳：`pio run -e sparkfun_promicro16 -t upload`

## 修改與擴充指引

- 若變更矩陣尺寸或接腳，請同步更新 `ROWS/COLS` 與 `rowPins/colPins`。
- 鍵位配置由 `keymaps[4][56]` 定義，可依層級調整。
- 若新增功能鍵，請更新 keyID 常數並在程式中加入對應邏輯。

## 專案檔案結構

- `src/main.cpp`：主要韌體邏輯
- `platformio.ini`：平台設定與函式庫依賴
- `include/`、`lib/`、`test/`：保留給自訂程式庫與測試
