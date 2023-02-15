/* - - - - - - NanoSAM-IV Main FSW WIP Script - - - - - - */
/* Included Libraries */
#include <SPI.h>
/* GLOBAL VARIABLES */
bool collect = false;       // is NanoSAM in collect mode ?
int sec = 0;                // how many seconds of collect mode ?
int maxTime = 30;           // maximum seconds allowed in collect mode
/* Mode of Operation */
bool mode1 = 0;             // "orbital model mode"
bool mode2 = 0;             // "manual mode"
bool mode3 = 0;             // "threshold irradiance mode"
bool mode4 = 0;             // not tested yet ! "timed mode"
short mode4time = 0;        // time input in seconds
short mode4count = 0;       // count of samples
/* Irradiance Threshold Parameters */
uint16_t threshold = (2^16)/4;      // dummy value
uint16_t thresholdCount;            // # samples below threshold
/* Sun Conditions */
bool sunsetCondition;
bool sunriseCondition;
/* PINS */
const int PIN_ADC_CS = 10;          // ADC Chip Select Pin
const int PIN_AREG_CURR = 17;       // analog regulator current pin
const int PIN_DREG_CURR = 18;       // digital regulator current pin
const int PIN_DIGITAL_THERM = 14;   // digital board thermistor pin
const int PIN_ANALOG_THERM = 15;    // analog board thermistor pin
const int PIN_OPTICS_THERM = 16;    // optics bench thermistor pin
const int ADC_MAX_SPEED = 2000000;
/* SPI and Serial Setup */
void setup() {
  Serial.begin(9600);  // dummy baud rate
  SPI.begin();         // SPI startup
  pinMode(PIN_ADC_CS, OUTPUT); // set ADC chip select pin to output
  sunsetCondition = 1; // initial sun condition
  sunriseCondition = 0;
  Serial.println("Setup Complete");
}
/* Get String and Convert CMD to INT */
int getMessageFromSerial() {
  // input MUST be a 3 digit string !
  // unless command starts with @M4...
  Serial.flush();
  if (!Serial.available()){
    return 0;
  }
  int count = 0;
  int n = 3;
  char buf[n]; //buffer size 
  while(count < n){
      if (Serial.available()){
          buf[count++] = Serial.read();
          if (count==3 & buf[2]==4){
            n = 6;
          }
      }
  }
  // Convert string command to int //
  if (buf[0]!='@') { // vot a valid message
    //Serial.println();
    //Serial.println("Not Valid");
    return 0;
  }
  if (buf[2]=='1') { // valid mode 1 start
    //Serial.println();
    //Serial.println("Start Command (M1) Inputted");
    return 1;
  }
  if (buf[2]=='2') { // valid mode 2 start
    //Serial.println();
    //Serial.println("Start Command (M2) Inputted");    
    return 2;
  }
  if (buf[2]=='3'){ // valid mode 3 start
    //Serial.println();
    //Serial.println("Start Command (M3) Inputted");
    return 3;
  }
  if (buf[2]=='4'){
    //Serial.println();
    //Serial.println("Start Command (M4) Inputted");
    for (int i = 0; i < 2; i++){
      mode4time[i] = (short) buf[i+2];
    }
    return 4;
  }
  if (buf[1]=='S'){  // valid mode 1/2/4 stop
    //Serial.println();
    //Serial.println("Stop Command (S) Inputted");
    return 5;
  }
  return 0;
}
/* Change Global Variables */
void commandHandling(){
  // Mode 3 Collection Stop
  if(mode3 & (thresholdCount >= 100)){
    collect = 0;
    mode3 = 0;
  }
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
  if(returnMessage==3){
    sec = 0;    
    collect = 1;
    mode4 = 1;
    return;
  }
  return;
}
/* Mode 4 Time Check */
void mode4Check(){
  if (!mode4)
    return;
  mode4count++;
  if (mode4count/50 >= mode4time){
    collect = 0;
    mode4count = 0;
    mode4time = 0;
  }
  return;
}
/* Compare to BIN Threshold Value */
byte thresholdCheck(uint16_t data){
  if (data >= threshold){
    thresholdCount = 0;     
    return 97; // char "g"
  }
  if (data < threshold){
    thresholdCount++; 
    return 108; // char "l"
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
  // access ADC Pin (SPI)
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
  uint16_t photodiode16 = scienceData();
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
void loop() {
  // Command Handling
  commandHandling();
  // 10 Hz Command Handling 
  if(!collect){
    delay(100);
  }
  // Data Collection Mode 1,2,3
  // Check for Max Time
  if(collect & (sec < maxTime)){
    int count = 0;
    // this loop runs for a second !
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
