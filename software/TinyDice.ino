// ===================================================================================
// Project:   TinyDice - Electronic Dice based on ATtiny13A
// Version:   v1.0
// Year:      2020
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// A tiny electronic dice powered by ATtiny13A. Timer0 is used to
// constantly change the number of pips. Chance is created by the
// uncertainty of the moment the button is pressed by the user.
//
// Wiring:
// -------
//                           +-\/-+
//        --- RST ADC0 PB5  1|°   |8  Vcc
// LED2/5 ------- ADC3 PB3  2|    |7  PB2 ADC1 -------- LED1/4
// Button ------- ADC2 PB4  3|    |6  PB1 AIN1 OC0B --- LED3/6
//                     GND  4|    |5  PB0 AIN0 OC0A --- LED7
//                           +----+
//         Dice
// +------------------+
// | LED1        LED6 |
// |                  |
// | LED2  LED7  LED5 |
// |                  |
// | LED3        LED4 |
// +------------------+
//
// Compilation Settings:
// ---------------------
// Controller:  ATtiny13A
// Core:        MicroCore (https://github.com/MCUdude/MicroCore)
// Clockspeed:  1.2 MHz internal
// BOD:         BOD disabled
// Timing:      Micros disabled
//
// Leave the rest on default settings. Don't forget to "Burn bootloader"!
// No Arduino core functions or libraries are used. Use the makefile if 
// you want to compile without Arduino IDE.
//
// Fuse settings: -U lfuse:w:0x2a:m -U hfuse:w:0xff:m


// ===================================================================================
// Libraries and Definitions
// ===================================================================================

// Libraries
#include <avr/io.h>               // for GPIO
#include <avr/sleep.h>            // for sleep mode
#include <avr/interrupt.h>        // for interrupts
#include <util/delay.h>           // for delays

// Global variables
volatile uint8_t pips = 0;        // current number of pips

// ===================================================================================
// Main Function
// ===================================================================================

int main(void) {
  // Local variables
  uint8_t matrix[] = {0b00110001, // 1
                      0b00110100, // 2
                      0b00110011, // 3
                      0b00110110, // 4
                      0b00110111, // 5
                      0b00111110};// 6 - for converting pips to pins

  // Setup pins
  DDRB   = 0b00001111;            // PB0 - PB3 as output, PB4 input
  PORTB  = 0b00110001;            // pull-up for PB4/5; LED7 on

  // Setup timer/counter
  TCCR0A = 0b00000000;            // no output
  TCCR0B = 0b00000011;            // set prescaler to 64
  TIMSK0 = 0b00000010;            // enable timer overflow interrupt

  // Setup pin change interrupt
  GIMSK  = 0b00100000;            // turn on pin change interrupts
  PCMSK  = 0b00010000;            // pin change interrupt on button pin
  SREG  |= 0b10000000;            // enable global interrupts

  // Disable unused peripherals and set sleep mode to save power
  ACSR   = 0b10000000;            // disable analog comperator
  PRR    = 0b00000001;            // shut down ADC
  set_sleep_mode(SLEEP_MODE_IDLE);// set sleep mode to IDLE

  // Loop
  while(1) {
    sleep_mode();                         // go to sleep
    if(~PINB & 0b00010000) {              // if button pressed:  
      for(uint8_t i = 0; i < 16; i++) {   // roll the dice
        uint8_t del = (i << 4);           // increasing delay between pip-shows
        while(del--) _delay_ms(1);        // set the delay
        PORTB = matrix[pips];             // show current number of pips
      }
      while(~PINB & 0b00010000);          // wait for button released
      _delay_ms(10);                      // debounce
    }
  }
}

// ===================================================================================
// Interrupt Service Routines
// ===================================================================================

// Timer0 overflow interrupt service routine
ISR(TIM0_OVF_vect) {
  if(++pips > 5) pips = 0;        // cycle number of pips on every timer overflow
}

// Pin change interrupt service routine
EMPTY_INTERRUPT(PCINT0_vect);     // nothing to be done here, just wake up from sleep
