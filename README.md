# LokaBot

LokaBot is an open source palm sized autonomous robot powered by ESP32 and programmed with the Arduino IDE.  
It brings advanced robotics concepts such as orientation sensing, multi zone distance mapping, and autonomous navigation into a very small and affordable platform.

<br>
<p align="center">
  <img src="images/lokabot.jpg" alt="lokabot" width="500"/>
</p>
<br>

The project also includes the **Loka Library**, which unifies the IMU, light sensor, time of flight sensor, and motor control under one simple set of functions.  
This removes the need for multiple libraries and makes it easy to try obstacle avoidance, light following, or IMU based actions.
The goal is to make robotics approachable through a compact and easy to use robot.  
LokaBot is built to support education, hobby projects, and early research by providing an affordable hands on platform.  
By combining sensing, decision making, and motion in a tiny robot, Loka hopes to bring STEM learning to more people and inspire newcomers of all ages to explore robotics.

<br><br>
<p align="center">
  <img src="images/lokabot_avoidance.GIF" alt="LokaBot avoidance" width="500"/>
</p>
<br><br>

## Key Ideas

- A single VL53L7CX provides a full 4×4 or 8×8 distance map.  
  You can print all zones, or choose specific zone to use.    
- A custom library `Loka` makes all sensors work with simple commands.  
  Instead of using three separate libraries, everything is unified.  
- The body is fully 3D printed and uses **press fit assembly without screws**,  
  making it easy to build even for beginners.  

The goal is to create a robot that is easy for students and hobbyists,  
but also shows good engineering choices for more advanced users.

## Features

- Ultra compact size: 51 × 28 × 23 mm (51 × 54 × 23 mm with wheels) 
- Zone based obstacle avoidance with VL53L7CX  
- Ambient light and proximity sensing with VCNL4040
- Orientation, gyro, and tap detection with BNO085 IMU  
- Simple motor control for DC motors  
- One piece 3D printed body and custom wheels  
- Press-fit slots for motor driver and ToF sensor (no screws needed)  
- DIY silicone tires for better grip  
- USB-C charging and onboard LiPo battery support
  
<br><br>
<p align="center">
  <img src="images/lokabot_lights.GIF" alt="LokaBot lights" width="500"/>
</p>
<br><br>

## Hardware

LokaBot combines custom 3D printed parts with low cost standard components.  
The design is compact, screw free, and uses press fit assembly for easy building.

Full BOM, 3D files, wiring diagram, and assembly guide: [hardware/README.md](hardware/README.md)

## Software

The Loka library brings all sensors and controls into one simple set of functions.
It wraps the original SparkFun drivers and makes them easier to use.  
Instead of three different libraries, you only need `Loka`.

## Supported

- **IMU BNO085**: orientation, gyro, tap detection  
- **Light sensor VCNL4040**: ambient and proximity sensing, auto headlight control  
- **Time of Flight VL53L7CX**: multi-zone ranging (Z16 or Z64) with Datasheet zone layout; select any zones to print  
- **Motors**: simple DC motor control with speed and direction
  
<br><br>
<p align="center">
  <img src="images/lokabot_motors.GIF" alt="LokaBot motors" width="500"/>
</p>
<br><br>

## Core Classes

- `LokaMCU` → IMU and light  
- `LokaToF` → time of flight distance sensing  
- `LokaMotors` → motor control  

## Examples

Quick demos are included in the `examples/` folder.  
Here are short versions to get started:
```cpp
/*
  Loka Example — Quick Start (ToF + IMU + Light + Motors ready)
  -------------------------------------------------------------
  Prints ToF grid layout, IMU, and light values.
  Motors are initialized (no movement here).
*/

#include <LokaBot.h>

// Motor pins (use PWM-capable pins on your board)
#define In1Pin1 2
#define In1Pin2 3
#define In2Pin1 5
#define In2Pin2 7

LokaMCU   mcu;                 // BNO085 IMU + VCNL4040 Light
LokaToF   tof;                 // VL53L7CX ToF
LokaMotor M1(In1Pin1, In1Pin2);
LokaMotor M2(In2Pin1, In2Pin2);

void setup() {
  Serial.begin(115200);

  // Enable IMU (rotation + gyro + tap) and Light
  mcu.Init(LIGHT + ROT + GYR + TAP);

  // ToF: choose one
  tof.Init(Z16);               // 4×4 (default ~30 Hz)
  // tof.Init(Z64);            // 8×8 (default ~15 Hz)

  tof.Zones();                 // show all zones

  M1.Init();
  M2.Init();
}

void loop() {
  // Update sensors
  mcu.Run(25);                 // IMU + Light update
  tof.PrintZones();            // auto-fetches when a new frame is ready

  // Print readings
  mcu.PrintIMU();
  mcu.PrintLight();

  delay(100);
}
```

### Motors Control
```cpp
#include <LokaBot.h>

#define In1Pin1 2
#define In1Pin2 3
#define In2Pin1 5
#define In2Pin2 7

LokaMotor M1(In1Pin1, In1Pin2);
LokaMotor M2(In2Pin1, In2Pin2);

void setup() {
  M1.Init();
  M2.Init();
}

void loop() {
  delay(3000);
  M1.Ctrl(70);     // forward 70%
  M2.Ctrl(-80);    // backward 80%

  delay(3000);
  M1.Ctrl(0);      // stop
  M2.Ctrl(100);    // forward 100%
}
```

### Time of Flight VL53L7CX
```cpp
#include <LokaBot.h>

LokaToF tof;

void setup() {
  Serial.begin(115200);

  // Choose one:
  tof.Init(Z16);     // 4×4 (default ~30 Hz)
  // tof.Init(Z64);  // 8×8 (default ~15 Hz)

  // Print all zones
  tof.Zones();

  // Or select specific zones:
  // tof.Zones(12, 13, 14, 15);                // top row (Z16)
  // tof.Zones(56, 57, 58, 59, 60, 61, 62, 63); // top row (Z64)
}

void loop() {
  // Optional: override default rate
  // tof.Run(30);

  tof.PrintZones();
  delay(100);
}
```

### Light VCNL4040
```cpp
#include <LokaBot.h>
LokaMCU loka;

void setup() {
  Serial.begin(115200);
  loka.Init(LIGHT);
  loka.SetDarkLED(1, 250, true, AMB); // auto headlight
}

void loop() {
  loka.Run(20);
  loka.Light(amb);
  loka.PrintLight();
}
```

### IMU BNO085
```cpp
#include <LokaBot.h>
LokaMCU loka;

void setup() {
  Serial.begin(115200);
  loka.Init(ROT + GYR + TAP);
  loka.TapSens(3);                    // sensitivity
}

void loop() {
  loka.Run(25);
  loka.Rot(y, r);
  loka.Gyro(gx, gz);
  loka.PrintIMU();
}
```

## Getting Started

1. **Install ESP32 boards**  
   In Arduino IDE → `Tools > Board > Board Manager` → search **esp32** → install by *Espressif Systems*.  
   Select **ESP32C3 Dev Module** and set `USB CDC On Boot` = **Enabled**.

2. **Install the library**  
   Download ZIP from the green **Code** button → Arduino IDE →  
   `Sketch > Include Library > Add .ZIP Library...`
    or  
   Clone the repo directly:  
   ```bash
   git clone https://github.com/faajmid/Loka.git
   ```

4. **Run an example**  
   Open `File > Examples > Loka > ToF` (or Light, IMU), upload, and check Serial Monitor.

## Roadmap

- Loka core library  
- Time-of-Flight sensor VL53L7CX
- Light sensor VCNL4040
- IMU BNO085  
- Motor control N10 DC motors + DRV8833 
- Assembly guide with 3D files and tire molding  
- Extra examples obstacle avoidance, following, IMU actions

## Attribution

- **CEVA SH2/SHTP drivers**  
  Portions of this code are licensed from CEVA, Inc. and included under the Apache License 2.0.  
  Copyright (c) CEVA, Inc. and its licensors. All rights reserved.  
  CEVA and the CEVA logo are trademarks of CEVA, Inc.  
  For more information, visit [CEVA's website](https://www.ceva-dsp.com/app/motion-sensing/).

- **SparkFun Libraries**  
  Inspired by SparkFun’s sensor libraries, including the  
  [VL53L5CX ToF driver](https://github.com/sparkfun/SparkFun_VL53L5CX_Arduino_Library) and  
  [BNO08x Arduino Library](https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library),  
  both released under the MIT License.

## Acknowledgements

- [Microbots CodeCell](https://github.com/microbotsio/CodeCell)  
  for both the CodeCell ESP32-C3 hardware platform and the library style,  
  which inspired the simplified initialization (`Init()`) approach in Loka.


## License

This project is licensed under the MIT License.  
See the [LICENSE](LICENSE) file for details.

