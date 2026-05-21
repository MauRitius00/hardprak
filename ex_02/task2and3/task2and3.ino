#include <Adafruit_TinyUSB.h>
#define FREQUENCY  1046UL

// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#include <Arduino.h>

bool currenttogglestate = 0;
void setup() {
  // Set Pin P0.29 to Output
  NRF_P0->DIRSET = (1UL << 29);
  NRF_TIMER1->PRESCALER = 4;
  NRF_TIMER1->BITMODE = 3;
  NRF_TIMER1->MODE = 0;
  

  setTimer1Freq();
  NRF_TIMER1->TASKS_START = 1;
}


void loop() {

}



void setTimer1Freq() {
  // read from timer1 and interrupt
  NRF_TIMER1->CC[0] = 1000000 / (2 * 1046);
  NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Msk;
  NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;

  // NVIC_ClearPendingIRQ(TIMER1_IRQn);
  NVIC_EnableIRQ(TIMER1_IRQn);
  
}


void setP029(bool high) {
  // set Pin P0.29 on 1 or 0 (high or low)
  if (high) {
    NRF_P0->OUTSET = (1UL << 29);
  }
  else {
    NRF_P0->OUTCLR = (1UL << 29);
  }

}

void setBuzzerFreq() {

}


extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  
    setP029(currenttogglestate);
    currenttogglestate = ! currenttogglestate;
  }
  

}



