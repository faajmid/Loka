#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <type_traits>
#include "tof/SparkFun_VL53L5CX_Library.h"

// Supported resolutions
enum LokaToFRes : uint8_t {
  Z16, // 4x4 grid (16 zones)
  Z64  // 8x8 grid (64 zones)
};

class LokaToF {
public:
  LokaToF();

  // --- Setup ---
  bool Init(LokaToFRes res);   // Init sensor with resolution (Z16 or Z64)
  void Run(uint8_t hz);        // Call in loop; 'hz' = desired update rate (1..100)

  // --- Group setup ---
  // No-arg: set that side to DEFAULT physical mapping (others untouched)
  void Left();
  void Middle();
  void Right();

  // Brace-list style: Left({3,7,11,15})
  void Left(std::initializer_list<uint8_t> zones);
  void Middle(std::initializer_list<uint8_t> zones);
  void Right(std::initializer_list<uint8_t> zones);

  // Comma-style convenience: Left(3,7), Middle(1,2,5,6), Right(0,4) ...
  template<typename... Z,
           typename std::enable_if<(sizeof...(Z) > 0), int>::type = 0>
  void Left(Z... z)  {
    const uint8_t tmp[] = { (uint8_t)z... };
    setGroupFromArray_(tmp, (uint8_t)sizeof...(z), _leftIdx,  _leftCount);
  }
  template<typename... Z,
           typename std::enable_if<(sizeof...(Z) > 0), int>::type = 0>
  void Middle(Z... z){
    const uint8_t tmp[] = { (uint8_t)z... };
    setGroupFromArray_(tmp, (uint8_t)sizeof...(z), _midIdx,   _midCount);
  }
  template<typename... Z,
           typename std::enable_if<(sizeof...(Z) > 0), int>::type = 0>
  void Right(Z... z) {
    const uint8_t tmp[] = { (uint8_t)z... };
    setGroupFromArray_(tmp, (uint8_t)sizeof...(z), _rightIdx, _rightCount);
  }

  // --- Values for logic (physical sides) ---
  int  LeftAvg();             // average of physical LEFT zones (mm), -1 if none
  int  MiddleAvg();           // average of MIDDLE zones (mm), -1 if none
  int  RightAvg();            // average of physical RIGHT zones (mm), -1 if none
  int  ZoneValue(int index);  // raw distance of one zone (mm), -1 if OOR
  int  Error();               // LeftAvg() - RightAvg() (for PD steering)

  // Back-compat aliases
  int  LeftValue()   { return LeftAvg();   }
  int  MiddleValue() { return MiddleAvg(); }
  int  RightValue()  { return RightAvg();  }

  // --- Print ---
  void PrintZones();     // grid only (no averages)
  void PrintZonesAvg();  // one-line averages (L | M | R)

private:
  // Driver
  SparkFun_VL53L5CX _sensor;

  // Config / timing
  LokaToFRes _res;
  uint8_t    _loopHz;
  uint8_t    _rangeHz;
  uint32_t   _lastTickMs;

  // Last frame (always keep 64; use first 16 for Z16)
  int16_t    _distances[64];

  // Groups as index lists (PHYSICAL sides)
  uint8_t _leftIdx[64];   uint8_t _leftCount;   // physical LEFT
  uint8_t _midIdx[64];    uint8_t _midCount;    // middle
  uint8_t _rightIdx[64];  uint8_t _rightCount;  // physical RIGHT

  // Mask of zones to show (others print '.')
  bool _mask[64];

  // Helpers
  void   setDefaultGroups_();                  // set all three defaults (physical)
  void   defaultLeft_();                       // physical LEFT defaults
  void   defaultMiddle_();                     // middle defaults
  void   defaultRight_();                      // physical RIGHT defaults
  void   setGroup_(const std::initializer_list<uint8_t>& in, uint8_t* buf, uint8_t& count);
  void   setGroupFromArray_(const uint8_t* src, uint8_t n, uint8_t* dst, uint8_t& count);
  void   buildMask_();                         // union of L/M/R (whatever is set)
  int    avgOf_(const uint8_t* idx, uint8_t count);
  void   printGrid_();                         // TOP→BOTTOM, LEFT→RIGHT; zone 0 at top-right
  uint8_t zoneCount_() const { return (_res == Z16) ? 16 : 64; }
  uint8_t gridW_()    const { return (_res == Z16) ? 4  : 8;  }
};
