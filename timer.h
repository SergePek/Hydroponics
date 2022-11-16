#pragma once
#include <Arduino.h>
// класс таймера на миллис
class Timer {
  public:
    // создать с указанием периода
    Timer (int period) {
      _period = period;
    }
    // возвращает true когда сработал период
    bool ready() {
      if (millis() - _tmr >= _period) {
        _tmr = millis();
        return true;
      }
      return false;
    }
  private:
    uint32_t _tmr;
    int _period;
};