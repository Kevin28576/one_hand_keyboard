
這個目錄用來放專案私有（本地）函式庫。PlatformIO 會把這些函式庫編譯成
靜態庫並連結到韌體中。

此專案已在 `platformio.ini` 透過 Library Manager 引入
RotaryEncoder、HID-Project、Keypad、Bounce2；若需要自訂或修改的函式庫，
才放在 `lib/` 內。

每個函式庫請放在獨立資料夾中，例如：
```
lib/
  MyLib/
    src/
      MyLib.cpp
      MyLib.h
    library.json   (選用，可設定編譯選項)
```

在程式中使用：
```cpp
#include <MyLib.h>
```

PlatformIO 的 Library Dependency Finder 會自動掃描 `src` 內的 include
並解析相依關係。

文件：
https://docs.platformio.org/page/librarymanager/ldf.html
