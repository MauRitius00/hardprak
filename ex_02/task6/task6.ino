
// ------------------------------------------------------------
//  Task 6:
//      Use your code from Task 4 and Task 5, and make necessary changes. 
//      Implement the function playMelody() according to the exercise sheet.
//      The array is already filled with the right frequency for each note.
// ------------------------------------------------------------
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
uint16_t durations[10] = {620, 2940, 3030, 1049, 3092, 2040, 2094, 2023, 3807, 1509};
uint16_t notes[10]     = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659};
volatile uint32_t tCount = 0;
volatile uint8_t melodyIdx = 0;

volatile bool currentToggleState = false; // for ISR to generate Frequence
bool buzzerRunning = false; // to work with button task 3

void setP029(bool high);
void setTimer2(bool enable);

void setTimer1Freq(uint32_t freq);
void setBuzzerFreq(uint32_t freq);
void playMelody();


void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Initializing Timer 2 (1ms interval)...");
  setTimer2(true);

  // initialize buzzer (Pin 0.29 as Output)
  NRF_P0->DIRSET = (1UL << 29);
  NRF_P0->OUTCLR = (1UL << 29);

  setTimer1Freq(1046);

  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->TASKS_CLEAR = 1;

  playMelody();
}


void loop() {

}

extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[0]) {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;

    tCount++;
    if ((tCount % 1000) == (0)) { 
      Serial.println(tCount);
    }

    if (tCount >= durations[melodyIdx]) {
      tCount = 0;
      melodyIdx++;

      if (melodyIdx >= 10) {
        setBuzzerFreq(0);   // Buzzer aus
        setTimer2(false);   // Melodie-Timer aus
      } else {
        setBuzzerFreq(notes[melodyIdx]);
      }
    }
  }
}

extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]) {
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;

    currentToggleState = !currentToggleState;
    setP029(currentToggleState);
  }
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

void setP029(bool high) {
  if (high) {
    NRF_P0->OUTSET = (1UL << 29);
  } else {
    NRF_P0->OUTCLR = (1UL << 29);
  }
}

void setBuzzerFreq(uint32_t freq) {

  if ((100 <= freq) && (freq <= 3000)){
    NRF_TIMER1->CC[0] = 1000000UL / (2UL * freq);
    NVIC_ClearPendingIRQ(TIMER1_IRQn);
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->TASKS_START = 1;
  }
  else {
    setP029(false);
    NRF_TIMER1->TASKS_STOP = 1;
  }
}
  

void playMelody() {
  melodyIdx = 0;
  setBuzzerFreq(notes[melodyIdx]);
  setTimer2(true);

}