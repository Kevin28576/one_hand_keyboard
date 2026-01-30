# 單手鍵盤韌體（Pro Micro ATmega32U4）

本專案為國際科展展示用之單手鍵盤韌體，採用 PlatformIO + Arduino 架構，
以 Pro Micro（ATmega32U4）為主控。韌體包含 7x8 鍵盤矩陣掃描、四層鍵位
（兩組基底層 + 各自 FN 層）、旋鈕滾輪與中鍵點擊功能，並透過 USB HID
輸出鍵盤與滑鼠事件，同時回報 HID 狀態資料供電腦端 App 顯示。

## 專案特色

- 7x8 矩陣鍵盤掃描（56 鍵位）
- 4 層鍵位設計（Layer 0/1/2/3）
- 旋鈕滾輪 + 中鍵點擊
- HID 狀態回報（Layer/統計數據）

## 硬體與接線摘要

- 開發板：Pro Micro Dev Board（ATmega32U4）
- 鍵盤矩陣：7 行 x 8 列
- 旋鈕：DT/CLK = 0/1、按鍵 SW = 15
- 行列接腳（程式中定義）
  - 行（ROWS）：9, 8, 7, 6, 14, 16, 10
  - 列（COLS）：A0, A1, A2, A3, 2, 3, 4, 5

## 層級（Layer）設計與切換規則

- Layer 0：英文層
- Layer 1：英文層 FN（按住 FN 暫時進入）
- Layer 2：注音層
- Layer 3：注音層 FN（按住 FN 暫時進入）

切換規則：

- `LAYER_KEY`：在 Layer 0 與 Layer 2 間切換
- `FN_KEY`：在 0<->1 或 2<->3 暫時切換

## HID 狀態監控 App

電腦端 App 會顯示目前層級與使用統計，並提供熱度圖、匯出功能等。
詳見 [docs/APP.md](/docs/APP.md)。

## 編譯與上傳

- 編譯：`pio run -e sparkfun_promicro16`
- 上傳：`pio run -e sparkfun_promicro16 -t upload`

## 專案結構

- `src/main.cpp`：主要韌體
- `platformio.ini`：平台設定與函式庫依賴
- `tools/`：監控 App 與工具
- `docs/APP.md`：App 使用說明
