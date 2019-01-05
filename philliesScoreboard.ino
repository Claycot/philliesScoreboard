//Define statements for clearer code
#define TEAM_AWAY 0
#define TEAM_HOME 1
#define INNING_TOP 0
#define INNING_BOT 1
#define BASE_EMPTY 0
#define BASE_FULL 1

//Define bits for individual LEDs
#define BOT_INNING  0b00000001
#define TOP_INNING  0b00000010
#define THIRD_BASE  0b00000100
#define SECOND_BASE 0b00001000
#define FIRST_BASE  0b00010000
#define RIGHT_OUT   0b00100000
#define LEFT_OUT    0b01000000
#define FUTURE_LED  0b10000000

//Categories of display data
enum displayType {
  TYPE_PITCHER = 1,
  TYPE_TEAM,
  TYPE_SCORE,
  TYPE_INNING,
};

enum displayLED {
  LED_OUTS = 1,
  LED_INNHALF,
  LED_RUNNERS
};

//Pin numbering for shift registers
const byte clockPin = 6;
const byte dataBusPin = 7;
const byte powerBusPin = 8;
const byte latch1Pin = 9;
const byte latch2Pin = 10;
const byte latch3Pin = 11;
const byte latch45Pin = 12;
const byte latch6Pin = 13;

//Hex addresses for quantities
static int hexIndex[9] = {0, 0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
static int hexLetter[27] = {0x3BC0, 0x3C52, 0x2700, 0x3C12, 0x27C0, 0x23C0, 0x2F40, 0x1BC0, 0x2412, 0x1E00, 0x038C, 0x0700, 0x1B28, 0x1B24, 0x3F00, 0x33C0, 0x3F04, 0x33C4, 0x2DC0, 0x2012, 0x1F00, 0x0309, 0x1B05, 0x002D, 0x002A, 0x2409, 0};
static int hexNumber[11] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x73, 0x1};

//Program counter used for the loop, allows for actions to cycle instead of always running
static byte programCounter;

//Information used to display on the scoreboard (default parameters)
byte awayTeam[3] = {"POW"};
byte homeTeam[3] = {"ON "};
byte awayPitcher = 12;
byte homePitcher = 34;
byte awayScore = 56;
byte homeScore = 78;
byte inningNum = 90;
bool inningIsBot = 0;
bool runnersOn[3] = {BASE_FULL, BASE_FULL, BASE_FULL};
byte outs = 2;

//Initialize environment
void setup() {
  programCounter = 0;
  
  //Set pin mode for all register pins
  pinMode(clockPin, OUTPUT);
  pinMode(dataBusPin, OUTPUT);
  pinMode(powerBusPin, OUTPUT);
  pinMode(latch1Pin, OUTPUT);
  pinMode(latch2Pin, OUTPUT);
  pinMode(latch3Pin, OUTPUT);
  pinMode(latch45Pin, OUTPUT);
  pinMode(latch6Pin, OUTPUT);

  //Lock all latch pins
  digitalWrite(latch1Pin, HIGH);
  digitalWrite(latch2Pin, HIGH);
  digitalWrite(latch3Pin, HIGH);
  digitalWrite(latch45Pin, HIGH);
  digitalWrite(latch6Pin, HIGH);

  //Establish WiFi connection
  
}

//Functions (High Level) used within the loop
void writePitcher(bool teamIsHome, byte pitcherNumber);
void writeScore(bool teamIsHome, byte teamScore);
void writeName(bool teamIsHome, byte teamName[3]);
void writeInningNum(byte inningNumber);
void writeLEDs(bool baseStatus[3], byte numOuts, bool inningHalf);
void setRunners(bool baseStatus[3], byte &LEDsequence);
void setOuts(byte numOuts, byte &LEDsequence);
void setTopBotInn(bool inningHalf, byte &LEDsequence);

//Functions (Low Level) used within the loop
void illuminateDigit(displayType displayCategory, byte digitNumOnLine, bool teamIsHome = TEAM_AWAY);
void writeData(displayType displayCategory, byte displayChar);
void stopGhosting(byte latchPin, byte delayLength = 0);

//Main loop, uses a program counter structure to stagger updating of data
void loop() {
  //Make sure counter goes between 4 states (0 to 3)
  programCounter %= 4;

  switch (programCounter) {
    case 0: //Read information from internet
      //ToDo: Actually fetch the info for each of these variables below
//      awayTeam = ;
//      homeTeam = ;
//      awayPitcher = ;
//      homePitcher = ;
//      awayScore = ;
//      homeScore = ;
//      inningNum = ;
//      inningIsBot = ;
//      runnersOn = ;
//      outs = ;
      break;
    case 1: //Write pitcher and score display
      //ToDo
      writePitcher(TEAM_AWAY, awayPitcher);
      writePitcher(TEAM_HOME, homePitcher);
      writeScore(TEAM_AWAY, awayScore);
      writeScore(TEAM_HOME, homeScore);
      break;
    case 2: //Write team names and inning number
      //ToDo
      writeName(TEAM_AWAY, awayTeam);
      writeName(TEAM_HOME, homeTeam);
      writeInningNum(inningNum);
      break;
    case 3: //Write runners, outs, and top/bot inning LEDs
      //ToDo
      writeLEDs(runnersOn, outs, inningIsBot);
      break;      
  }

  programCounter++;
}

//Functions (High Level) used within the loop
void writePitcher(bool teamIsHome, byte pitcherNumber) {
  byte ones = pitcherNumber % 10;
  byte tens = (pitcherNumber - ones) / 10;
  writeData(TYPE_PITCHER, tens);
  illuminateDigit(TYPE_PITCHER, 1, teamIsHome);
  writeData(TYPE_PITCHER, ones);
  illuminateDigit(TYPE_PITCHER, 2, teamIsHome);
  return;
}

void writeScore(bool teamIsHome, byte teamScore) {
  byte ones = teamScore % 10;
  byte tens = (teamScore - ones) / 10;
  writeData(TYPE_SCORE, tens);
  illuminateDigit(TYPE_SCORE, 1, teamIsHome);
  writeData(TYPE_SCORE, ones);
  illuminateDigit(TYPE_SCORE, 2, teamIsHome);
  return;
}

void writeName(bool teamIsHome, byte teamName[3]) {
  writeData(TYPE_TEAM, teamName[0]);
  illuminateDigit(TYPE_TEAM, 1, teamIsHome);
  writeData(TYPE_TEAM, teamName[1]);
  illuminateDigit(TYPE_TEAM, 2, teamIsHome);
  writeData(TYPE_TEAM, teamName[2]);
  illuminateDigit(TYPE_TEAM, 3, teamIsHome);;
  return;
}

void writeInningNum(byte inningNumber) {
  byte ones = inningNumber % 10;
  byte tens = (inningNumber - ones) / 10;
  writeData(TYPE_INNING, tens);
  illuminateDigit(TYPE_INNING, 1);
  writeData(TYPE_INNING, ones);
  illuminateDigit(TYPE_INNING, 2);
  return;
}

void writeLEDs(bool baseStatus[3], byte numOuts, bool inningHalf) {
  byte registerValue = 0;
  byte &registerLoc = registerValue; 

  setRunners(baseStatus, registerLoc);
  setOuts(numOuts, registerLoc);
  setTopBotInn(inningHalf, registerLoc);

   //Write the latch low, to enable writing
  digitalWrite(latch3Pin, LOW);
  //Shift out the byte
  shiftOut(powerBusPin, clockPin, LSBFIRST, registerValue);
  //Write the latch high, to disable writing
  digitalWrite(latch3Pin, HIGH); 

  return;
}

void setRunners(bool baseStatus[3], byte &LEDsequence) {
  byte maskRunners = 0;
  
  if (baseStatus[0] == BASE_FULL) {
    maskRunners = maskRunners | FIRST_BASE;
  }
  if (baseStatus[1] == BASE_FULL) {
    maskRunners = maskRunners | SECOND_BASE;
  }
  if (baseStatus[2] == BASE_FULL) {
    maskRunners = maskRunners | THIRD_BASE;
  }
  
  LEDsequence = LEDsequence | maskRunners;
  return;
}

void setOuts(byte numOuts, byte &LEDsequence) {
  byte maskOuts = 0;
  
  switch(numOuts) {
    case 0: 
      break;
    case 1:
      maskOuts = LEFT_OUT;
      break;
    case 2:
      maskOuts = LEFT_OUT | RIGHT_OUT;
      break;
    case 3:
      break;
  }
  
  LEDsequence = LEDsequence | maskOuts;
  return; 
}
void setTopBotInn(bool inningHalf, byte &LEDsequence) {
  byte maskInn = 0;
  
  if (inningHalf == INNING_TOP) {
    maskInn = TOP_INNING;
  }
  else {
    maskInn = BOT_INNING;
  }
    
  LEDsequence = LEDsequence | maskInn;
  return;
}

//Functions (Low Level) used within the loop
void illuminateDigit(displayType displayCategory, byte digitNumOnLine, bool teamIsHome = TEAM_AWAY) {
  byte digitNum;
  switch (displayCategory) {
    case TYPE_PITCHER:
      //stopGhosting(latch2Pin);
      digitalWrite(latch2Pin, LOW);
      teamIsHome ? digitNum = digitNumOnLine + 2 : digitNum = digitNumOnLine; 
      shiftOut(powerBusPin, clockPin, LSBFIRST, hexIndex[digitNum]);
      digitalWrite(latch2Pin, HIGH);
      stopGhosting(latch2Pin);
      break;
    case TYPE_TEAM:
      //stopGhosting(latch6Pin);
      digitalWrite(latch6Pin, LOW);
      teamIsHome ? digitNum = digitNumOnLine + 3 : digitNum = digitNumOnLine; 
      shiftOut(powerBusPin, clockPin, LSBFIRST, hexIndex[digitNum]);
      digitalWrite(latch6Pin, HIGH);
      stopGhosting(latch6Pin);
      break;
    case TYPE_SCORE:
      //stopGhosting(latch2Pin);
      digitalWrite(latch2Pin, LOW);
      teamIsHome ? digitNum = digitNumOnLine + 6 : digitNum = digitNumOnLine + 4; 
      shiftOut(powerBusPin, clockPin, LSBFIRST, hexIndex[digitNum]);
      digitalWrite(latch2Pin, HIGH);
      stopGhosting(latch2Pin);
      break;
    case TYPE_INNING:
      //stopGhosting(latch6Pin);
      digitalWrite(latch6Pin, LOW);
      digitNum = digitNumOnLine + 6;
      shiftOut(powerBusPin, clockPin, LSBFIRST, hexIndex[digitNum]);
      digitalWrite(latch6Pin, HIGH);
      stopGhosting(latch6Pin);
      break;     
  }
  return;
}

void stopGhosting(byte latchPin, byte delayLength = 0) {
  digitalWrite(latchPin, LOW);
  shiftOut(powerBusPin, clockPin, LSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  delay(delayLength);
  return;
}

//Usage:
//displayCategory = type of data being shifted out (pitcher, team name, etc.)
//displayChar = either the integer or the letter to be printed
void writeData(displayType displayCategory, byte displayChar) {
  switch (displayCategory) {
    case TYPE_PITCHER:
      digitalWrite(latch1Pin, LOW);
      shiftOut(dataBusPin, clockPin, LSBFIRST, ~hexNumber[displayChar]);
      digitalWrite(latch1Pin, HIGH);
      break;
    case TYPE_TEAM:
      digitalWrite(latch45Pin, LOW);
      displayChar -= 'A';
      shiftOut(dataBusPin, clockPin, LSBFIRST, ~lowByte(hexLetter[displayChar]));
      shiftOut(dataBusPin, clockPin, LSBFIRST, ~highByte(hexLetter[displayChar]));
      digitalWrite(latch45Pin, HIGH);
      break;
    case TYPE_SCORE:
      digitalWrite(latch1Pin, LOW);
      shiftOut(dataBusPin, clockPin, LSBFIRST, ~hexNumber[displayChar]);
      digitalWrite(latch1Pin, HIGH);
      break;
    case TYPE_INNING:
      digitalWrite(latch45Pin, LOW);
      shiftOut(dataBusPin, clockPin, LSBFIRST, ~hexNumber[displayChar]);
      //Since there are two registers here, need to push a 0 to the 4 register, to get real byte to the 5 register :)
      shiftOut(dataBusPin, clockPin, LSBFIRST, 0);
      digitalWrite(latch45Pin, HIGH);
      break;     
  }
  return;
}
