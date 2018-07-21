#include <SPI.h>

#define DATA_PIN 11
#define CLOCK_PIN 13
#define CS_PIN 10
#define ANALOG_PIN A0

void setup() {
  Serial.begin(9600);

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);

  digitalWrite(CS_PIN, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

  // Setup timer
  cli(); // Stop interrupts

  // Setup timer 1 at 1x time
  TCCR2A = 0;
  TCCR2B = 0b00000001;
  TIMSK2 = 0b00000001;

  sei(); // Enable interrupts

  Serial.println("Start");
}

volatile int audio = 0;
volatile uint32_t timer = 0;
volatile int freq = 40;
volatile short cycles = 0;
byte samples[] = {
  0, 255
};

#define DAC_HEADER (0b0011 << 12)

ISR(TIMER2_OVF_vect) {
  // short tStart = TCNT1;
  timer++;
  if (timer & 1) return;
  
  int scaler = freq;
  
  audio = (audio + 1) % scaler;
  short data = (1 << 11) + (1 << 10)
                * (audio / (scaler / 2) == 0 ? 1 : -1 );

  // message |= 0b111111111111 & data;

  // Enable the DAC
  // digitalWrite(CS_PIN, LOW);
  PORTB &= ~0b100; // Optimization

  // Send the message
  SPI.transfer16(DAC_HEADER | data);

  // Disable the DAC
  // digitalWrite(CS_PIN, HIGH);
  PORTB |= 0b100;
  
  // cycles = TCNT1 - tStart;
}

void loop() {
  static int time = 0;

  int now = millis();
  if (now - time > 1000) {
    time = now;
    Serial.println(timer);
    Serial.println(freq);
    timer = 0;
  }

  freq = map(analogRead(A0), 0, 1024, 200, 0);

  // Serial.print("Cycles: ");
  // Serial.println(cycles);
}

