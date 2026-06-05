
// ------------------------------------------------------------
//  Task 4:
//      Make necessary changes to your code from Task 3. Provided tests
//      do not cover all cases.
//      We already worked pretty good in task 1-3, so this task was easy
//      to fullfill. We used the functions from the previous tasks.
//      We hat issues with measuring the frequency with our smartphone.
// ------------------------------------------------------------
#include <Arduino.h>

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

  setTimer1Freq(1046);

  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->TASKS_CLEAR = 1;
}


void loop() {
// tests
  setBuzzerFreq(440);  //A4
  delay(300);
  setBuzzerFreq(99);  // additional
  delay(300);
  setBuzzerFreq(3001);  // additional
  delay(300);
  setBuzzerFreq(100000);  // additional
  delay(300);
  setBuzzerFreq(100); // additional
  delay(300);
  setBuzzerFreq(440);  //A4
  delay(300);
  setBuzzerFreq(554);  //C#5
  delay(300);
  setBuzzerFreq(659);  //E5
  delay(300);
  setBuzzerFreq(880);  //A5
  delay(500);
  setBuzzerFreq(5000); //out of range
  delay(3000);
  for (int f = 100; f <= 3000; f += 50) {
    setBuzzerFreq(f);
    delay(20);}
}


void setTimer1Freq(uint32_t freq) {
  // Timer setup acording to task 2

  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->TASKS_CLEAR = 1;


  NRF_TIMER1->MODE = 0;
  NRF_TIMER1->BITMODE = 3;
  NRF_TIMER1->PRESCALER = 4; // 1 MHz

  NRF_TIMER1->CC[0] = 1000000UL / (2UL * freq);

  NRF_TIMER1->SHORTS = 1;

  NRF_TIMER1->EVENTS_COMPARE[0] = 0;

  NRF_TIMER1->INTENSET = (1UL << 16);


  NVIC_ClearPendingIRQ(TIMER1_IRQn);
  NVIC_EnableIRQ(TIMER1_IRQn);
  NRF_TIMER1->TASKS_START = 1;

}

void setP029(bool high) {
  if (high) {
    NRF_P0->OUTSET = (1UL << 29);
  } else {
    NRF_P0->OUTCLR = (1UL << 29);
  }
}

void setBuzzerFreq(uint32_t freq) {
  // set the frequency of the buzzer to freq
  if ((100 <= freq) && (freq <= 3000)){
    NRF_TIMER1->CC[0] = 1000000UL / (2UL * freq);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->TASKS_START = 1;
  }
  else {
    // deactivate buzzer if freq out of range
    setP029(false);
    NRF_TIMER1->TASKS_STOP = 1;
  }
}

extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;

    currentToggleState = !currentToggleState;
    setP029(currentToggleState);
  }
}