#ifndef GLOBAL_H
#define GLOBAL_H
#include <Arduino.h>

#define BOOT_BTN 1

#define LOGI(format, ...) Serial.printf("[INFO] "  "%s::%d" "  \"" format "\"  " "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define DEBOUNCE_DELAT 50 // 消抖延迟，单位毫秒
#define LONG_PRESS_DELAY 1000 // 长按延迟，单位毫秒

#endif
