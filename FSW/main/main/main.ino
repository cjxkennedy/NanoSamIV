/* - - - - - - NanoSAM-IV Main FSW Script - - - - - - */
/* Included Libraries */
#include <SPI.h>
#include <C:\Users\tomat\OneDrive\Desktop\FSW\main\NANOSAM4.h>
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