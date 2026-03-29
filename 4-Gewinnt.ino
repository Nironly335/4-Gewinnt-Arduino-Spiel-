//Franz Regir - s86074, Radmir Mullagaliev - s85138
//We always have to include the library
#include "LedControl.h"
#include <LiquidCrystal.h>
#include <Bounce2.h>

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
// LED matrix
#define DATA_PIN 12
#define CLK_PIN 13
#define CS_PIN 11
//buttons
#define LEFT_PIN 2
#define RIGHT_PIN 1
#define CHECK_PIN 0

#define TOPS_INIT 0
//LCD display
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

LedControl lc=LedControl(DATA_PIN,CLK_PIN,CS_PIN,1);
Bounce ButtonLeft = Bounce();
Bounce ButtonRight = Bounce();
Bounce ButtonCheck = Bounce();

uint8_t playingField[8][8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t tops [8] = {TOPS_INIT};
uint8_t player = 1, setPos1 = 4, setPos2 = 4, currPos = 4;
bool light = true, interrActive = true;
/* we always wait a bit between updates of the display */
unsigned long delaytime=100;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  ButtonLeft.attach(LEFT_PIN, INPUT_PULLUP);
  ButtonLeft.interval(100);
  ButtonRight.attach(RIGHT_PIN, INPUT_PULLUP);
  ButtonRight.interval(100);
  
  ButtonCheck.attach(CHECK_PIN, INPUT_PULLUP);
  ButtonCheck.interval(100);

  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,4);
  /* and clear the display */
  lc.clearDisplay(0);
}

void loop() { 
  currPos = (player == 1)?setPos1:setPos2;
  lcdPrintRound();
  
  //player 1 move;
  interrActive = true;
  currentMatrixPlayer1();
  if (playingField[7][currPos] != 0)
    moveRight();
  while (!(ButtonCheck.update() && ButtonCheck.fell())) {    
    if (ButtonLeft.update() && ButtonLeft.fell()) {
      moveLeft();
    } else if (ButtonRight.update() && ButtonRight.fell()) {
      moveRight();
    }
    
    // show move
    showMove(currPos, light);
    currentMatrixPlayer2(light);
    light = !light;
    delay(100);
  }
  interrActive = false;
  //update data
  playingField[findTop(currPos)][currPos] = player;

  // check win
  printPlayingField();
  uint8_t ret = checkWin();
  Serial.print("return von checkWin: "); Serial.println(ret);
  tops[currPos]++;
  printTops();
  // go to next non-full column
  if (ret != 0){
    while (tops[currPos] == 7);
      currPos = (currPos+1) % 8;
  }
  // update player position
  if (player == 1)
    setPos1 = currPos;
  else
    setPos2 = currPos;
  // show results
  lcdPrintResult(ret);
  while (ret != 0 && !(ButtonCheck.update() && ButtonCheck.fell()));
  if (ret != 0) 
    resetGame();
  // change player
  player = player%2 + 1 ;
}
void resetGame() {
  lc.clearDisplay(0);
  for (uint8_t i = 0; i < 64; i++)
    playingField[i/8][i%8] = 0;
  for (uint8_t i = 0; i < 8; i++)
    tops[i] = 0;
  player = 1, setPos1 = 4, setPos2 = 4, currPos = 4;
  lcd.clear();
}
void moveLeft () {
  if (interrActive) {
    lc.setLed(0, findTop(currPos), currPos, false);
    currPos = (currPos + 1) % 8;
    while (playingField[7][currPos] != 0) {
      currPos = (currPos + 1) % 8;
    }
  }
}
void moveRight () {
  if (interrActive) {
    lc.setLed(0, findTop(currPos), currPos, false);
    currPos = (currPos - 1) % 8;
    while (playingField[7][currPos] != 0) {
      currPos = (currPos - 1) % 8;
    } 
  }
}
/*
void printTops() {
  Serial.print("tops:\n\t");
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(tops[i]);
    Serial.print(", ");
  }
  Serial.println("");
}
void printPlayingField() {
  Serial.println("playingField:");
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("\ti = "); Serial.print(i); Serial.print(": ");
    for (uint8_t j = 0; j < 8; j++){
      Serial.print(playingField[i][j]);
      Serial.print(", ");
    }
    Serial.println("");
  }
}
void lcdPrintText(char* text) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(text);
}
*/
void lcdPrintRound() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("player ");
  lcd.print(player);
  lcd.print("\'s turn");
  //lcd.print(roundCount/2+1);
  lcd.setCursor(0,1);
  lcd.print("left/throw/right");
}
void lcdPrintResult(uint8_t res) {
  if (res != 0){
    lcd.clear();
    lcd.setCursor(0,0);
  }
  if (res == 3) {
    lcd.print("game undecided");
    lcd.setCursor(0,1);
    lcd.print("retry? press mid");
  } else if (res == player) {
    lcd.print("player ");
    lcd.print(player);
    lcd.print(" has won");
    lcd.setCursor(0,1);
    lcd.print("retry? press mid");
  }
}
uint8_t checkWin() {
  uint8_t c = 1, currCol = currPos, currRow = findTop(currPos);

  //check Row
  while (playingField[currRow][--currCol] == player && currCol < 8)
    c++;
  currCol = currPos;
  while (playingField[currRow][++currCol] == player && currCol < 8)
    c++;
  if (c > 3)
    return player;
  
  currCol = currPos;
  c = 1;
  //check Col
  while (playingField[--currRow][currCol] == player && currRow < 8) 
    c++;
  
  if (c > 3)
    return player;
  c = 1;

  currRow = findTop(currPos);
  //check Diagonal top right
  while (playingField[--currRow][--currCol] == player && currRow < 8 && currCol < 8)
    c++;
  currCol = currPos;
  currRow = findTop(currPos);
  while (playingField[++currRow][++currCol] == player && currRow < 8 && currCol < 8)
    c++;
  if (c > 3)
    return player;
  c = 1;

  currCol = currPos;
  currRow = findTop(currPos);
  //check Diagonal bottom right
  while (playingField[++currRow][--currCol] == player && currRow < 8 && currCol < 8)
    c++;
  currCol = currPos;
  currRow = findTop(currPos);
  while (playingField[--currRow][++currCol] == player && currCol < 8 && currRow < 8)
    c++;
  if (c > 3)
    return player;
  
  // check if undecided, i.e. completely full matrix
  if (playingField[7][0] != 0 && playingField[7][1] != 0 && playingField[7][2] != 0 && playingField[7][3] != 0 && playingField[7][4] != 0 && playingField[7][5] != 0 && playingField[7][6] != 0 && playingField[7][7] != 0)
    return 3;
  else return 0;
}
void showMove(uint8_t pos, bool light) {
  if (player == 1)
    lc.setLed(0, findTop(pos), pos, true);
  else
    lc.setLed(0, findTop(pos), pos, light);

}
void setMatrix(uint8_t pos, bool move) {
  currentMatrixPlayer1();
}

void currentMatrixPlayer1() {
  for (uint8_t i = 0; i < 64; i++)
    if (playingField[i/8][i%8] == 1)
      lc.setLed(0, i/8, i%8, true);
}

void currentMatrixPlayer2(bool light) {
  for (uint8_t i = 0; i < 64; i++)
    if (playingField[i/8][i%8] == 2)
      lc.setLed(0, i/8, i%8, light);
}

uint8_t findTop(uint8_t pos) {
  return tops[pos];
}
