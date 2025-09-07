# LokaBot

LokaBot is an open source palm sized autonomous robot powered by ESP32 and programmed with the Arduino IDE.  
It brings advanced robotics concepts such as orientation sensing, multi zone distance mapping, and autonomous navigation into a very small and affordable platform.


<br>
<p align="center">
  <img src="images/lokabot.jpg" alt="lokabot" width="500"/>
</p>
<br>


The project also includes the **Loka Library**, which unifies the IMU, light sensor, time of flight sensor, and motor control under one simple API.  
This removes the need for multiple libraries and makes it easy to try obstacle avoidance, light following, or IMU based actions.
<br><br>
<p align="center">
  <img src="images/lokabot_avoidance.GIF" alt="LokaBot avoidance" width="500"/>
</p>
<br><br>
The goal is to make robotics approachable through a compact and easy to use robot.  
LokaBot is built to support education, hobby projects, and early research by providing an affordable hands on platform.  
By combining sensing, decision making, and motion in a tiny robot, Loka hopes to bring STEM learning to more people and inspire newcomers of all ages to explore robotics.

## Key Ideas

- One VL53L7CX sensor is split into left, middle, and right zones for navigation.  
  This replaces the need for multiple distance sensors.  
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
- Simple motor control API for left and right motors (coming soon)  
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

Loka is built with a mix of custom 3D printed parts and standard components.  
The design keeps the part count low while still covering the essentials.

- **CodeCell board** ESP32-C3 with onboard VCNL4040 light sensor and BNO085 IMU
- **VL53L7CX time of flight sensor** for 4×4 or 8×8 distance zones  
- **Two N10 DC motors 140 RPM** with DRV8833 motor driver  
- **LiPo battery**: 200 mAh tested, 400 mAh recommended for longer runtime  
- **LED battery indicator module**  
- **Mini power switch with LED**  
- **One piece 3D printed body** and **custom wheels**  
- **DIY silicone tires** molded to fit the printed wheels  
- **Front caster ball** made from a small perfume roller ball

Detailed sourcing links are provided in the `hardware/` folder.

## Software

The Loka library brings all sensors and controls into one simple API.  
It wraps the original SparkFun drivers and makes them easier to use.  
Instead of three different libraries, you only need `Loka`.

## Supported

- **IMU BNO085**: orientation, gyro, tap detection  
- **Light sensor VCNL4040**: ambient and proximity sensing, auto headlight control  
- **Time of Flight VL53L7CX**: multi-zone ranging, split into Left / Middle / Right  
- **Motors**: simple control API for left and right motors *(coming soon)*
<br><br>
<p align="center">
  <img src="images/lokabot_motors.GIF" alt="LokaBot motors" width="500"/>
</p>
<br><br>

## Core Classes

- `LokaMCU` → IMU and light  
- `LokaToF` → time of flight distance sensing  
- `LokaMotors` → motor control  

## Example

```cpp
#include <Loka.h>

LokaMCU mcu;
LokaToF tof;

void setup() {
  Serial.begin(115200);
  mcu.Init(LIGHT + ROT + GYR + TAP);  // enable IMU + Light
  tof.Init(Z16);                      // ToF in 4×4 mode
}

void loop() {
  mcu.Run();                          // update IMU + Light
  tof.Run(10);                        // update ToF at 10 Hz

  // IMU + Light printing
  mcu.PrintIMU();
  mcu.PrintLight();

  // ToF printing
  tof.PrintZones();                   // full grid
  tof.PrintZonesAvg();                // averages: Left | Middle | Right
  delay(100);
}
```

## Examples

Quick demos are included in the `examples/` folder.  
Here are short versions to get started:

### Time of Flight VL53L7CX
```cpp
#include <Loka.h>
LokaToF tof;

void setup() {
  Serial.begin(115200);
  tof.Init(Z16);                    // choose Z16 for 4×4 or Z64 for 8×8
  tof.Left();                       // default groups of zones
  tof.Middle();
  tof.Right();
}

void loop() {
  tof.Run(30);
  tof.PrintZones();                 // Print zones grid
  tof.PrintZonesAvg();              // L | M | R averages
  delay(100);
}
```

### Light VCNL4040
```cpp
#include <Loka.h>
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
#include <Loka.h>
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

