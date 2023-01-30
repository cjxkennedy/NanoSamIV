/* - - - - - - NanoSAM-IV SPI Testing Script - - - - - - */
/* GLOBAL VARIABLES */
#include <SPI.h>
bool collect = false;       // is NanoSAM in collect mode ?
int sec = 0;                // how many seconds of collect mode ?
int maxTime = 30;           // maximum seconds allowed in collect mode
/* Mode of Operation */
bool mode1 = false;             // "collection mode"
/* PINS */
const int PIN_ADC_CS = 10;          // ADC Chip Select Pin
const int ADC_MAX_SPEED = 2000000;
/* SPI and Serial Setup */
void setup() {
  Serial.begin(9600); // dummy baud rate
  SPI.begin();  // SPI startup
  pinMode(PIN_ADC_CS, OUTPUT); // set ADC chip select pin to output
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
    Serial.println();
    Serial.println("Not Valid");
    return 0;
  }
  if (buf[2]=='1') { // valid mode 1 start
    Serial.println();
    Serial.println("Start Command (M#) Inputted");
    return 1;
  }
  if (buf[1]=='S'){  // valid mode 1/mode 2 stop
    Serial.println();
    Serial.println("Stop Command (S) Inputted");
    return 4;
  }
  return 0;
}
/* Change Global Variables */
void commandHandling(){
  // Check for and Return Serial Command
  int returnMessage = getMessageFromSerial();
  if(returnMessage==0){
    return;
  }
  // Start Collection Mode
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
    return;
  }
  return;
}
/* 1 Sample SPI Configuration */
uint16_t scienceData(){  
  // access ADC Pin (SPI)
  uint16_t photodiode16; // 16 bit variable to hold bin number from ADC
  SPI.beginTransaction(SPISettings(ADC_MAX_SPEED, MSBFIRST, SPI_MODE3)); // SPISettings(maxSpeed,dataOrder,dataMode)
  digitalWrite(PIN_ADC_CS, LOW);   // set Slave Select pin to low to select chip
  photodiode16 = SPI.transfer16(0x0000);// transfer data, send 0 to slave, recieve data from ADC
  digitalWrite(PIN_ADC_CS, HIGH);  // set Slave Select pin to high to de-select chip
  SPI.endTransaction();
  return photodiode16;
}
/* Collect, Store, and Send 20 Byte Data Buffer */
void dataCollection(){
  // Photodiode Data
  uint16_t photodiode16 = scienceData();
  Serial.println(photodiode16, DEC);
}

void loop() {
  // Command Handling
  commandHandling();
  if (collect){
    dataCollection();
  }
  delay(999);
  /* 50 Hz Test
  if(!collect){
    delay(100);
  }
  // Collect Data
  if(collect & (sec < maxTime)){
    int count = 0;
    while(count < 50){ // this loop runs for a second !
      dataCollection();      
      count++;
      delay(19);
      delayMicroseconds(880);
    }
    sec++; // count of number of "1 second" periods 
  } */
}
