//單手鍵盤韌體
//AVR: Pro Micro 開發板（ATmega32U4）

#include <Arduino.h>
#include <RotaryEncoder.h>
#include <Keypad.h>
#include <HID-Project.h> 
#include <Bounce2.h>

//旋鈕腳位定義
#define DT_PIN 0
#define CLK_PIN 1
#define SW_PIN 15

//特殊按鍵 ID
#define FN_KEY_ID  40
#define CTRL_KEY_ID 46
#define LAYER_KEY_ID 47
#define HASHTAG_KEY_ID 9
#define AND_KEY_ID 10
#define STAR_KEY_ID 11
#define L_BRACKET_KEY_ID 2
#define PLUS_KEY_ID 13
#define CARET_KEY_ID 22
#define R_BRACKET_KEY_ID 3
#define SEMICOLON_KEY_ID 25
#define QUOTE_KEY_ID 26
#define COMMA_KEY_ID 27
#define PERIOD_KEY_ID 28
#define MOUSE_LEFT_KEY_ID 48
#define MOUSE_RIGHT_KEY_ID 49

//建立旋鈕物件
RotaryEncoder encoder(DT_PIN, CLK_PIN, RotaryEncoder::LatchMode::TWO03);
int newPos = 0;
//建立旋鈕按鍵防彈跳物件
Bounce middleButton = Bounce();

//鍵位表定義
//鍵位表定義
uint8_t keymaps[4][56] = {
// Layer 0：一般層（非 FN）
  { 0,                KEY_TILDE,        KEY_LEFT_BRACE,   KEY_RIGHT_BRACE,  KEY_MINUS,        KEY_EQUAL,        KEY_SEMICOLON,    KEY_BACKSLASH,
    KEY_ESC,          KEY_6,            KEY_7,            KEY_8,            KEY_9,            KEY_0,            KEY_COMMA,        KEY_PERIOD,
    KEY_CAPS_LOCK,    KEY_1,            KEY_2,            KEY_3,            KEY_4,            KEY_5,            KEY_SLASH,        KEY_QUOTE,
    KEY_TAB,          KEY_Y,            KEY_Q,            KEY_W,            KEY_E,            KEY_R,            KEY_T,            KEY_BACKSPACE,
    KEY_LEFT_SHIFT,   KEY_H,            KEY_A,            KEY_S,            0x07,             KEY_F,            KEY_G,            KEY_ENTER,
    0,                KEY_Z,            KEY_X,            KEY_C,            KEY_V,            KEY_LEFT_ALT,     KEY_LEFT_CTRL,    1,
    KEY_LEFT,         KEY_RIGHT,        KEY_SPACE,        0,                0,                0,                0,                0
  },                  
  
// Layer 1：一般層（FN）
  { 0,                0,                0,                0,                0,                0,                0,                0,
    KEY_LEFT_WINDOWS, KEY_F7,           KEY_F8,           KEY_F9,           KEY_F10,          KEY_F11,          KEY_F12,          0,
    KEY_CAPS_LOCK,    KEY_F1,           KEY_F2,           KEY_F3,           KEY_F4,           KEY_F5,           KEY_F6,           0,
    KEY_TAB,          0,                KEY_UP,           KEY_U,            KEY_I,            KEY_O,            KEY_P,            KEY_DELETE,
    KEY_LEFT_SHIFT,   KEY_LEFT,         KEY_DOWN,         KEY_RIGHT,        KEY_J,            KEY_K,            KEY_L,            KEY_ENTER,
    0,                0,                KEY_B,            KEY_N,            KEY_M,            0,                1,                1,
    0,                0,                0,                0,                0,                0,                0,                0
  },

// Layer 2：注音層（非 FN）
  { 0,                KEY_1,            KEY_S,            KEY_F,            KEY_Y,            KEY_8,            KEY_L,            KEY_SLASH,
    KEY_ESC,          KEY_Q,            KEY_X,            KEY_V,            KEY_H,            KEY_I,            KEY_PERIOD,       KEY_MINUS,
    KEY_CAPS_LOCK,    KEY_A,            KEY_E,            KEY_5,            KEY_N,            KEY_K,            KEY_0,            KEY_TILDE,
    KEY_TAB,          KEY_Z,            KEY_D,            KEY_T,            KEY_U,            KEY_COMMA,        KEY_P,            KEY_BACKSPACE,
    KEY_LEFT_SHIFT,   KEY_2,            KEY_C,            KEY_G,            KEY_J,            KEY_9,            KEY_SEMICOLON,    KEY_ENTER,
    0,                KEY_W,            KEY_R,            KEY_B,            KEY_M,            KEY_O,            KEY_LEFT_CTRL,    1,
    KEY_LEFT,         KEY_RIGHT,        KEY_SPACE,        0,                0,                0,                0,                0
  },

// Layer 3：注音層（FN）
  { 0,                0,                1,                1,                0,                0,                0,                0,
    KEY_LEFT_WINDOWS, 1,                1,                1,                KEY_EQUAL,        1,                0,                0,
    KEY_CAPS_LOCK,    KEY_SPACE,        KEY_6,            KEY_3,            KEY_4,            KEY_7,            1,                0,
    KEY_TAB,          1,                1,                1,                1,                0,                0,                KEY_DELETE,
    KEY_LEFT_SHIFT,   0,                0,                0,                0,                0,                0,                0,
    0,                0,                0,                0,                0,                0,                1,                1,
    KEY_LEFT,         KEY_RIGHT,        KEY_SPACE,        0,                0,                0,                0,                0
  },
};

//矩陣索引定義
const byte ROWS = 7;
const byte COLS = 8;
byte indices[ROWS][COLS] = {
  {0, 1, 2, 3, 4, 5, 6, 7},
  {8, 9, 10, 11, 12, 13, 14, 15},
  {16, 17, 18, 19, 20, 21, 22, 23},
  {24, 25, 26, 27, 28, 29, 30, 31},
  {32, 33, 34, 35, 36, 37, 38, 39},
  {40, 41, 42, 43, 44, 45, 46, 47},
  {48, 49, 50, 51, 52, 53, 54, 55}
};

//矩陣行列腳位定義
byte rowPins[ROWS] = { 9, 8, 7, 6, 14, 16, 10 }; 
byte colPins[COLS] = { A0, A1, A2, A3, 2, 3, 4, 5 }; 

Keypad kpd = Keypad(makeKeymap(indices), rowPins, colPins, ROWS, COLS);

//目前層級
byte currentLayer = 0;

//HID 監控用統計資料
uint32_t keyPressCount = 0;
uint32_t fnPressCount = 0;
uint32_t encoderTurnCount = 0;
uint32_t mouseClickCount = 0;
uint8_t lastKeyId = 0;
uint8_t lastKeyLayer = 0;
bool telemetryDirty = true;
unsigned long lastTelemetryMs = 0;

static int16_t clamp16(uint32_t v) {
  return (v > 32767U) ? 32767 : (int16_t)v;
}

static void sendTelemetryIfNeeded() {
  const unsigned long now = millis();
  if (!telemetryDirty && (now - lastTelemetryMs) < 100) {
    return;
  }

  Gamepad.xAxis(clamp16(keyPressCount));
  Gamepad.yAxis(clamp16(fnPressCount));
  Gamepad.rxAxis(clamp16(encoderTurnCount));
  Gamepad.ryAxis(clamp16(mouseClickCount));
  Gamepad.zAxis((int8_t)currentLayer);
  Gamepad.rzAxis((int8_t)lastKeyId);
  Gamepad.dPad1((int8_t)(lastKeyLayer + 1));
  Gamepad.dPad2((int8_t)(currentLayer + 1));
  Gamepad.write();

  telemetryDirty = false;
  lastTelemetryMs = now;
}

void setup() {  
  Keyboard.begin();
  Mouse.begin();
  Gamepad.begin();
  pinMode(SW_PIN, INPUT_PULLUP);  
  middleButton.attach(SW_PIN); //將 SW_PIN 綁定到 Bounce 物件
  middleButton.interval(10); //防彈跳 10ms
}

void loop() {  
  //掃描矩陣
  if (kpd.getKeys()) {
    for (int i = 0; i < LIST_MAX; i++) {
      if (kpd.key[i].stateChanged) {
        int keyID = kpd.key[i].kchar;
        //防止異常的 keyID
        if (keyID < 0 || keyID >= 56) {
          continue;
        }
        if (kpd.key[i].kstate == PRESSED) {
          if (keyID != MOUSE_LEFT_KEY_ID && keyID != MOUSE_RIGHT_KEY_ID) {
            keyPressCount++;
          }
          lastKeyId = (uint8_t)keyID;
          lastKeyLayer = currentLayer;
          telemetryDirty = true;
          if (keyID == FN_KEY_ID) {
            fnPressCount++;
          }
        }
        // Layer 0：一般層（非 FN）
        if (keyID == LAYER_KEY_ID) {
          switch (kpd.key[i].kstate) {
            case PRESSED:
               if (currentLayer == 0) currentLayer = 2;
               else if (currentLayer == 2) currentLayer = 0;
               Serial.println("Layer:"+String(currentLayer));
               Keyboard.releaseAll(); 
               break;               
             case RELEASED:
               Keyboard.releaseAll();
               break;
            case IDLE:
              break;
            case HOLD:
              break;
          }          
        }
        //非 LAYER_KEY
        else 
          {
                //滑鼠左右鍵：任何層都可用，不送出鍵盤碼
                if ((keyID == MOUSE_LEFT_KEY_ID || keyID == MOUSE_RIGHT_KEY_ID))
                {
                  uint8_t mouseButton = (keyID == MOUSE_LEFT_KEY_ID) ? MOUSE_LEFT : MOUSE_RIGHT;
                  switch (kpd.key[i].kstate) {
                    case PRESSED:
                      Mouse.press(mouseButton);
                      mouseClickCount++;
                      telemetryDirty = true;
                      break;
                      
                    case RELEASED:
                      Mouse.release(mouseButton);
                      break;
                    case IDLE:
                      break;
                    case HOLD:
                      break;
                  }
                //避免同時被當作鍵盤鍵處理
                continue;
                }

// 一般層-----------------------------------------------------------------------
          if (currentLayer == 0 || currentLayer == 1) 
          {
            //FN 鍵
            if (keyID == FN_KEY_ID) {
              switch (kpd.key[i].kstate) {
                case PRESSED:
                  currentLayer = 1;
                  Serial.println("Layer:"+String(currentLayer));
                  Keyboard.releaseAll(); 
                  break;
                  
                case RELEASED:
                  currentLayer = 0;
                  Serial.println("Layer:"+String(currentLayer));
                  Keyboard.releaseAll();
                  break;
                case IDLE:
                  break;
                case HOLD:
                  break;
              }
              
            }
            //其他按鍵
            else {
              //FN+Ctrl（送出 WIN+SPACE）
              if (currentLayer == 1 && keyID == CTRL_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_WINDOWS);
                    Keyboard.press((KeyboardKeycode)KEY_SPACE);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_WINDOWS);
                    Keyboard.release((KeyboardKeycode)KEY_SPACE);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              else
              {
                //一般按鍵：從鍵位表取得鍵碼
                uint8_t rawCode1 = keymaps[currentLayer][keyID];
                Serial.print(currentLayer);
                Serial.print("\t");
                Serial.print(keyID);
                Serial.print("\t");
                Serial.println(rawCode1,HEX);

                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)rawCode1);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)rawCode1);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
            }
          }

// 注音層-------------------------------------------------------------------------
          else if (currentLayer == 2 || currentLayer == 3)
          {
            //FN 鍵
            if (keyID == FN_KEY_ID) {
              switch (kpd.key[i].kstate) {
                case PRESSED:
                  currentLayer = 3;
                  Serial.println("Layer:"+String(currentLayer));
                  Keyboard.releaseAll(); 
                  break;
                  
                case RELEASED:
                  currentLayer = 2;
                  Serial.println("Layer:"+String(currentLayer));
                  Keyboard.releaseAll();
                  break;
                case IDLE:
                  break;
                case HOLD:
                  break;
              }              
            }
            //其他按鍵
            else {
              //FN+Ctrl（送出 WIN+SPACE）
              if (currentLayer == 3 && keyID == CTRL_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_WINDOWS);
                    Keyboard.press((KeyboardKeycode)KEY_SPACE);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_WINDOWS);
                    Keyboard.release((KeyboardKeycode)KEY_SPACE);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // [ 鍵（送出 CTRL+[）
              else if (currentLayer == 3 && keyID == L_BRACKET_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_BRACE);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_BRACE);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // ] 鍵（送出 CTRL+]）
              else if (currentLayer == 3 && keyID == R_BRACKET_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.press((KeyboardKeycode)KEY_RIGHT_BRACE);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.release((KeyboardKeycode)KEY_RIGHT_BRACE);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // # 鍵（送出 SHIFT+3）
              else if (currentLayer == 3 && keyID == HASHTAG_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.press((KeyboardKeycode)KEY_3);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.release((KeyboardKeycode)KEY_3);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // & 鍵（送出 SHIFT+7）
              else if (currentLayer == 3 && keyID == AND_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.press((KeyboardKeycode)KEY_7);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.release((KeyboardKeycode)KEY_7);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // * 鍵（送出 SHIFT+8）
              else if (currentLayer == 3 && keyID == STAR_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.press((KeyboardKeycode)KEY_8);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.release((KeyboardKeycode)KEY_8);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // + 鍵（送出 SHIFT+=）
              else if (currentLayer == 3 && keyID == PLUS_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.press((KeyboardKeycode)KEY_EQUAL);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.release((KeyboardKeycode)KEY_EQUAL);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // ^ 鍵（送出 SHIFT+6）
              else if (currentLayer == 3 && keyID == CARET_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.press((KeyboardKeycode)KEY_6);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_SHIFT);
                    Keyboard.release((KeyboardKeycode)KEY_6);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // ; 鍵（送出 CTRL+;）
              else if (currentLayer == 3 && keyID == SEMICOLON_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.press((KeyboardKeycode)KEY_SEMICOLON);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.release((KeyboardKeycode)KEY_SEMICOLON);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // ' 鍵（送出 CTRL+'）
              else if (currentLayer == 3 && keyID == QUOTE_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.press((KeyboardKeycode)KEY_QUOTE);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.release((KeyboardKeycode)KEY_QUOTE);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // , 鍵（送出 CTRL+,）
              else if (currentLayer == 3 && keyID == COMMA_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.press((KeyboardKeycode)KEY_COMMA);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.release((KeyboardKeycode)KEY_COMMA);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              // . 鍵（送出 CTRL+.）
              else if (currentLayer == 3 && keyID == PERIOD_KEY_ID)  
              {
                switch (kpd.key[i].kstate) {
                  case PRESSED:                  
                    Keyboard.press((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.press((KeyboardKeycode)KEY_PERIOD);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)KEY_LEFT_CTRL);
                    Keyboard.release((KeyboardKeycode)KEY_PERIOD);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
              else 
              {
                //一般按鍵：從鍵位表取得鍵碼
                uint8_t rawCode2 = keymaps[currentLayer][keyID];

                switch (kpd.key[i].kstate) {
                  case PRESSED:
                    Keyboard.press((KeyboardKeycode)rawCode2);
                    break;
                    
                  case RELEASED:
                    Keyboard.release((KeyboardKeycode)rawCode2);
                    break;
                  case IDLE:
                    break;
                  case HOLD:
                    break;
                }
              }
            }
          }
        }
      }
    }
  }

  //旋鈕滾動（上/下）
  static int pos = 0;
  encoder.tick();
  int newPos = encoder.getPosition();
  if (pos != newPos) {
    int dir = (int)(encoder.getDirection());
    int delta = newPos - pos;
    pos = newPos;
    Mouse.move(0, 0, -dir);
    encoderTurnCount += (uint32_t)abs(delta);
    telemetryDirty = true;
  }

  //旋鈕按鍵防彈跳
  middleButton.update();
  //偵測到落沿時送出滑鼠中鍵點擊
  if (middleButton.fell()) {
    Mouse.click(MOUSE_MIDDLE);
    mouseClickCount++;
    telemetryDirty = true;
  }

  sendTelemetryIfNeeded();
}
