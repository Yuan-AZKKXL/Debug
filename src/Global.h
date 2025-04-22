#ifndef GLOBAL_H
#define GLOBAL_H
#include <Arduino.h>

#define BOOT_BTN 1

#define LOGI(format, ...) Serial.printf("[INFO] " format "%s::%d \n", ##__VA_ARGS__, __FILE__, __LINE__)

const uint8_t debounceDelay = 50; // 消抖延迟，单位毫秒
const uint8_t longPressDelay = 1000; // 长按延迟，单位毫秒
#endif
