#ifndef NANOSAM4_H
#define NANOSAM4_H
#include "Arduino.h"

// GLOBAL VARIABLES
extern bool collect;                 // is NanoSAM in collect mode ?
extern int sec;                      // how many seconds of collect mode true ?
extern int maxTime;                  // maximum seconds allowed in collect mode

// Mode of Operation
extern bool mode1;                   // "orbital model mode"
extern bool mode2;                   // "manual mode"
extern bool mode3;                   // "threshold irradiance mode"
extern bool mode4;                   // somewhat tested
extern short mode4time;              // time input in seconds
extern short mode4count;             // count of samples

// Irradiance Threshold Parameters
extern uint16_t threshold;           // dummy value
extern uint16_t thresholdCount;      // # samples below threshold

// PINS
extern const int PIN_ADC_CS;         // ADC Chip Select Pin
extern const int PIN_AREG_CURR;      // analog regulator current pin
extern const int PIN_DREG_CURR;      // digital regulator current pin
extern const int PIN_DIGITAL_THERM;  // digital board thermistor pin
extern const int PIN_ANALOG_THERM;   // analog board thermistor pin
extern const int PIN_OPTICS_THERM;   // optics bench thermistor pin
extern const int ADC_MAX_SPEED;      // 2 MHz

// Function Prototypes
void setup();
void commandHandling();
void mode3Check();
void mode4Check();
int getMessageFromSerial();
byte thresholdCheck(uint16_t data);

#endif
