/*
  LokaBot - CodeCell onboard sensors

  Uses the official CodeCell Arduino library by Microbots.
  Original library and documentation:
  https://github.com/microbotsio/CodeCell
  https://microbots.io/pages/learn-codecell

  The CodeCell library is MIT licensed. This LokaBot integration example
  keeps the upstream API visible instead of wrapping it in a custom library.
*/

#include <CodeCell.h>

CodeCell myCodeCell;

void setup() {
  Serial.begin(115200);
  delay(1000);

  myCodeCell.Init(
    LIGHT +
    MOTION_ROTATION_NO_MAG +
    MOTION_GYRO +
    MOTION_ACCELEROMETER +
    MOTION_STATE +
    MOTION_TAP_DETECTOR
  );

  Serial.println("LokaBot CodeCell sensor test");
}

void loop() {
  if (!myCodeCell.Run(20)) return;

  float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
  float gx = 0.0f, gy = 0.0f, gz = 0.0f;
  float ax = 0.0f, ay = 0.0f, az = 0.0f;

  myCodeCell.Motion_RotationNoMagRead(roll, pitch, yaw);
  myCodeCell.Motion_GyroRead(gx, gy, gz);
  myCodeCell.Motion_AccelerometerRead(ax, ay, az);

  const uint16_t proximity = myCodeCell.Light_ProximityRead();
  const uint16_t ambient = myCodeCell.Light_AmbientRead();
  const uint16_t white = myCodeCell.Light_WhiteRead();
  const uint8_t motionState = myCodeCell.Motion_StateRead();
  const bool tapped = myCodeCell.Motion_TapRead();
  const uint16_t battery = myCodeCell.BatteryLevelRead();

  Serial.print("Proximity: "); Serial.print(proximity);
  Serial.print(" | Ambient: "); Serial.print(ambient);
  Serial.print(" | White: "); Serial.print(white);

  Serial.print(" | RPY: ");
  Serial.print(roll, 1); Serial.print(", ");
  Serial.print(pitch, 1); Serial.print(", ");
  Serial.print(yaw, 1);

  Serial.print(" | Gyro: ");
  Serial.print(gx, 1); Serial.print(", ");
  Serial.print(gy, 1); Serial.print(", ");
  Serial.print(gz, 1);

  Serial.print(" | Accel: ");
  Serial.print(ax, 2); Serial.print(", ");
  Serial.print(ay, 2); Serial.print(", ");
  Serial.print(az, 2);

  Serial.print(" | Motion state: "); Serial.print(motionState);
  Serial.print(" | Battery: ");

  if (battery <= 100) {
    Serial.print(battery);
    Serial.print('%');
  } else if (battery == 101) {
    Serial.print("Charging");
  } else if (battery == 102) {
    Serial.print("USB power");
  }

  if (tapped) Serial.print(" | TAP");
  Serial.println();
}
