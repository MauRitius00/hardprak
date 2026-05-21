#include <Adafruit_TinyUSB.h>
#define BLINK_INTERVAL_MS  1000UL

// ------------------------------------------------------------
//  Task 1:
//      Write the body of setP026() as specified in the exercise sheet.
//      To toggle at the right frequency, you can use
//      this function in a loop.
// ------------------------------------------------------------

unsigned long last_blink_ms = 0;
bool input = 0;

void setup() {

  last_blink_ms = millis();
   

  // Set Pin to Output 
  NRF_P0->DIRSET = (1UL << 26);
}


void loop() {
  // imported from ex01 to create blinking
  if (millis() - last_blink_ms >= BLINK_INTERVAL_MS) {
    last_blink_ms = millis();
    setP026(input);
    input = ! input;
  }
}


void setP026(bool high) {
  // set Pin P0.26 on 1 or 0 (high or low)
  if (high) {
    NRF_P0->OUTSET = (1UL << 26);
  }
  else {
    NRF_P0->OUTCLR = (1UL << 26);
  }

}