#include "Global.h"
#include "InputTask.h"
#include "Event.h"

#include <Arduino.h>

// #define INPUT_DEBUG

InputTask::InputTask() 
    : m_wheelTask(0), 
      m_buttonTask(0), 
      m_touchTask(0),
      m_stateMachine(0)
{
}

InputTask::~InputTask() {
    stop();
}

void InputTask::setStateMachine(StateMachine* machine) {
    m_stateMachine = machine;
}

bool InputTask::start(UBaseType_t priority) {
    if (!m_stateMachine) {
        return false;
    }
    
    // 创建滚轮任务
    BaseType_t result = xTaskCreate(
        wheelTaskFunc,
        "WheelTask",
        4096,
        this,
        priority,
        &m_wheelTask
    );
    
    if (result != pdPASS) {
        return false;
    }
    
    // 创建按钮任务
    result = xTaskCreate(
        buttonTaskFunc,
        "ButtonTask",
        4096,
        this,
        priority,
        &m_buttonTask
    );
    
    if (result != pdPASS) {
        vTaskDelete(m_wheelTask);
        m_wheelTask = 0;
        return false;
    }
    
    // 创建触摸屏任务
    result = xTaskCreate(
        touchTaskFunc,
        "TouchTask",
        4096,
        this,
        priority,
        &m_touchTask
    );
    
    if (result != pdPASS) {
        vTaskDelete(m_wheelTask);
        vTaskDelete(m_buttonTask);
        m_wheelTask = 0;
        m_buttonTask = 0;
        return false;
    }
    
    return true;
}

void InputTask::stop() {
    if (m_wheelTask) {
        vTaskDelete(m_wheelTask);
        m_wheelTask = 0;
    }
    
    if (m_buttonTask) {
        vTaskDelete(m_buttonTask);
        m_buttonTask = 0;
    }
    
    if (m_touchTask) {
        vTaskDelete(m_touchTask);
        m_touchTask = 0;
    }
}

void InputTask::wheelTaskFunc(void* params) {
    InputTask* inputTask = static_cast<InputTask*>(params);
    StateMachine* stateMachine = inputTask->m_stateMachine;
    
    for (;;) {
        // 轮询滚轮
        // TODO: 实现滚轮状态检测

#ifdef INPUT_DEBUG
        LOGI();
#endif
        bool wheelMoved = false;
        bool isClockwise = true;
        
        if (wheelMoved) {
            // 创建滚轮事件
            WheelEvent event(isClockwise);
            
            // 发送事件到状态机
            stateMachine->postEvent(&event);
        }
        
        // 滚轮检测延迟
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void InputTask::buttonTaskFunc(void* params)
{
    InputTask* inputTask = static_cast<InputTask*>(params);
    StateMachine* stateMachine = inputTask->m_stateMachine;
    // 初始化按键状态
    BtnState buttonState = BtnRelease;
    unsigned long pressStartTime = 0;
    // 轮询按钮
    for (;;){
        // 读取按钮状态
        bool currentButtonState = digitalRead(BOOT_BTN) == LOW;
        switch (buttonState){
            case BtnRelease:{
                if (currentButtonState){
                    // 按键按下，开始消抖
                    pressStartTime = millis();
                    buttonState = BtnPress;
                }
            }break;
            case BtnPress:{
                if (!currentButtonState){
                    // 按键松开，判断是否为短按
                    if (millis() - pressStartTime < debounceDelay){
                        // 消抖时间内松开，忽略
                        buttonState = BtnRelease;
                    }
                    else{
                        // 短按事件
                        LOGI("press ");
                        ButtonEvent event(EVENT_BUTTON_PRESS, BOOT_BTN);
                        stateMachine->postEvent(&event);
                        buttonState = BtnRelease;
                    }
                }
                else if (millis() - pressStartTime >= longPressDelay){
                    LOGI("long press");
                    ButtonEvent event(EVENT_BUTTON_LONGPRESS, BOOT_BTN);
                    stateMachine->postEvent(&event);
                    buttonState = BtnLongPress;
                }
            }break;
            case BtnLongPress:{
                if (!currentButtonState){
                    // 长按后松开
                    buttonState = BtnRelease;
                }
            }break;
        }
        // 按钮检测延迟
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void InputTask::touchTaskFunc(void* params) {
    InputTask* inputTask = static_cast<InputTask*>(params);
    StateMachine* stateMachine = inputTask->m_stateMachine;

    for (;;) {
        // 轮询触摸屏
        // TODO: 实现触摸屏状态检测

#ifdef INPUT_DEBUG
        LOGI();
#endif
        bool touchActive = false;
        int touchX = 0;
        int touchY = 0;

        if (touchActive) {
            // 创建触摸事件
            TouchEvent event(EVENT_TOUCH_PRESS, touchX, touchY);
            
            // 发送事件到状态机
            stateMachine->postEvent(&event);
        }

        // 触摸屏检测延迟
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
