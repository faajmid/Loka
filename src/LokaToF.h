#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <type_traits>
#include "tof/SparkFun_VL53L5CX_Library.h"

// Supported resolutions
enum LokaToFRes : uint8_t {
  Z16, // 4x4 grid 16 zones
  Z64  // 8x8 grid 64 zones
};

class LokaToF {
public:
  LokaToF();

  // Setup
  bool Init(LokaToFRes res);   // Init sensor with resolution (Z16 or Z64)
  void Run(uint8_t hz);

  // Group setup
  void Left();
  void Middle();
  void Right();

  void Left(std::initializer_list<uint8_t> zones);
  void Middle(std::initializer_list<uint8_t> zones);
  void Right(std::initializer_list<uint8_t> zones);

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

  // Values
  int  LeftAvg();             // average of physical LEFT zones
  int  MiddleAvg();           // average of MIDDLE zones 
  int  RightAvg();            // average of physical RIGHT zones
  int  ZoneValue(int index);  // distance of one zone
  int  Error();               // LeftAvg() - RightAvg() PD Control

  int  LeftValue()   { return LeftAvg();   }
  int  MiddleValue() { return MiddleAvg(); }
  int  RightValue()  { return RightAvg();  }

  // Print
  void PrintZones();     // grid only (no averages)
  void PrintZonesAvg();  // one line averages

private:
  // Driver
  SparkFun_VL53L5CX _sensor;

  LokaToFRes _res;
  uint8_t    _loopHz;
  uint8_t    _rangeHz;
  uint32_t   _lastTickMs;

  int16_t    _distances[64];

  uint8_t _leftIdx[64];   uint8_t _leftCount;   // physical LEFT
  uint8_t _midIdx[64];    uint8_t _midCount;    // middle
  uint8_t _rightIdx[64];  uint8_t _rightCount;  // physical RIGHT

  bool _mask[64];

  void   setDefaultGroups_();
  void   defaultLeft_();
  void   defaultMiddle_();
  void   defaultRight_();
  void   setGroup_(const std::initializer_list<uint8_t>& in, uint8_t* buf, uint8_t& count);
  void   setGroupFromArray_(const uint8_t* src, uint8_t n, uint8_t* dst, uint8_t& count);
  void   buildMask_();
  int    avgOf_(const uint8_t* idx, uint8_t count);
  void   printGrid_();
  uint8_t zoneCount_() const { return (_res == Z16) ? 16 : 64; }
  uint8_t gridW_()    const { return (_res == Z16) ? 4  : 8;  }
};
