# Acknowledgements

LokaBot is possible because of the open-source hardware and software community.

## Microbots

The LokaBot controller is the **Microbots CodeCell C3**. The examples use the official **CodeCell Arduino Library**, including its sensor, GPIO/PWM, battery, power-management, and status functions.

- Project: CodeCell
- Maintainer: Microbots
- License: MIT
- Documentation: `https://microbots.io/pages/learn-codecell`
- Source: `https://github.com/microbotsio/CodeCell`

Thank you to the Microbots team for creating a compact controller that combines an ESP32-C3, BNO085 IMU, VCNL4040 light/proximity sensor, LiPo charging, and maker-friendly I/O.

## SparkFun Electronics

The multi-zone Time-of-Flight example is based on the official **SparkFun VL53L5CX Arduino Library** and its `Example1_DistanceArray` sketch by Nathan Seidle.

- Project: SparkFun VL53L5CX Arduino Library
- Maintainer: SparkFun Electronics
- Original example author: Nathan Seidle
- License: MIT
- Source: `https://github.com/sparkfun/SparkFun_VL53L5CX_Arduino_Library`

Thank you to SparkFun Electronics for maintaining accessible Arduino drivers and examples for robotics sensors.

## Other upstream contributors

CodeCell and the SparkFun ToF library incorporate or interface with work from additional organizations and contributors, including:

- Espressif Systems for the ESP32 Arduino core
- STMicroelectronics for the VL53L5CX sensor API and underlying driver technology
- CEVA and SparkFun contributors associated with BNO08x/BNO085 support
- The Arduino open-source community

All third-party projects retain their respective copyright notices and licenses. LokaBot does not claim ownership of those libraries. The libraries are installed separately through Arduino Library Manager and are not bundled as a custom Loka library.

## LokaBot project

LokaBot mechanical design, compact hardware integration, printed parts, wiring, assembly documentation, and robot-specific example adaptations were created by **Fahad Al Ajmi**.
