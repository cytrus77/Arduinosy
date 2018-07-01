#ifndef DEFINES_H
#define DEFINES_H

// PINOUT
// PIN 0 - TX serial - RS485
// PIN 1 - RX serial - RS485
const unsigned int rollerUpPin   = 2;
const unsigned int rollerDownPin = 3;
const unsigned int switchUpPin   = 4;
const unsigned int switchDownPin = 5;
const unsigned int statusLedPin  = 6;

// measurement period
const unsigned int  timer0Period = 1000;  // 1ms
const unsigned long countsIn1sec = 1000000/timer0Period;
const unsigned long countsIn1ms  = 1000/timer0Period;
const unsigned int  longButtonCommandTimer = 5;    // 5s



#endif
