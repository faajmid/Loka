#include "LokaMCU.h"
#include <math.h>
#include "mcu/sh2.h"  

static constexpr uint16_t VCNL_POLL_MS = 50; 

float     r = 0, p = 0, y = 0;
float     gx = 0, gy = 0, gz = 0;
uint16_t  prox = 0, amb = 0;
bool      tap = false;

static inline void _pushUnique(char* arr, uint8_t &count, char v, uint8_t maxCount) {
  for (uint8_t i = 0; i < count; ++i) if (arr[i] == v) return;
  if (count < maxCount) arr[count++] = v;
}

void LokaMCU::Init(uint16_t featuresMask) {
  _light_en = featuresMask & LIGHT;
  _rgb_en   = featuresMask & RGB;
  _rot_en   = featuresMask & ROT;
  _gyr_en   = featuresMask & GYR;
  _tap_en   = featuresMask & TAP;

  Wire.begin();
  delay(5);

  if (_rot_en || _gyr_en || _tap_en) {
    _imu_ok = _imu.begin(Wire);
    if (_imu_ok) { imuEnable_(); imuTareReset_(); }
  }

  if (_light_en) vcnlInit_();
  if (_rgb_en) _pwr_state = POWER_BAT_RUN;

  _last_tick_ms  = millis();
  _last_light_ms = _last_tick_ms;
}

void LokaMCU::Run(uint8_t hz) {
  _hz = constrain(hz, (uint8_t)1, (uint8_t)100);
  const uint32_t period_ms = 1000UL / _hz;

  while (millis() - _last_tick_ms < period_ms) {
    delay(1); 
  }
  _last_tick_ms = millis();

  // reset selection logs each tick
  _rotCount = _gyrCount = _lightCount = 0;
  _tapQueriedThisTick = false;
  _tapWasTrue = false;

  // poll sensors on the tick
  if (_imu_ok) imuPoll_();

  const uint32_t now = millis();
  if (_light_en && (now - _last_light_ms >= VCNL_POLL_MS)) {
    _last_light_ms = now;
    vcnlPoll_();

    // auto headlight helper
    if (_dark_led_en) {
      const uint16_t sense = (_dark_src == WHITE) ? _white : _amb;
      const bool wantOn = (sense < _dark_led_thr);
      digitalWrite(_dark_led_pin, (_dark_led_activeHigh ? (wantOn ? HIGH : LOW)
                                                        : (wantOn ? LOW : HIGH)));
    }
  }
}

void LokaMCU::SetDarkLED(uint8_t pin, uint16_t threshold, bool activeHigh, LokaDarkSource src) {
  _dark_led_pin = pin;
  _dark_led_thr = threshold;
  _dark_led_en  = (threshold > 0);
  _dark_led_activeHigh = activeHigh;
  _dark_src = src;
  pinMode(_dark_led_pin, OUTPUT);
  digitalWrite(_dark_led_pin, _dark_led_activeHigh ? LOW : HIGH); // off by default
}

void LokaMCU::Headlight(bool on) {
  if (!_dark_led_en) return;
  digitalWrite(_dark_led_pin, (_dark_led_activeHigh ? (on ? HIGH : LOW)
                                                    : (on ? LOW : HIGH)));
}

void LokaMCU::PwrState(PowerState s) {
  _pwr_state = s;
}

void LokaMCU::Rotation(float &rOut, float &pOut, float &yOut) { Rot(rOut, pOut, yOut); }
void LokaMCU::Gyro(float &gxOut, float &gyOut, float &gzOut)  { Gyro(gxOut); Gyro(gyOut); Gyro(gzOut); }

bool LokaMCU::TapRead() {
  const bool t = _tap_flag;
  _tap_flag = false;
  tap = t;            

  _tapQueriedThisTick = true;
  if (t) _tapWasTrue = true;
  return t;
}
void LokaMCU::TapRead(bool &tapOut) {
  tapOut = _tap_flag;
  _tap_flag = false;
  tap = tapOut;       
  _tapQueriedThisTick = true;
  if (tapOut) _tapWasTrue = true;
}

void LokaMCU::Light(uint16_t &p, uint16_t &a, uint16_t &w) { p=_prox; a=_amb; w=_white; }
void LokaMCU::Light(uint16_t &p, uint16_t &a)              { Light(p); Light(a); }


// Rot(): fills r,p,y in that order and prints as r,p,y
void LokaMCU::Rot() {
  r = _r; _pushUnique(_rotOrder, _rotCount, 'r', 3);
  p = _p; _pushUnique(_rotOrder, _rotCount, 'p', 3);
  y = _y; _pushUnique(_rotOrder, _rotCount, 'y', 3);
}
void LokaMCU::Rot(float &a) {
  if (&a == &::r) { a = _r; r = _r; _pushUnique(_rotOrder, _rotCount, 'r', 3); }
  else if (&a == &::p) { a = _p; p = _p; _pushUnique(_rotOrder, _rotCount, 'p', 3); }
  else { a = _y; y = _y; _pushUnique(_rotOrder, _rotCount, 'y', 3); }
}
void LokaMCU::Rot(float &a, float &b) { Rot(a); Rot(b); }
void LokaMCU::Rot(float &a, float &b, float &c) { Rot(a); Rot(b); Rot(c); }

// Gyro(): fills gx,gy,gz in that order and prints as x,y,z
void LokaMCU::Gyro() {
  gx = _gx; _pushUnique(_gyrOrder, _gyrCount, 'x', 3);
  gy = _gy; _pushUnique(_gyrOrder, _gyrCount, 'y', 3);
  gz = _gz; _pushUnique(_gyrOrder, _gyrCount, 'z', 3);
}
void LokaMCU::Gyro(float &a) {
  if (&a == &::gx) { a = _gx; gx = _gx; _pushUnique(_gyrOrder, _gyrCount, 'x', 3); }
  else if (&a == &::gy) { a = _gy; gy = _gy; _pushUnique(_gyrOrder, _gyrCount, 'y', 3); }
  else { a = _gz; gz = _gz; _pushUnique(_gyrOrder, _gyrCount, 'z', 3); }
}
void LokaMCU::Gyro(float &a, float &b) { Gyro(a); Gyro(b); }

// Light(): fills amb,prox in that order and prints as amb,prox
void LokaMCU::Light() {
  amb = _amb; _pushUnique(_lightOrder, _lightCount, 'a', 2);
  prox = _prox; _pushUnique(_lightOrder, _lightCount, 'p', 2);
}
void LokaMCU::Light(uint16_t &a) {
  if (&a == &::prox) { a = _prox; prox = _prox; _pushUnique(_lightOrder, _lightCount, 'p', 2); }
  else { a = _amb; amb = _amb; _pushUnique(_lightOrder, _lightCount, 'a', 2); }
}


void LokaMCU::PrintIMU(bool withLabels) {
  bool printedSomething = false;

  if (_rotCount) {
    if (withLabels) Serial.print(F("\nRot   > "));
    for (uint8_t i = 0; i < _rotCount; ++i) {
      if (i) Serial.print(F("   "));
      const char t = _rotOrder[i];
      if (withLabels) {
        if (t=='r') Serial.print(F("Roll: "));
        else if (t=='p') Serial.print(F("Pitch: "));
        else Serial.print(F("Yaw: "));
      }
      float v = (t=='r') ? r : (t=='p') ? p : y;
      Serial.printf("%.1f", v);
    }
    printedSomething = true;
  }

  if (_gyrCount) {
    if (printedSomething) Serial.print(F("\n"));
    if (withLabels) Serial.print(F("Gyro  > "));
    for (uint8_t i = 0; i < _gyrCount; ++i) {
      if (i) Serial.print(F("   "));
      const char t = _gyrOrder[i];
      if (withLabels) {
        if (t=='x') Serial.print(F("X: "));
        else if (t=='y') Serial.print(F("Y: "));
        else Serial.print(F("Z: "));
      }
      float v = (t=='x') ? gx : (t=='y') ? gy : gz;
      Serial.printf("%.1f", v);
    }
    printedSomething = true;
  }

  if (_tapQueriedThisTick && _tapWasTrue) {
    if (printedSomething) Serial.print(F("                       "));
    Serial.print(F(" ((( Tap ))) "));
    printedSomething = true;
  }

  if (printedSomething) Serial.println();
}

void LokaMCU::PrintLight(bool withLabels) {
  if (!_lightCount) return;
  if (withLabels) Serial.print(F("Light > "));
  for (uint8_t i = 0; i < _lightCount; ++i) {
    if (i) Serial.print(F("   "));
    const char t = _lightOrder[i];
    if (withLabels) Serial.print((t=='a') ? F("AMB: ") : F("PROX: "));
    uint16_t v = (t=='a') ? amb : prox;
    Serial.print(v);
  }
  Serial.println();
}

// ----- Tap sensitivity (accelerometer magnitude EMA) -----
void LokaMCU::TapSens(uint8_t level) {
  level = constrain(level, (uint8_t)1, (uint8_t)3);  // clamp to 1..3
  switch (level) {
    case 1:  _tap_alpha=0.18f; _tap_thresh=1.00f; _tap_refract_ms=180; break; // low
    case 3:  _tap_alpha=0.30f; _tap_thresh=0.35f; _tap_refract_ms=80;  break; // high
    default: _tap_alpha=0.25f; _tap_thresh=0.60f; _tap_refract_ms=120; break; // medium
  }
}

// ----- IMU internals -----
void LokaMCU::imuEnable_() {
  if (_rot_en)  _imu.enableGameRotationVector(10);
  if (_gyr_en)  _imu.enableGyro(10);
  if (_tap_en)  { _imu.enableTapDetector(10); _imu.enableAccelerometer(10); _acc_en = true; }
}

void LokaMCU::imuTareReset_() {
  _have_q0 = false;
  _r = _p = _y = 0;
  _gx = _gy = _gz = 0;
  _tap_flag = false;
  _amagEMA = 9.8f;
  _last_tap_ms = 0;
}

void LokaMCU::imuPoll_() {
  for (int i=0;i<6;++i) {
    if (!_imu.getSensorEvent()) break;
    switch (_imu.getSensorEventID()) {
      case SH2_GAME_ROTATION_VECTOR: {
        float w=_imu.getGameReal(), x=_imu.getGameI(), y=_imu.getGameJ(), z=_imu.getGameK();
        if (!_have_q0) { _q0w=w; _q0x=-x; _q0y=-y; _q0z=-z; _have_q0=true; }
        float rw,rx,ry,rz; quatMul_(w,x,y,z, _q0w,_q0x,_q0y,_q0z, rw,rx,ry,rz);
        quatToEulerDeg_(rw,rx,ry,rz, _r,_p,_y);
        break;
      }
      case SH2_GYROSCOPE_CALIBRATED:
        _gx = _imu.getGyroX() * RAD_TO_DEG;
        _gy = _imu.getGyroY() * RAD_TO_DEG;
        _gz = _imu.getGyroZ() * RAD_TO_DEG;
        break;
      case SH2_ACCELEROMETER: {
        if (_acc_en) {
          _ax = _imu.getAccelX();
          _ay = _imu.getAccelY();
          _az = _imu.getAccelZ();
          float mag = sqrtf(_ax*_ax + _ay*_ay + _az*_az);
          _amagEMA = (1.0f - _tap_alpha) * _amagEMA + _tap_alpha * mag;
          float spike = mag - _amagEMA;
          const uint32_t now = millis();
          if (spike > _tap_thresh && (now - _last_tap_ms) > _tap_refract_ms) {
            _tap_flag = true;
            _last_tap_ms = now;
          }
        }
        break;
      }
      case SH2_TAP_DETECTOR:
        _tap_flag = true;
        break;
      default: break;
    }
  }
}

// ----- VCNL4040 internals -----
bool LokaMCU::vcnlInit_() {
  Wire.beginTransmission(VCNL4040_I2C_ADDR);
  if (Wire.endTransmission() != 0) return false;
  if (!vcnlWriteU16_(VCNL4040_ALS_CONF,    0x0000)) return false;
  if (!vcnlWriteU16_(VCNL4040_PS_CONF1_2,  0x080E)) return false;
  if (!vcnlWriteU16_(VCNL4040_PS_CONF3_MS, 0x4710)) return false;
  return true;
}

bool LokaMCU::vcnlReadU16_(uint8_t reg, uint16_t &out) {
  Wire.beginTransmission(VCNL4040_I2C_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((int)VCNL4040_I2C_ADDR, 2) != 2) return false;
  uint8_t l = Wire.read(), h = Wire.read();
  out = (uint16_t)l | ((uint16_t)h << 8);
  return true;
}

bool LokaMCU::vcnlWriteU16_(uint8_t reg, uint16_t val) {
  Wire.beginTransmission(VCNL4040_I2C_ADDR);
  Wire.write(reg);
  Wire.write(val & 0xFF);
  Wire.write((val >> 8) & 0xFF);
  return Wire.endTransmission() == 0;
}

void LokaMCU::vcnlPoll_() {
  uint16_t v;
  if (vcnlReadU16_(VCNL4040_PROX,    v)) _prox  = v;
  if (vcnlReadU16_(VCNL4040_AMBIENT, v)) _amb   = v;
  if (vcnlReadU16_(VCNL4040_WHITE,   v)) _white = v;
}

void LokaMCU::quatMul_(float aw,float ax,float ay,float az,
                       float bw,float bx,float by,float bz,
                       float &rw,float &rx,float &ry,float &rz) {
  rw = aw*bw - ax*bx - ay*by - az*bz;
  rx = aw*bx + ax*bw + ay*bz - az*by;
  ry = aw*by - ax*bz + ay*bw + az*bx;
  rz = aw*bz + ax*by - ay*bx + az*bw;
}

void LokaMCU::quatToEulerDeg_(float w, float x, float y, float z,
                              float &roll, float &pitch, float &yaw) {
  float sinr_cosp = 2.0f * (w * x + y * z);
  float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
  roll = atan2f(sinr_cosp, cosr_cosp) * 180.0f / PI;

  float sinp = 2.0f * (w * y - z * x);
  if (fabsf(sinp) >= 1.0f) pitch = copysignf(90.0f, sinp);
  else pitch = asinf(sinp) * 180.0f / PI;

  float siny_cosp = 2.0f * (w * z + x * y);
  float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
  yaw = atan2f(siny_cosp, cosy_cosp) * 180.0f / PI;
}
