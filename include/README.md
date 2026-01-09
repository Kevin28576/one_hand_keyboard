
這個目錄用來放專案的標頭檔（header files）。

此專案是單手鍵盤韌體（Pro Micro / ATmega32U4），建議把共用的宣告與常數
（例如鍵位定義、層切換規則、硬體腳位）整理成 `.h` 檔放在此處，再由 `src`
內的程式透過 `#include` 引用。

範例：
```cpp
#include "keymap.h"

void setup() {
  // ...
}
```

使用標頭檔的好處是把共用宣告集中在同一處，修改時只需改一個檔案即可。

命名慣例：C/C++ 標頭檔通常以 `.h` 結尾。

參考文件（GCC Header Files）：
https://gcc.gnu.org/onlinedocs/cpp/Header-Files.html
