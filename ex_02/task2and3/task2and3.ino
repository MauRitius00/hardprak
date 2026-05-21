#include <Arduino.h>

// ------------------------------------------------------------
// Task 2 and 3:
// ------------------------------------------------------------

volatile bool currentToggleState = false; // for ISR to generate Frequence
bool buzzerRunning = false; // to work with button task 3

void setTimer1Freq();
void setBuzzerFreq();
void setP029(bool high);

void setup() {

  // initialize buzzer (Pin 0.29 as Output)
  NRF_P0->DIRSET = (1UL << 29);
  NRF_P0->OUTCLR = (1UL << 29);

  // initialize Button as aktive low

  // set correct binary value in register for button
  // 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0
  NRF_P0->PIN_CNF[3] = 12;

  setTimer1Freq();

  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->TASKS_CLEAR = 1;
}

void loop() {
  setBuzzerFreq();
}

void setTimer1Freq() {
  // Timer setup acording to task 2

  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->TASKS_CLEAR = 1;


  NRF_TIMER1->MODE = 0;
  NRF_TIMER1->BITMODE = 3;
  NRF_TIMER1->PRESCALER = 4; // 1 MHz

  NRF_TIMER1->CC[0] = 1000000UL / (2UL * 1046UL);

  NRF_TIMER1->SHORTS = 1;

  NRF_TIMER1->EVENTS_COMPARE[0] = 0;

  NRF_TIMER1->INTENSET = (1UL << 16);


  NVIC_ClearPendingIRQ(TIMER1_IRQn);
  NVIC_EnableIRQ(TIMER1_IRQn);
}

void setP029(bool high) {
  if (high) {
    NRF_P0->OUTSET = (1UL << 29);
  } else {
    NRF_P0->OUTCLR = (1UL << 29);
  }
}

void setBuzzerFreq() {
  // True if button pressed
  bool buttonPressed = ((NRF_P0->IN & (1UL << 3)) == 0);

  if (buttonPressed && !buzzerRunning) {
    currentToggleState = false;
    setP029(false);

    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    NVIC_ClearPendingIRQ(TIMER1_IRQn);

    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->TASKS_START = 1;

    buzzerRunning = true;
  }

  if (!buttonPressed && buzzerRunning) {
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;

    setP029(false);
    currentToggleState = false;

    buzzerRunning = false;
  }
}

extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;

    currentToggleState = !currentToggleState;
    setP029(currentToggleState);
  }
}