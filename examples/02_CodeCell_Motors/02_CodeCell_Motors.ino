/*
  LokaBot - DRV8833 dual motor test

  Uses the official CodeCell Arduino library by Microbots.
  The four constants below match the LokaBot wiring documentation.

  Raise the robot so the wheels are clear before the first test.
*/

#include <CodeCell.h>

CodeCell myCodeCell;

// DRV8833 inputs connected to the four available CodeCell GPIO pins.
constexpr uint8_t LEFT_IN1  = 0;
constexpr uint8_t LEFT_IN2  = 1;
constexpr uint8_t RIGHT_IN1 = 2;
constexpr uint8_t RIGHT_IN2 = 3;

constexpr uint16_t PWM_FREQUENCY_HZ = 20000;

void setMotor(uint8_t in1, uint8_t in2, int speedPercent) {
  speedPercent = constrain(speedPercent, -100, 100);
  const uint8_t duty = abs(speedPercent);

  if (speedPercent > 0) {
    myCodeCell.pinPWM(in1, PWM_FREQUENCY_HZ, 0);
    myCodeCell.pinPWM(in2, PWM_FREQUENCY_HZ, duty);
  } else if (speedPercent < 0) {
    myCodeCell.pinPWM(in2, PWM_FREQUENCY_HZ, 0);
    myCodeCell.pinPWM(in1, PWM_FREQUENCY_HZ, duty);
  } else {
    myCodeCell.pinPWM(in1, PWM_FREQUENCY_HZ, 0);
    myCodeCell.pinPWM(in2, PWM_FREQUENCY_HZ, 0);
  }
}

void drive(int leftPercent, int rightPercent) {
  setMotor(LEFT_IN1, LEFT_IN2, leftPercent);
  setMotor(RIGHT_IN1, RIGHT_IN2, rightPercent);
}

void stopMotors() {
  drive(0, 0);
}

void runStep(const char *label, int left, int right, uint32_t durationMs) {
  Serial.println(label);
  drive(left, right);
  delay(durationMs);
  stopMotors();
  delay(500);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // No onboard sensor is required, but Init keeps CodeCell power management active.
  myCodeCell.Init();
  stopMotors();

  Serial.println("LokaBot motor test begins in 3 seconds");
  delay(3000);
}

void loop() {
  // Keep CodeCell power and battery management updated.
  myCodeCell.Run(20);

  runStep("Forward", 35, 35, 1500);
  runStep("Reverse", -35, -35, 1500);
  runStep("Turn left", -30, 30, 1000);
  runStep("Turn right", 30, -30, 1000);

  Serial.println("Motor test complete. Restart the board to run again.");
  stopMotors();
  while (true) {
    myCodeCell.Run(10);
    delay(10);
  }
}
