/*
  Loka Example — IMU (Guide)
  ---------------------------------------
  Project: Loka Robot
  Author : Fahad Al Ajmi
  GitHub : https://github.com/faajmid/Loka
  License: MIT

  Overview
  This example shows how to read Euler rotation (roll/pitch/yaw), gyroscope rates,
  and tap events from the BNO085, then print exactly what you requested each loop.

  Features demonstrated
  • Enable Rotation, Gyro, and Tap
  • Tap sensitivity (1 = low, 2 = medium, 3 = very sensitive)
  • Orderless, subset-friendly reads for Rot/Gyro
  • Print lines that mirror your read choices (and order)

  API summary
    Init(ROT + GYR + TAP);         // enable IMU subsystems
    TapSens(level);                 // 1..3
    Run(hz);                        // poll/cache IMU at 'hz'

    // Rotation (BNO085 Euler) — flexible & orderless
    Rot();                          // all: r,p,y   (order r,p,y)
    Rot(y, r);                      // any subset, any order

    // Gyroscope (BNO085) — flexible & orderless
    Gyro();                         // all: gx,gy,gz (order x,y,z)
    Gyro(gx, gz);                   // any subset, any order

    // Tap
    TapRead();                      // returns true once when a tap occurs this loop

    // Printer (reflects exactly what you read this tick, in your order)
    PrintIMU();                     // "Rot > …   Gyro > …  -------> ((( Tap )))"
*/

#include <Loka.h>

LokaMCU loka;

void setup() {
  Serial.begin(115200);
  loka.Init(ROT + GYR + TAP);
  loka.TapSens(3);                  // 1..3
}

void loop() {
  loka.Run(25);                     // ~25 Hz

  // Choose what you want (any order)
  loka.Rot(y, r);                   // yaw, roll
  loka.Gyro(gx, gz);                // x, z rates
  loka.TapRead();                   // tap flag (once per tap)

  // Prints exactly what you read, same order:
  loka.PrintIMU();                  // e.g. "Rot > y: …  r: …   Gyro > x: … z: …  -------> ((( Tap )))"
}
