#include "NANOSAM4.h"
#include "SPI.h"
// GLOBAL VARIABLES
bool collect = false;                   // is NanoSAM in collect mode ?
int sec = 0;                            // how many seconds of collect mode true ?
int maxTime = 60*16;                    // maximum seconds allowed in collect mode

// Mode of Operation
bool mode1 = false;                     // "orbital model mode"
bool mode2 = false;                     // "manual mode"
bool mode3 = false;                     // "threshold irradiance mode"
bool mode4 = false;                     // somewhat tested
short mode4time = 0;                    // time input in seconds

// Irradiance Threshold Parameters
uint16_t threshold = 330;          // .0063 volts (dark current)
uint16_t thresholdCount = 0;            // # samples below threshold

// Sun Condition
bool sunsetCondition = false;
bool sunriseCondition = false;

// PINS
const int PIN_ADC_CS = 10;              // ADC Chip Select Pin
const int PIN_AREG_CURR = 17;           // analog regulator current pin
const int PIN_DREG_CURR = 18;           // digital regulator current pin
const int PIN_DIGITAL_THERM = 14;       // digital board thermistor pin
const int PIN_ANALOG_THERM = 15;        // analog board thermistor pin
const int PIN_OPTICS_THERM = 16;        // optics bench thermistor pin
const int ADC_MAX_SPEED = 2000000;      // 2 MHz

/* Get String and Convert CMD to INT */
int getMessageFromSerial() {
  Serial.flush();
  if (!Serial.available()){
    return 0;
  }
  int count = 0;
  int n = 3;
  char buf[7]; 
  while(count < n){
    if (Serial.available()){
      buf[count++] = Serial.read();
      if (buf[2]=='4'){
        n = 6;
      }
    }
  }
  if (buf[2]=='4'){
    char buffer[4];  
    for (int i = 0; i < 3; i++){
      buffer[i] = buf[i+3];
    }
    buffer[3] = '\0';  // Add null terminator to buffer
    mode4time = atoi(buffer);
    return 4;
  }
  else if (buf[1]=='S'){ // Check for Stop command
    return 5;
  }
  else if (buf[0]=='@' && buf[2]>='1' && buf[2]<='3'){ // Check for "M1", "M2", "M3" commands
    return buf[2] - '0';
  }
  
  return 0; // Invalid command
}
/* Change Global Variables */
void commandHandling(){
  // Mode 3 Collection Stop
  mode3Check();
  // Mode 4 Collection Stop
  mode4Check();
  // Check for and Return Serial Command
  int returnMessage = getMessageFromSerial();
  if(returnMessage==0){
    return;
  }
  // Start Collection Mode 1
  if(returnMessage==1){
    sec = 0;
    collect = 1;
    mode1 = 1;
    return;
  }
  // End Collection Mode 1/Mode 2/Mode 4
  if(returnMessage==5){
    collect = 0;
    mode1 = 0;
    mode2 = 0;
    mode4 = 0;
    bool tmp = sunsetCondition;
    sunsetCondition = sunriseCondition;
    sunriseCondition = tmp;
    return;
  }
  // Start Collection Mode 2
  if(returnMessage==2){
    sec = 0;
    mode2 = 1;
    collect = 1;
    return;
  }
  // Start Collection Mode 3
  if(returnMessage==3){
    sec = 0;    
    collect = 1;
    mode3 = 1;
    return;
  }
  // Start Collection Mode 4
  if(returnMessage==4){
    sec = 0;    
    collect = 1;
    mode4 = 1;
    return;
  }
  return;
}
/* Mode 4 Stop Check */
void mode4Check(){
  if (!mode4)
    return;
  if (sec >= mode4time){
    collect = 0;
    mode4 = 0;
    //mode4count = 0;
    mode4time = 0;
  }
  return;
}
/* Mode 3 Stop Check */
void mode3Check(){
  if (!mode3)
    return;
  if(thresholdCount >= 100){
    collect = 0;
    mode3 = 0;
  }
}
/* Compare to BIN Threshold Value */
byte thresholdCheck(uint16_t data){
  if (data >= threshold){
    thresholdCount = 0; // reset "below threshold" flag
    return 97; // "above threshold" flag
  }
  if (data < threshold){
    thresholdCount++; // increase "below threshold" count
    return 89; // "below threshold" flag
  }
  return 0; 
}
/* Check Sun Condition Global Variables */
byte sunsetSunrise(){
  if (sunsetCondition){ 
    return 115; // char s - set
  }
  if (sunriseCondition){
    return 114; // char r - rise
  }
  return 0;
}
/* 1 Sample SPI Configuration */
uint16_t scienceData(){
  uint16_t photodiode16; // 16 bit variable to hold bin number from ADC
  SPI.beginTransaction(SPISettings(ADC_MAX_SPEED, MSBFIRST, SPI_MODE3)); //SPISettings(maxSpeed,dataOrder,dataMode)
  digitalWrite(PIN_ADC_CS, LOW);   // set Slave Select pin to low to select chip
  photodiode16 = SPI.transfer16(0x0000);// transfer data, send 0 to slave, recieve data from ADC
  digitalWrite(PIN_ADC_CS, HIGH);  // set Slave Select pin to high to de-select chip
  SPI.endTransaction();
  return photodiode16;
}
/* Collect, Store, and Send 20 Byte Data Buffer */
void dataCollection(){
  byte buffer[20];
  // Start and End Byte 
  buffer[0] = 65; // char "A"
  buffer[19] = 90; // char "Z"
  // Time Data (ms)
  uint32_t timeData = millis(); // 32 bit overflow at 1193 hours
  buffer[1] = (timeData & 0xFF);  
  buffer[2] = ((timeData >> 8) & 0xFF); 
  buffer[3] = (timeData >> 16) & 0xFF;
  buffer[4] = ((timeData >> 24) & 0xFF);
  // Photodiode Data
  uint16_t photodiode16 = scienceData(); // 52428 = 1 V
  //Serial.println(photodiode16*1.25/(pow(2,16)));
  buffer[5] = (photodiode16 & 0xFF);
  buffer[6] = (photodiode16 >> 8) & 0xFF;
  // Threshold Flag
  buffer[7] = thresholdCheck(photodiode16);
  // Sun Flag
  buffer[8] = sunsetSunrise();
  // Thermistors
  uint16_t photodiodeTherm = analogRead(16);
  buffer[9] = (photodiodeTherm & 0xFF);
  buffer[10] = (photodiodeTherm >> 8) & 0xFF;
  uint16_t analogTherm = analogRead(15);
  buffer[11] = (analogTherm & 0xFF);
  buffer[12] = (analogTherm >> 8) & 0xFF;
  uint16_t digitalTherm = analogRead(14);
  buffer[13] = (digitalTherm & 0xFF);
  buffer[14] = (digitalTherm >> 8) & 0xFF;
  // Current Monitors from Regulators
  uint16_t analogCurrent = analogRead(17);
  buffer[15] = (analogCurrent & 0xFF);
  buffer[16] = (analogCurrent >> 8) & 0xFF;
  uint16_t digitalCurrent = analogRead(18);
  buffer[17] = (digitalCurrent & 0xFF);
  buffer[18] = (digitalCurrent >> 8) & 0xFF;
  // Write BUFFER to Serial
  Serial.write(buffer,20);
  return;
}
