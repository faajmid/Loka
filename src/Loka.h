#pragma once
#include <Arduino.h>

#include "LokaMCU.h"   // IMU BNO085, Light VCNL4040, Tap
#include "LokaToF.h"   // ToF VL53L7CX

// Motors are WIP and intentionally excluded from builds for now.
// When ready, I'll re-enable with a simple feature flag:
//
// #define LOKA_ENABLE_MOTORS 1
// #ifdef LOKA_ENABLE_MOTORS
// #include "LokaMotors.h"
// #endifc
