#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "mcu/BNO085.h"   // keep lightweight in the header; SH2 goes in the .cpp

// ---------------- Feature flags ----------------
#define LIGHT  0x0001
#define RGB    0x0002
#define ROT    0x0004
#define GYR    0x0008
#define TAP    0x0010

// ---------------- Power state (kept for completeness) ----------------
enum PowerState : uint8_t {
  POWER_BAT_RUN = 0,
  POWER_USB     = 1,
  POWER_INIT    = 2,
  POWER_BAT_LOW = 3,
  POWER_BAT_FULL= 4,
  POWER_BAT_CHRG= 5
};

// ---------------- VCNL4040 (prox/ambient) ----------------
#define VCNL4040_I2C_ADDR    0x60
#define VCNL4040_ALS_CONF    0x00
#define VCNL4040_PS_CONF1_2  0x03
#define VCNL4040_PS_CONF3_MS 0x04
#define VCNL4040_PROX        0x08
#define VCNL4040_AMBIENT     0x09
#define VCNL4040_WHITE       0x0A

enum LokaDarkSource : uint8_t { AMB=0, WHITE=1 };

class LokaMCU {
public:
  LokaMCU() {}

  // ---------- lifecycle ----------
  void Init(uint16_t featuresMask);   // enable subsystems
  void TapSens(uint8_t level);        // 1=low, 2=med, 3=high (clamped)
  void Run(uint8_t hz = 10);          // cadence gate

  // ---------- helpers ----------
  void SetDarkLED(uint8_t pin = 1, uint16_t threshold = 120, bool activeHigh = true,
                  LokaDarkSource src = AMB);
  void Headlight(bool on);
  void PwrState(PowerState s);

  // ---------- sensors (legacy, kept for back-compat) ----------
  void Rotation(float &roll, float &pitch, float &yaw);      // legacy exact-order
  void Gyro(float &gx, float &gy, float &gz);                 // legacy exact-order
  void Light(uint16_t &prox, uint16_t &ambient, uint16_t &white); // legacy 3-arg
  void Light(uint16_t &prox, uint16_t &ambient);                   // legacy 2-arg

  // Tap: expose both forms
  bool TapRead();                // one-liner: if (loka.TapRead()) ...
  void TapRead(bool &tapOut);    // beginner ref form: loka.TapRead(tap);

  // ---------- FLEX: orderless & subset reads (kids-safe) ----------
  // Rot(): fills r,p,y (in that order) and prints as r,p,y
  void Rot();                         // 0 args: take all (r,p,y)
  // Rot(any subset/order): fills and prints in the same order you pass
  void Rot(float &a);                 // 1 axis (r or p or y)
  void Rot(float &a, float &b);       // 2 axes
  void Rot(float &a, float &b, float &c); // 3 axes

  // Gyro(): fills gx,gy,gz (in that order) and prints as x,y,z
  void Gyro();                        // 0 args: take all (gx,gy,gz)
  // Gyro(any subset/order)
  void Gyro(float &a);                // 1 axis
  void Gyro(float &a, float &b);      // 2 axes
  // (3-arg legacy declared above; routes internally)

  // Light(): fills amb,prox (in that order) and prints as amb,prox
  void Light();                       // 0 args: take both (amb,prox)
  // Light(any subset/order)
  void Light(uint16_t &a);            // 1 value (prox or amb)
  // (2-arg legacy declared above)

  // ---------- Printers that reflect exactly what you requested ----------
  void PrintIMU(bool withLabels = true);
  void PrintLight(bool withLabels = true);

  // Optional direct accessors
  uint16_t LightProximity() const { return _prox; }
  uint16_t LightAmbient()  const { return _amb;  }

private:
  // IMU
  BNO085   _imu;
  bool     _imu_ok = false;
  bool     _rot_en = false, _gyr_en = false, _tap_en = false;

  float    _r = 0, _p = 0, _y = 0;    // cached euler deg
  float    _gx = 0, _gy = 0, _gz = 0; // cached gyro dps
  volatile bool _tap_flag = false;

  bool     _acc_en = false;
  float    _ax = 0, _ay = 0, _az = 0;
  float    _amagEMA = 9.8f;
  float    _tap_alpha = 0.20f;
  float    _tap_thresh = 0.6f;
  uint32_t _tap_refract_ms = 120;
  uint32_t _last_tap_ms = 0;

  bool     _have_q0 = false;
  float    _q0w = 1.0f, _q0x = 0.0f, _q0y = 0.0f, _q0z = 0.0f;

  // Light / LED
  bool     _light_en = false;
  bool     _dark_led_en = false;
  bool     _dark_led_activeHigh = true;
  uint8_t  _dark_led_pin = 1;
  uint16_t _dark_led_thr = 120;
  LokaDarkSource _dark_src = AMB;
  uint16_t _prox = 0, _amb = 0, _white = 0;

  // Misc
  bool        _rgb_en = false;
  PowerState  _pwr_state = POWER_BAT_RUN;

  uint8_t  _hz = 10;
  uint32_t _last_tick_ms = 0;
  uint32_t _last_light_ms = 0;

  // ---- Per-tick selection logs (preserve user call order) ----
  uint8_t _rotCount = 0, _gyrCount = 0, _lightCount = 0;
  char    _rotOrder[3];   // 'r','p','y'
  char    _gyrOrder[3];   // 'x','y','z'
  char    _lightOrder[2]; // 'a' (ambient), 'p' (prox)

  bool _tapQueriedThisTick = false;
  bool _tapWasTrue         = false;

  // internals
  void imuEnable_();
  void imuPoll_();
  void imuTareReset_();

  bool vcnlInit_();
  bool vcnlReadU16_(uint8_t reg, uint16_t &out);
  bool vcnlWriteU16_(uint8_t reg, uint16_t val);
  void vcnlPoll_();

  static void quatMul_(float aw,float ax,float ay,float az,
                       float bw,float bx,float by,float bz,
                       float &rw,float &rx,float &ry,float &rz);
  static void quatToEulerDeg_(float w, float x, float y, float z,
                              float &roll, float &pitch, float &yaw);
};

// --------- Beginner-friendly globals (no need to declare in the sketch) ---------
extern float r, p, y;       // rotation
extern float gx, gy, gz;    // gyro
extern uint16_t prox, amb;  // light
extern bool tap;            // tap flag
