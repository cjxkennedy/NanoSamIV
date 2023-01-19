/* - - - - - - NanoSAM-IV WIP Testing Script - - - - - - */
/* GLOBAL VARIABLES */
bool collect = false;       // is NanoSAM in collect mode ?
int sec = 0;                // how many seconds of collect mode ?
int maxTime = 30;           // maximum seconds allowed in collect mode
/* Mode of Operation */
bool mode1 = 0;             // "orbital model mode"
bool mode2 = 0;             // "manual mode"
bool mode3 = 0;             // 
bool mode4 = 0;             // not implemented
/* Irradiance Threshold Parameters */
uint16_t threshold = (2^16)/8; // dummy value
uint16_t thresholdCount;       // # samples below threshold
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
/* SPI and Serial Setup */
void setup() {
  Serial.begin(9600); // dummy baud rate
  //SPI.begin();  // SPI startup
  //pinMode(PIN_ADC_CS, OUTPUT); // set ADC chip select pin to output
  randomSeed(analogRead(0));
  sunsetCondition = 1; // initial sun condition
  sunriseCondition = 0;
  Serial.println("Setup Complete");
}
/* Get 3 Byte String and Convert CMD to INT */
int getMessageFromSerial() { // input MUST be a 3 digit string !
  Serial.flush();
  if (!Serial.available()){
    return 0;
  }
  int count = 0;
  char buf[3]; //buffer size 
  while(count < 3){
      if (Serial.available()){
          buf[count++] = Serial.read();
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
  if (buf[1]=='S'){  // valid mode 1/mode 2 stop
    //Serial.println();
    //Serial.println("Stop Command (S) Inputted");
    return 4;
  }
  return 0;
}
/* Change Global Variables */
void commandHandling(){
  // Mode 3 Collection Stop
  if(mode3 & (thresholdCount >= 100))
    collect = 0;
    mode3 = 0;
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
  // End Collection Mode 1/Mode 2
  if(returnMessage==4){
    collect = 0;
    mode1 = 0;
    mode2 = 0;
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
  return;
}
/* Compare to BIN Threshold Value */
byte thresholdCheck(uint16_t data){
  if (data >= threshold){
    if (thresholdReset)
      thresholdCount=0;     
    return 97; // char "g"
  }
  //if (data == threshold){
  //  return 101; // char "e"
  //}
  if (data < threshold){
    thresholdChecking = 1;
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
  /* SPI 
  // access ADC Pin (SPI)
  uint16_t photodiode16; // 16 bit variable to hold bin number from ADC
  SPI.beginTransaction(SPISettings(ADC_MAX_SPEED, MSBFIRST, SPI_MODE3)); //SPISettings(maxSpeed,dataOrder,dataMode)
  digitalWrite(PIN_ADC_CS, LOW);   // set Slave Select pin to low to select chip
  photodiode16 = SPI.transfer16(0x0000);// transfer data, send 0 to slave, recieve data from ADC
  digitalWrite(PIN_ADC_CS, HIGH);  // set Slave Select pin to high to de-select chip
  SPI.endTransaction();
  return photodiode16;
  */
  return 30000;
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
    while(count < 50){ // this loop runs for a second !
      dataCollection();      
      count++;
      delay(19);
      delayMicroseconds(880);
    }
    sec++; // count of number of "1 second" periods 
  }
}