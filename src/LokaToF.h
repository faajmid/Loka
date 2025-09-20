// LokaToF.h
#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "tof/SparkFun_VL53L5CX_Library.h"

enum LokaToFRes : uint8_t { Z16, Z64 };

class LokaToF {
public:
  LokaToF();
  bool Init(LokaToFRes res);
  void Run(uint8_t hz = 30);
  void Zones();
  void Zones(std::initializer_list<uint8_t> ids);
  template<typename... Z>
  void Zones(Z... z) { Zones(std::initializer_list<uint8_t>{ static_cast<uint8_t>(z)... }); }
  void PrintZones();

private:
  SparkFun_VL53L5CX _sensor;
  LokaToFRes _res;
  uint8_t _loopHz;
  uint8_t _rangeHz;
  uint32_t _lastTickMs;
  int16_t _dist[64];
  bool _selectAll;
  bool _mask[64];
  uint8_t _sel[64];
  uint8_t _selCount;

  uint8_t count_() const { return (_res == Z16) ? 16 : 64; }
  uint8_t width_() const { return (_res == Z16) ? 4 : 8; }

  void readFrame_();
  void buildMask_();
  void printGrid_();
};
