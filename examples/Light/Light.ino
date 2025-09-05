/*
  Loka Example — Light (Guide)
  -----------------------------------------
  Project: Loka Robot
  Author : Fahad Al Ajmi
  GitHub : https://github.com/faajmid/Loka
  License: MIT

  Overview
  This example shows how to read ambient light and proximity from the VCNL4040,
  optionally drive an LED when it's dark, and print exactly what you requested.

  Features demonstrated
  • Enable Light sensor VCNL4040
  • Automatic headlight helper (threshold on ambient)
  • Orderless, subset-friendly reads for Light
  • Print lines that mirror your read choices (and order)

  API summary
    Init(LIGHT);                         // enable VCNL4040
    SetDarkLED(pin, thresh, activeHigh, AMB);
                                         // drive LED when ambient < thresh
    Run(hz);                             // poll/cache Light at 'hz'

    // Light (VCNL4040) — flexible & orderless
    Light();                             // both: amb, prox (order amb,prox)
    Light(amb);                          // or Light(prox) / Light(prox, amb)

    // Printer (reflects exactly what you read this tick, in your order)
    PrintLight();                        // "Light: amb=… [, prox=…]"
*/

#include <Loka.h>

LokaMCU loka;

void setup() {
  Serial.begin(115200);
  loka.Init(LIGHT);

  // Example: LED on pin 1 when ambient < 250 (active-high), using AMB source
  loka.SetDarkLED(1, 250, true, AMB);
}

void loop() {
  loka.Run(20);                         // ~20 Hz is fine for Light

  // Choose what you want (any order)
  loka.Light(amb);                      // just ambient
  // loka.Light(prox);                  // or just proximity
  // loka.Light(prox, amb);             // or both, any order

  // Prints exactly what you read, same order:
  loka.PrintLight();                    // e.g. "Light: amb=…"
}
