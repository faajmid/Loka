// LokaToF.cpp
#include "LokaToF.h"

LokaToF::LokaToF()
: _res(Z16), _loopHz(30), _rangeHz(30), _lastTickMs(0), _selectAll(true), _selCount(0) {
  for (uint8_t i = 0; i < 64; ++i) { _dist[i] = -1; _mask[i] = false; }
}

bool LokaToF::Init(LokaToFRes res) {
  _res = res;
  Wire.begin();
  Wire.setClock(400000);
  if (!_sensor.begin(0x29, Wire)) return false;

  _sensor.setResolution((_res == Z16) ? 16 : 64);
  _rangeHz = (_res == Z16) ? 30 : 15;
  _loopHz  = _rangeHz;
  _sensor.setRangingFrequency(_rangeHz);
  _sensor.startRanging();

  _lastTickMs = millis();
  _selectAll = true;
  _selCount = 0;
  return true;
}

void LokaToF::Run(uint8_t hz) {
  if (_res == Z16) { if (hz < 1) hz = 1; if (hz > 60) hz = 60; }
  else              { if (hz < 1) hz = 1; if (hz > 15) hz = 15; }

  _loopHz = hz;
  if (_rangeHz != _loopHz) { _sensor.setRangingFrequency(_loopHz); _rangeHz = _loopHz; }

  const uint32_t periodMs = 1000UL / _loopHz;
  if (millis() - _lastTickMs < periodMs) return;
  _lastTickMs = millis();

  if (_sensor.isDataReady()) readFrame_();
}

void LokaToF::readFrame_() {
  VL53L5CX_ResultsData frame;
  if (!_sensor.getRangingData(&frame)) return;

  const uint8_t w = width_();
  const uint8_t h = w;
  const uint8_t n = count_();

  for (uint8_t i = 0; i < n; ++i) {
    uint8_t r = i / w;
    uint8_t c = i % w;
    uint8_t idx = (h - 1 - r) * w + (w - 1 - c);
    int16_t v = (int16_t)frame.distance_mm[i];
    _dist[idx] = (v > 0) ? v : -1;
  }
}

void LokaToF::Zones() {
  _selectAll = true;
  _selCount = 0;
}

void LokaToF::Zones(std::initializer_list<uint8_t> ids) {
  _selectAll = false;
  _selCount = 0;
  const uint8_t n = count_();
  for (auto z : ids) if (z < n && _selCount < 64) _sel[_selCount++] = z;
}

void LokaToF::buildMask_() {
  const uint8_t n = count_();
  for (uint8_t i = 0; i < 64; ++i) _mask[i] = false;
  if (_selectAll || _selCount == 0) {
    for (uint8_t i = 0; i < n; ++i) _mask[i] = true;
  } else {
    for (uint8_t i = 0; i < _selCount; ++i) _mask[_sel[i]] = true;
  }
}

void LokaToF::printGrid_() {
  const uint8_t w = width_();
  const uint8_t h = w;
  for (int y = w * (h - 1); y >= 0; y -= w) {
    for (uint8_t x = 0; x < w; ++x) {
      const uint8_t id = (uint8_t)(y + x);
      if (!_mask[id]) {
        Serial.print('.');
      } else {
        int v = _dist[id];
        if (v <= 0) Serial.print('-'); else Serial.print(v);
      }
      if (x < w - 1) Serial.print('\t');
    }
    Serial.println();
  }
}

void LokaToF::PrintZones() {
  if (_sensor.isDataReady()) readFrame_();
  buildMask_();
  printGrid_();
  Serial.println();
}
