#include "LokaMotors.h"

LokaMotor::LokaMotor(uint8_t IN1, uint8_t IN2) : _IN1(IN1), _IN2(IN2) {}

void LokaMotor::Init() {
#if defined(ARDUINO_ARCH_ESP32)
  pinMode(_IN1, OUTPUT);
  pinMode(_IN2, OUTPUT);
  ledcAttach(_IN1, LOKA_PWM_BASE_FREQ, LOKA_PWM_RES_BITS);
  ledcAttach(_IN2, LOKA_PWM_BASE_FREQ, LOKA_PWM_RES_BITS);
#else
  pinMode(_IN1, OUTPUT);
  pinMode(_IN2, OUTPUT);
#endif
}

void LokaMotor::Ctrl(int8_t percent) {
  if (percent > 100) percent = 100;
  if (percent < -100) percent = -100;

#if defined(ARDUINO_ARCH_ESP32)
  uint8_t duty = (uint16_t)abs(percent) * 255 / 100;
  if (percent > 0) {
    ledcWrite(_IN1, 0);
    ledcWrite(_IN2, duty);
  } else if (percent < 0) {
    ledcWrite(_IN2, 0);
    ledcWrite(_IN1, duty);
  } else {
    ledcWrite(_IN1, 0);
    ledcWrite(_IN2, 0);
  }
#else
  uint8_t duty = (uint16_t)abs(percent) * 255 / 100;
  if (percent > 0) {
    digitalWrite(_IN1, LOW);
    #if defined(analogWrite)
      analogWrite(_IN2, duty);
    #else
      digitalWrite(_IN2, duty > 127 ? HIGH : LOW);
    #endif
  } else if (percent < 0) {
    digitalWrite(_IN2, LOW);
    #if defined(analogWrite)
      analogWrite(_IN1, duty);
    #else
      digitalWrite(_IN1, duty > 127 ? HIGH : LOW);
    #endif
  } else {
    #if defined(analogWrite)
      analogWrite(_IN1, 0);
      analogWrite(_IN2, 0);
    #else
      digitalWrite(_IN1, LOW);
      digitalWrite(_IN2, LOW);
    #endif
  }
#endif
}
