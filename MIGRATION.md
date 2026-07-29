# Migration from the former Loka library

The repository has moved from a custom Arduino library to a hardware-first project.

## Removed legacy software

The former custom wrappers, bundled third-party driver sources, Arduino library metadata, and broken examples have been removed completely. The repository no longer contains a `src/` library tree.

Removed items include:

- `LokaBot.h`
- `LokaMCU.h/.cpp`
- `LokaMotors.h/.cpp`
- `LokaToF.h/.cpp`
- `src/library.properties`
- Bundled BNO085/SH-2 source files
- Bundled SparkFun/ST VL53L5CX source files
- Old `IMU`, `Light`, and `ToF` sketches that included `Loka.h`

## Replacement

Use the standalone examples in `examples/`:

1. `01_CodeCell_Sensors` — official CodeCell sensor and power API
2. `02_CodeCell_Motors` — official CodeCell GPIO/PWM API with the external DRV8833
3. `03_SparkFun_ToF` — official SparkFun VL53L5CX Arduino library

Install dependencies through Arduino Library Manager. Third-party libraries are no longer copied into or maintained inside the LokaBot repository.
