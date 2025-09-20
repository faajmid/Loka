#ifndef LOKAMOTORS_H
#define LOKAMOTORS_H

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
  #define LOKA_PWM_BASE_FREQ 20000
  #define LOKA_PWM_RES_BITS  8
#endif

class LokaMotor {
public:
  LokaMotor(uint8_t IN1, uint8_t IN2);
  void Init();
  void Ctrl(int8_t percent);

private:
  uint8_t _IN1, _IN2;
};

#endif
