/*
  LokaBot - multi-zone Time-of-Flight test

  Based on SparkFun's Example1_DistanceArray by Nathan Seidle.
  SparkFun Electronics, MIT License.
  https://github.com/sparkfun/SparkFun_VL53L5CX_Arduino_Library

  This sketch reads a 4x4 frame, prints the distance grid in millimetres,
  and calculates left, centre and right obstacle distances for robot use.
*/

#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>

SparkFun_VL53L5CX tof;
VL53L5CX_ResultsData frame;

constexpr uint8_t RESOLUTION = 4;
constexpr uint8_t RANGING_HZ = 15;
constexpr uint16_t OBSTACLE_MM = 180;

uint16_t averageValid(const uint8_t *zones, size_t count) {
  uint32_t sum = 0;
  uint16_t valid = 0;

  for (size_t i = 0; i < count; ++i) {
    const uint16_t distance = frame.distance_mm[zones[i]];
    if (distance > 0 && distance < 4000) {
      sum += distance;
      ++valid;
    }
  }

  return valid ? static_cast<uint16_t>(sum / valid) : 0;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();
  Wire.setClock(400000);

  Serial.println("Initializing LokaBot ToF sensor...");
  if (!tof.begin()) {
    Serial.println("ToF sensor not found. Check 3.3 V, GND, SDA and SCL.");
    while (true) delay(1000);
  }

  tof.setResolution(RESOLUTION * RESOLUTION);
  tof.setRangingFrequency(RANGING_HZ);
  tof.startRanging();

  Serial.println("ToF ready. Distances are printed in millimetres.");
}

void loop() {
  if (!tof.isDataReady()) {
    delay(5);
    return;
  }

  if (!tof.getRangingData(&frame)) {
    Serial.println("Failed to read ToF frame");
    return;
  }

  // SparkFun's driver returns the frame transposed relative to the datasheet.
  for (int row = 0; row < RESOLUTION; ++row) {
    const int base = row * RESOLUTION;
    for (int column = RESOLUTION - 1; column >= 0; --column) {
      Serial.print(frame.distance_mm[base + column]);
      Serial.print(column == 0 ? '\n' : '\t');
    }
  }

  // Raw frame groups used for a simple obstacle-avoidance decision.
  const uint8_t leftZones[]   = {3, 7, 11, 15};
  const uint8_t centreZones[] = {1, 2, 5, 6, 9, 10, 13, 14};
  const uint8_t rightZones[]  = {0, 4, 8, 12};

  const uint16_t left = averageValid(leftZones, 4);
  const uint16_t centre = averageValid(centreZones, 8);
  const uint16_t right = averageValid(rightZones, 4);

  Serial.print("Left: "); Serial.print(left);
  Serial.print(" mm | Centre: "); Serial.print(centre);
  Serial.print(" mm | Right: "); Serial.print(right);
  Serial.print(" mm | Decision: ");

  if (centre == 0) {
    Serial.println("NO VALID CENTRE READING");
  } else if (centre >= OBSTACLE_MM) {
    Serial.println("FORWARD");
  } else if (left > right) {
    Serial.println("TURN LEFT");
  } else {
    Serial.println("TURN RIGHT");
  }

  Serial.println();
}
