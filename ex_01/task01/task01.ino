#include <Adafruit_TinyUSB.h>
#include <U8g2lib.h>
#include <Wire.h>
#define BLINK_INTERVAL_MS  1000UL  // 1 second on, 1 second off → 1 Hz

// ------------------------------------------------------------
//  Global variables for Task 1 ii) and beyond
//  Use the smallest type that fits:
//    unsigned long — required for millis() values (32-bit)
//    uint8_t       — sufficient for a cycle/colour counter (8-bit, max 255)
// ------------------------------------------------------------
unsigned long last_blink_ms = 0;  // timestamp of the last LED toggle
bool          led_on        = false; // current LED state

// TODO (Bonus): add a counter for completed blink cycles
//               and a variable to track the current colour


void setup() {
  Serial.begin(115200);
  while (!Serial);
  // Set LED pins as outputs.
  pinMode(LED_GREEN, OUTPUT);
  // configures a pin for digital output
  // Start with all LEDs off (active-low → HIGH = off)
}


void loop() {

  // ----------------------------------------------------------
  //  Task 1 i) — Blink using delay()
  //  Comment this section out once you move on to Task 1 ii).
  // ----------------------------------------------------------

  // TODO: turn LED_RED on,
  //       wait BLINK_INTERVAL_MS ms,
  //       turn LED_RED off,
  //       wait BLINK_INTERVAL_MS ms.

  digitalWrite(LED_GREEN, LOW);
  delay(BLINK_INTERVAL_MS);
  digitalWrite(LED_GREEN, HIGH);
  delay(BLINK_INTERVAL_MS);
  // ----------------------------------------------------------
  //  Task 1 ii) — Blink using millis()  (comment out i) first)
  //
  // TODO: get the current time with millis().
  // TODO: check whether BLINK_INTERVAL_MS has elapsed since last_blink_ms.
  // TODO: if yes — update last_blink_ms, toggle led_on,
  //               and write the correct HIGH/LOW to LED_RED.


  // ----------------------------------------------------------
  //  Task 1 iii) — Answer as a comment
  // ----------------------------------------------------------

  // ----------------------------------------------------------
  //  Bonus — Colour cycling after 10 blink cycles
  // ----------------------------------------------------------
  // TODO: count completed blink cycles (one cycle = on + off).
  //       Every 10 cycles, switch to the next colour:
  //         red → green → blue → red → ...
  //       Turn off all LEDs before switching, then turn on only
  //       the new active colour.
}