# HID 狀態監控 App（Windows / Python）

本 App 透過 HID 讀取鍵盤韌體回報的狀態資料，提供即時監控與匯出功能，
適用於展示與使用者體驗分析。

## 功能總覽
- 目前 Layer 與統計數據顯示
- 最近 10 次按鍵紀錄
- 平均按鍵速度（次/秒）
- 記住上次裝置與自動連線
- 視窗置頂、最小化到托盤
- 記錄輸出（CSV / JSON）
- Excel 匯出（含鍵位對照表與每層熱度統計）
- 按鍵熱度圖（每層可切換）
- 熱度圖輸出 PNG（每層一張）

## 安裝與執行
1) 安裝依賴：
   - `pip install -r tools/requirements.txt`
2) 執行：
   - `python tools/hid_layer_monitor.py`

## 介面說明
- 裝置：選擇 HID 裝置並連線
- 視窗置頂：固定在最上層
- 自動連線：記住上次裝置並自動連線
- 鍵位表：顯示 4 層鍵位對照
- 熱度圖：顯示矩陣熱區（可切換 Layer）
- 開始記錄：輸出 CSV/JSON 到 `tools/logs/`
- 匯出 Excel：輸出即時資料 + 統計 + 鍵位表
- 匯出熱度PNG：每層輸出 `heatmap_layer_0~3.png`

## HID 回報欄位對應
回報格式使用 HID Gamepad（`HID-Project`），對應如下：
- X 軸（int16）：按鍵次數（最多 32767）
- Y 軸（int16）：FN 使用次數（最多 32767）
- Rx 軸（int16）：旋鈕轉動次數（最多 32767）
- Ry 軸（int16）：滑鼠點擊次數（最多 32767）
- Z 軸（int8）：目前 Layer（0~3）
- Rz 軸（int8）：最近按鍵 keyID（0~55）
- DPad1（4-bit）：最近按鍵所在 Layer（1~4）
- DPad2（4-bit）：目前 Layer（1~4）

## 檔案輸出
- CSV/JSON：`tools/logs/`
- Excel：使用者指定路徑（含 3~4 個工作表）
- PNG：使用者指定資料夾

## 常見問題
- Python 3.13：會自動使用 `pywinusb` 後端
- 若看不到裝置：請確認韌體已燒錄且裝置已連線
