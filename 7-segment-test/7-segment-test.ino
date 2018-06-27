#define SCLK 2
#define RCLK 3
#define DIO 4

// 4 digit 7-segment 'framebuffer'
volatile byte display[4] = {0};

// Mask into a digit pattern to add the decimal point
byte decimalMask = 0b10000000;

// Digit patterns for 0-9
byte digitPatterns[10] = {
  0b00111111,
  0b00000110,
  0b01011011,
  0b01001111,
  0b01100110,
  0b01101101,
  0b01111101,
  0b00000111,
  0b01111111,
  0b01101111,
};

void setup() {
  pinMode(SCLK, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(DIO, OUTPUT);

  // Setup timer
  cli(); // Stop interrupts

  // Setup timer 1 at 64x time
  TCCR1B = 0b00000011;
  TIMSK1 = 0b00000001;

  sei(); // Enable interrupts

  display[0] = digitPatterns[3] | decimalMask;
  display[1] = digitPatterns[1];
  display[2] = digitPatterns[4];
  display[3] = digitPatterns[1];
}

ISR(TIMER1_OVF_vect) {
  static byte currentDigit = 0;

  digitalWrite(RCLK, LOW);

  byte digitBitPattern = ~display[currentDigit];
  byte digitSelect = 1 << (3 - currentDigit);

  shiftOut(DIO, SCLK, MSBFIRST, digitBitPattern);
  shiftOut(DIO, SCLK, MSBFIRST, digitSelect);

  digitalWrite(RCLK, HIGH);

  currentDigit = (currentDigit + 1) % 4;
}

int digitIndex = 0;

void loop() {
  delay(1000);
}

