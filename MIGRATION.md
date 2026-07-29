# Migration from the former Loka library

The repository has moved from a custom Arduino library to a hardware-first project.

## Removed public API

The following former wrapper files and examples were removed:

- `LokaBot.h`
- `LokaMCU.h/.cpp`
- `LokaMotors.h/.cpp`
- `LokaToF.h/.cpp`
- `src/library.properties`
- Old `IMU`, `Light`, and `ToF` sketches that included `Loka.h`

## Replacement

Use the standalone examples in `examples/`:

1. `01_CodeCell_Sensors` — official CodeCell sensor and power API
2. `02_CodeCell_Motors` — official CodeCell GPIO/PWM API with the external DRV8833
3. `03_SparkFun_ToF` — official SparkFun VL53L5CX Arduino library

Install dependencies through Arduino Library Manager rather than copying or maintaining embedded wrappers.

## Historical sources

Some vendored implementation files from the former wrapper remain under `src/mcu` and `src/tof` only as historical material. They are not referenced by the examples, and the Arduino library metadata and public wrapper entry points have been removed.
