/* - - - - - - NanoSAM-IV Main FSW Script - - - - - - */
// Author: CJ Kennedy 
// Hardware: Teensy 4.0

// NanoSam-IV operates in four different modes of operation
//
// Mode 1: Orbital Simulation Start/Stop Control
// Mode 2: Manual Start/Stop Control
// Mode 3: Below Irradiance Threshold Stop
// Mode 4: Timed Collection Period

// Inputs to Teensy are command strings for all modes:
// To start: @M1, @M2, @M3, @M4, and @SS to stop collection

// Outputs from Teensy are sent as a buffer of bytes (binary)
// This is 20 bytes at 50 Hz and is controlled by delay() in loop()


/* Included Libraries */
#include <SPI.h>
#include "NANOSAM4.h"

/* SPI and Serial Setup */
void setup() {
  Serial.begin(9600);  // dummy baud rate
  SPI.begin();         // SPI startup
  pinMode(PIN_ADC_CS, OUTPUT); // set ADC chip select pin to output
  sunsetCondition = 1; // initial sun condition
  sunriseCondition = 0;
  Serial.println("Setup Complete");
}

void loop() {
  // Command Handling
  commandHandling();
  if(!collect){
    delay(100);
  }
  // Data Collection Mode 1,2,3,4
  if(collect & (sec < maxTime)){
    int count = 0;
    // this loop runs for a second 
    while(count < 50){ 
      dataCollection();
      count++;
      delay(19);
      delayMicroseconds(880);
    }
    // count of # of "1 second" periods
    sec++;
   }
}
