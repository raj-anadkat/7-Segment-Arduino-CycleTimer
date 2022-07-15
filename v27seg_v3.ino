//A 7 Segment Arduino based push button interface to set and store cycles timers for pumps
//raj-anadkat
//vrajanadkat@gmail.com

#include "LedControl.h"
#include <EEPROM.h>
LedControl lc = LedControl(7,6,5,4);

const int numOfInputs = 5; // number of buttons
const int inputPins[numOfInputs] = {8,9,10,11,12}; // assigned digital pins 
int inputState[numOfInputs];
int lastInputState[numOfInputs] = {LOW,LOW,LOW,LOW,LOW}; //initial state of buttons
bool inputFlags[numOfInputs] = {LOW,LOW,LOW,LOW,LOW}; // initial flags
long lastDebounceTime[numOfInputs] = {0,0,0,0,0};
long debounceDelay = 50; //lower the time if you notice debouncing (ms)
int timerinit = 0; //to prevent starting cycle timer at reboot 

//Menu Logic
const int numOfScreens = 3; // number of cycles 
int currentScreen = 0; // screen at reboot
int screens[numOfScreens] = {1,2,3};
int parameters[numOfScreens];

int parameters2[numOfScreens];
int parameters3[numOfScreens];
int editFlag = 1;
int digFlag = 1;
int numDig = 4;
int switchon = 0;
int stateChange = 0;

void setup() {
  for(int i =0;i<numOfInputs;i++) {
  pinMode(inputPins[i],INPUT);
  digitalWrite(inputPins[i],HIGH);
  delay(1000); 
 }
 Serial.begin(115200);
 lc.shutdown(0,false);
 lc.setIntensity(0,9);
 lc.clearDisplay(0);  
}


void loop() {
  
 if (switchon ==0){
  initializeEEPROMvariables();    
 }
 switchon = 1;
  stateChange = 0;
   setInputFlags();
  resolveInputFlags(); 
  if (stateChange == 1){
    eepromStore();
    
    
  }
}


void setInputFlags() {
  for(int i = 0;i<numOfInputs;i++){
    int reading = digitalRead(inputPins[i]);
    delay(20);
    if(reading != lastInputState[i]){
      lastDebounceTime[i]=millis();
    }
    if((millis()-lastDebounceTime[i])>debounceDelay){
      if (reading != inputState[i]){
        inputState[i] = reading;
        if(inputState[i]=HIGH){
          inputFlags[i]=HIGH;
        }
      }
    }
 lastInputState[i] = reading;
  }
}
void resolveInputFlags() {
  for(int i = 0;i<numOfInputs;i++){
    if(inputFlags[i]==HIGH){
      
      inputAction(i);
      inputFlags[i]=LOW;
      
      printScreen();
    }
  }
}



void inputAction(int input){

    if (input ==0 && digFlag == 1){
    if (currentScreen==numOfScreens-1){
      currentScreen=0;
    }else{
      currentScreen++;
    }
  }else if(input==1 && digFlag == 1 ){
  if (currentScreen==0){
      currentScreen=numOfScreens-1;
    }else{
      currentScreen--;
    }
    
  }
  
  if (input ==0 && digFlag == 2){
    stateChange = 1;
   if (parameters[currentScreen]>8){
      parameters[currentScreen]=0;
      
    }else{
      parameters[currentScreen]++;
      
    }
  }else if(input==1 && digFlag ==2){
    stateChange = 1;
    if (parameters[currentScreen]<0){
      parameters[currentScreen]=9;
      
    }else{
      parameters[currentScreen]--;
      
    }
  
    
  }
    if (input ==0 && digFlag == 3){
      stateChange = 1;
   if (parameters2[currentScreen]>8){
      parameters2[currentScreen]=0;
      
    }else{
      parameters2[currentScreen]++;
      
    }
  }else if(input==1 && digFlag ==3){
    stateChange = 1;
    if (parameters2[currentScreen]<0){
      parameters2[currentScreen]=9;
     
    }else{
      parameters2[currentScreen]--;
      
    }
  
    
  }

   if (input ==0 && digFlag == 4){
    stateChange = 1;
   if (parameters3[currentScreen]>8){
      parameters3[currentScreen]=0;
     
    }else{
      parameters3[currentScreen]++;
      
    }
  }else if(input==1 && digFlag ==4){
    stateChange = 1;
    if (parameters3[currentScreen]<0){
      parameters3[currentScreen]=9;
     
    }else{
      parameters3[currentScreen]--;
      
    }
  
    
  }
  
  
  
  else if(input==2 ){
     if (timerinit == 1){
    Serial.println("Starting Timer");
    cyclestart(0);
    }
    delay(1000);
    timerinit = 1;
    
  }
  else if(input==4){
    
    if (digFlag>numDig-1){
      digFlag = 1;
      
    }else{
      digFlag++;
      
    }
      lc.setChar(0,digFlag-1,18,false); // blinks the digit to be edited
       delay(200);
     
   }
  else if (input ==3) {
    if (digFlag==1){
      digFlag = numDig;
      
    }else{
      digFlag--;
      
    }
     
       lc.setChar(0,digFlag-1,18,false); // blinks the digit to be edited
       delay(200);    
  }
  }



void printScreen(){
    
  lc.setChar(0,0,screens[currentScreen],true);
  lc.setDigit(0,1,parameters[currentScreen],false);
  lc.setDigit(0,2,parameters2[currentScreen],false);
  lc.setDigit(0,3,parameters3[currentScreen],false);
}

void cyclestart(int key) {
    if(key==0){
       int units = parameters3[currentScreen];
   int tens = parameters2[currentScreen];
   int hundreds = parameters[currentScreen];
   int timer = units*1 + tens*10+100*hundreds;
   Serial.println(timer);
  timerstart(timer);
   }
  
  }
void timerstart(int count) {
  for(int i =1 ;i<=5;i++){
    lc.shutdown(0,true);
    delay(200);
    lc.shutdown(0,false);
    delay(200);
  }
  while(count != 0 ) {
    
  String t = String(count--);
    if (count >=99){
    lc.setDigit(0,1,(int)(t[0]-'0'),false);
    lc.setDigit(0,2,(int)(t[1]-'0'),false);
    lc.setDigit(0,3,(int)(t[2]-'0'),false);
    delay(60000);
    lc.shutdown(0,true);
    lc.shutdown(0,false);
    }
    else if (count >= 9 && count <99){
      lc.setDigit(0,1,0,false);
    lc.setDigit(0,2,(int)(t[0]-'0'),false);
    lc.setDigit(0,3,(int)(t[1]-'0'),false);
    delay(60000);
    lc.shutdown(0,true);
    lc.shutdown(0,false);
    }
    else {
      lc.setDigit(0,1,0,false);
    lc.setDigit(0,2,0,false);
    lc.setDigit(0,3,(int)(t[0]-'0'),false);
    delay(60000);
    
    }
}
}

void initializeEEPROMvariables() {
  parameters[0] = EEPROM.read(100);
  parameters[1] = EEPROM.read(110);
  parameters[2] = EEPROM.read(120);
  parameters2[0] = EEPROM.read(130);
  parameters2[1] = EEPROM.read(140);
  parameters2[2] = EEPROM.read(150);
  parameters3[0] = EEPROM.read(160);
  parameters3[1] = EEPROM.read(170);
  parameters3[2] = EEPROM.read(180);
}
void eepromStore() {
  EEPROM.write(100,parameters[0]);
  EEPROM.write(110,parameters[1]);
  EEPROM.write(120,parameters[2]);
  EEPROM.write(130,parameters2[0]);
  EEPROM.write(140,parameters2[1]);
  EEPROM.write(150,parameters2[2]);
  EEPROM.write(160,parameters3[0]);
  EEPROM.write(170,parameters3[1]);
  EEPROM.write(180,parameters3[2]);  
}
