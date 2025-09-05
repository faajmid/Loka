/*
  Loka Example — ToF (Guide)
  ---------------------------------------
  Project: Loka Robot
  Author : Fahad Al Ajmi
  GitHub : https://github.com/faajmid/Loka
  License: MIT

  Overview
  This example shows how to use the VL53L7CX multi-zone time-of-flight sensor on Loka.
  You choose a resolution (4×4 or 8×8), select Left/Middle/Right groups (defaults or
  custom), and print the grid plus quick group averages for steering logic.

  What this shows
    • Use VL53L7CX ToF in 16-zone (4×4) or 64-zone (8×8) mode
    • Print zones in a grid , oriented for Loka (zone 0 = TOP-RIGHT)
    • Get averages for Left, Middle, Right groups
    • Select your own zones or just use defaults
    • Use Error() = LeftAvg – RightAvg for simple steering

  API you’ll use
    Init(Z16 / Z64);            // choose resolution
    Run(hz);                    // update rate (Hz, 1–100)
    PrintZones();               // print grid + L/M/R averages
    PrintZonesAvg();            // print just L/M/R averages

    Left(); Right(); Middle();                // defaults (full columns)
    Left(3,7,11,15);                          // custom zone indices
    Middle(1,2,5,6,9,10,13,14);               // custom zones
    Right(0,4,8,12);                          // custom zones

    LeftAvg();  MiddleAvg();  RightAvg();     // averages (mm)
    Error();                                  // LeftAvg – RightAvg (mm)
    ZoneValue(i);                             // single zone distance (mm)

  ------------------------
  Zone order (Loka mount)
  ------------------------
  • Printing is TOP → BOTTOM, LEFT → RIGHT
  • Zone 0 starts at the TOP-RIGHT corner

  Z16 (4×4 grid):
      Row0 (top):     3   2   1   0   ← zone 0 here
      Row1:           7   6   5   4
      Row2:          11  10   9   8
      Row3 (bottom): 15  14  13  12

  Z64 (8×8 grid):
      Row0 (top):    7  6  5  4  3  2  1  0  ← zone 0 here
      Row1:         15 14 13 12 11 10  9  8
      Row2:         23 22 21 20 19 18 17 16
      Row3:         31 30 29 28 27 26 25 24
      Row4:         39 38 37 36 35 34 33 32
      Row5:         47 46 45 44 43 42 41 40
      Row6:         55 54 53 52 51 50 49 48
      Row7 (bot):   63 62 61 60 59 58 57 56

  Notes
    • Unselected zones print as "."
    • Selected zones with no valid reading print "-"
    • Averages (L/M/R) are shown under the grid
    • Typical max frame rates:
        Z16 (4×4): ~30 Hz
        Z64 (8×8): ~15 Hz
*/

#include <Loka.h>

LokaToF tof;

void setup() {
  Serial.begin(115200);

  // Choose resolution: Z16 (16 zones) or Z64 (64 zones)
  tof.Init(Z16);

  // Defaults (full columns)
  tof.Left();
  tof.Middle();
  tof.Right();

  // Example of custom groups:
  // tof.Left(3,7,11,15);
  // tof.Middle(1,2,5,6,9,10,13,14);
  // tof.Right(0,4,8,12);
}

void loop() {
  tof.Run(30);          // sensor update at 10 Hz (safe for both Z16/Z64)
  tof.PrintZones();     // grid
  tof.PrintZonesAvg();  // L/M/R averages
  delay(100);
}
