#include "LokaToF.h"

LokaToF::LokaToF()
: _res(Z16), _loopHz(10), _rangeHz(15), _lastTickMs(0),
  _leftCount(0), _midCount(0), _rightCount(0)
{
  for (int i = 0; i < 64; ++i) { _distances[i] = -1; _mask[i] = false; }
}

bool LokaToF::Init(LokaToFRes res) {
  _res = res;

  Wire.begin();
  Wire.setClock(400000); // 400 kHz I2C

  if (!_sensor.begin(0x29, Wire)) return false;

  if (_res == Z16) _sensor.setResolution(16);
  else             _sensor.setResolution(64);

  _rangeHz = 15;                       // Z64 max ~15 Hz (Z16 can go up to ~30 Hz)
  _sensor.setRangingFrequency(_rangeHz);
  _sensor.startRanging();

  setDefaultGroups_();                 // physical defaults

  _lastTickMs = millis();
  return true;
}

void LokaToF::Run(uint8_t hz) {
  if (hz < 1) hz = 1;
  if (hz > 100) hz = 100;
  _loopHz = hz;

  if (_rangeHz != _loopHz) {
    _sensor.setRangingFrequency(_loopHz);
    _rangeHz = _loopHz;
  }

  const uint32_t periodMs = 1000UL / _loopHz;
  if (millis() - _lastTickMs < periodMs) return;
  _lastTickMs = millis();

  if (_sensor.isDataReady()) {
    VL53L5CX_ResultsData frame;
    if (_sensor.getRangingData(&frame)) {
      const uint8_t n = zoneCount_();
      for (uint8_t i = 0; i < n; ++i) _distances[i] = frame.distance_mm[i];
    }
  }
}

// ---------- Group setup ----------
void LokaToF::Left()   { defaultLeft_(); }
void LokaToF::Middle() { defaultMiddle_(); }
void LokaToF::Right()  { defaultRight_(); }

void LokaToF::Left(std::initializer_list<uint8_t> zones)   { setGroup_(zones, _leftIdx,  _leftCount);  }
void LokaToF::Middle(std::initializer_list<uint8_t> zones) { setGroup_(zones, _midIdx,   _midCount);   }
void LokaToF::Right(std::initializer_list<uint8_t> zones)  { setGroup_(zones, _rightIdx, _rightCount); }

void LokaToF::setGroup_(const std::initializer_list<uint8_t>& in, uint8_t* buf, uint8_t& count) {
  count = 0;
  const uint8_t n = zoneCount_();
  for (auto z : in) {
    if (z < n && count < 64) buf[count++] = z;
  }
}

void LokaToF::setGroupFromArray_(const uint8_t* src, uint8_t n, uint8_t* dst, uint8_t& count) {
  count = 0;
  const uint8_t maxN = zoneCount_();
  for (uint8_t i = 0; i < n; ++i) {
    uint8_t z = src[i];
    if (z < maxN && count < 64) dst[count++] = z;
  }
}

// ---------- Physical defaults (match your Loka orientation) ----------
// Printing shows top row first with 0 at TOP-RIGHT.
// Z16 default groups:
//   LEFT  = {3,7,11,15}      (leftmost column)
//   RIGHT = {0,4,8,12}       (rightmost column)
//   MIDDLE= {1,2,5,6,9,10,13,14}
//
// Z64 default groups:
//   LEFT  = columns 7 & 6    (leftmost two columns)
//   RIGHT = columns 0 & 1    (rightmost two columns)
//   MIDDLE= columns 2..5

void LokaToF::defaultLeft_() {
  _leftCount = 0;
  if (_res == Z16) {
    const uint8_t L[] = {3,7,11,15};
    for (uint8_t i=0;i<4;i++) _leftIdx[_leftCount++] = L[i];
  } else {
    for (uint8_t row=0; row<8; ++row) {          // cols 7 & 6
      _leftIdx[_leftCount++] = row*8 + 7;
      _leftIdx[_leftCount++] = row*8 + 6;
    }
  }
}

void LokaToF::defaultMiddle_() {
  _midCount = 0;
  if (_res == Z16) {
    const uint8_t M[] = {1,2,5,6,9,10,13,14};
    for (uint8_t i=0;i<8;i++) _midIdx[_midCount++] = M[i];
  } else {
    for (uint8_t row=0; row<8; ++row) {          // cols 2..5
      _midIdx[_midCount++] = row*8 + 2;
      _midIdx[_midCount++] = row*8 + 3;
      _midIdx[_midCount++] = row*8 + 4;
      _midIdx[_midCount++] = row*8 + 5;
    }
  }
}

void LokaToF::defaultRight_() {
  _rightCount = 0;
  if (_res == Z16) {
    const uint8_t R[] = {0,4,8,12};
    for (uint8_t i=0;i<4;i++) _rightIdx[_rightCount++] = R[i];
  } else {
    for (uint8_t row=0; row<8; ++row) {          // cols 0 & 1
      _rightIdx[_rightCount++] = row*8 + 0;
      _rightIdx[_rightCount++] = row*8 + 1;
    }
  }
}

void LokaToF::setDefaultGroups_() {
  defaultLeft_();
  defaultMiddle_();
  defaultRight_();
}

// ---------- Values ----------
int LokaToF::avgOf_(const uint8_t* idx, uint8_t count) {
  if (count == 0) return -1;
  long sum = 0;
  int valid = 0;
  for (uint8_t i = 0; i < count; ++i) {
    int16_t d = _distances[idx[i]];
    if (d > 0) { sum += d; valid++; }
  }
  return (valid > 0) ? int(sum / valid) : -1;
}

int LokaToF::LeftAvg()   { if (_leftCount  == 0) defaultLeft_();   return avgOf_(_leftIdx,  _leftCount); }
int LokaToF::MiddleAvg() { if (_midCount   == 0) defaultMiddle_(); return avgOf_(_midIdx,   _midCount);  }
int LokaToF::RightAvg()  { if (_rightCount == 0) defaultRight_();  return avgOf_(_rightIdx, _rightCount); }

int LokaToF::ZoneValue(int index) {
  const uint8_t n = zoneCount_();
  if (index < 0 || index >= n) return -1;
  return _distances[index];
}

int LokaToF::Error() {
  const int L = LeftAvg();
  const int R = RightAvg();
  if (L < 0 || R < 0) return 0;
  return L - R;
}

// ---------- Mask building & printing ----------
void LokaToF::buildMask_() {
  // If a side has no zones set, apply physical defaults for that side only.
  if (_leftCount  == 0) defaultLeft_();
  if (_midCount   == 0) defaultMiddle_();
  if (_rightCount == 0) defaultRight_();

  for (uint8_t i=0; i<64; ++i) _mask[i] = false;
  for (uint8_t i=0; i<_leftCount;  ++i)  _mask[_leftIdx[i]]  = true;
  for (uint8_t i=0; i<_midCount;   ++i)  _mask[_midIdx[i]]   = true;
  for (uint8_t i=0; i<_rightCount; ++i)  _mask[_rightIdx[i]] = true;
}

// Print TOP→BOTTOM, LEFT→RIGHT, with zone 0 at TOP-RIGHT.
// Formula: display (row=r, col=c) maps to index idx = r*w + (w-1 - c)
void LokaToF::printGrid_() {
  const uint8_t w = gridW_();
  const uint8_t n = zoneCount_();
  const uint8_t h = n / w;

  for (uint8_t r = 0; r < h; ++r) {            // r = 0 .. top row
    for (uint8_t c = 0; c < w; ++c) {          // c = 0 .. left -> right
      const uint8_t idx = r * w + (w - 1 - c);

      if (!_mask[idx]) {
        Serial.print(F("."));                  // not selected
      } else {
        const int v = _distances[idx];
        if (v <= 0) Serial.print(F("-"));      // selected but no valid reading
        else        Serial.print(v);           // selected & valid
      }

      if (c < w - 1) Serial.print('\t');
    }
    Serial.println();
  }
}

// --- NEW: split printers ---
void LokaToF::PrintZones() {
  buildMask_();
  printGrid_();           // grid only
  Serial.println();       // spacer
}

void LokaToF::PrintZonesAvg() {
  Serial.print(F("L Avg: "));
  Serial.print(LeftAvg());
  Serial.print(F(" | M Avg: "));
  Serial.print(MiddleAvg());
  Serial.print(F(" | R Avg: "));
  Serial.println(RightAvg());
}
