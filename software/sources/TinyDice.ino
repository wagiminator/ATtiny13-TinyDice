// tinyDice for ATtiny13A
// 
// A tiny electronic dice powered by ATtiny13. Timer0 is used to
// constantly change the number of pips. Chance is created by the
// uncertainty of the moment the button is pressed by the user.
//
//                          +-\/-+
// ---------- A0 (D5) PB5  1|    |8  Vcc
// LED2/5 --- A3 (D3) PB3  2|    |7  PB2 (D2) A1 --- LED1/4
// Button --- A2 (D4) PB4  3|    |6  PB1 (D1) ------ LED3/6
//                    GND  4|    |5  PB0 (D0) ------ LED7
//                          +----+    
//         Dice
// +------------------+
// | LED1        LED6 |
// |                  |
// | LED2  LED7  LED5 |
// |                  |
// | LED3        LED4 |
// +------------------+
//
// Clockspeed 1.2 MHz internal.
// Millis/Tone must be disabled as Timer 0 is used.
//
// 2020 by Stefan Wagner


volatile uint8_t pips = 0;        // current number of pips
uint8_t matrix[] = {0b00110001,   // 1
                    0b00110100,   // 2
                    0b00110011,   // 3
                    0b00110110,   // 4
                    0b00110111,   // 5
                    0b00111110};  // 6 - for converting pips to pins


void setup() {
  // setup pins
  DDRB  = 0b00001111;             // PB0 - PB3 as output, PB4 input
  PORTB = 0b00110001;             // pull-up for PB4/5; LED7 on

  // setup timer/counter
  TCCR0A = 0b00000000;            // no output
  TCCR0B = 0b00000011;            // set prescaler to 64
  TIMSK0 = 0b00000010;            // enable timer overflow interrupt
  SREG  |= 0b10000000;            // enable global interrupts
}

void loop() {
  while(PINB & 0b00010000);       // wait for button pressed

  // rolling the dice:
  for (uint8_t i = 0; i < 16; i++) {
    delay(i << 4);                // increasing delay between pip-shows
    PORTB = matrix[pips];         // show current number of pips
  }

  while(~PINB & 0b00010000);      // wait for button released
  delay(10);                      // debounce
}

// timer0 overflow interrupt
ISR (TIM0_OVF_vect) {
  if (++pips > 5) pips = 0;       // cycle number of pips on every timer overflow
}
