
// ------------------------------------------------------------
//  Task 5:
//      Write the body of setTimer2() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER2_IRQHandler().
// ------------------------------------------------------------


// we did the same as in task 2, but with Timer2
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
volatile uint32_t tCount = 0;
void setTimer2();


void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Initializing Timer 2 (1ms interval)...");
  setTimer2(true);
}


void loop() {

}


extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[0]) {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    if ((tCount % 1000) == (0)) {
      Serial.println(tCount);
    }
    tCount ++;
  }

}


void setTimer2(bool enable) {
  if (enable) {
    NRF_TIMER2->TASKS_STOP = 1;
    NRF_TIMER2->TASKS_CLEAR = 1;

    NRF_TIMER2->MODE = 0;
    NRF_TIMER2->BITMODE = 3;
    NRF_TIMER2->PRESCALER = 4; // 1 MHz

    NRF_TIMER2->CC[0] = 1000;
    NRF_TIMER2->SHORTS = 1;
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    NRF_TIMER2->INTENSET = (1UL << 16);

    NVIC_ClearPendingIRQ(TIMER2_IRQn);
    NVIC_EnableIRQ(TIMER2_IRQn);
    NRF_TIMER2->TASKS_START = 1;
  }
  else {
    NRF_TIMER2->TASKS_STOP = 1;
  }
}
