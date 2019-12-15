#include "LedControl.h"

LedControl lc=LedControl(12,10,11,1);  // Pins: DIN,CLK,CS, # of Display connected

unsigned long delayTime=200;  // Delay between Frames

byte pong[] =
{
   B00000000,  
   B00000000,
   B00000000,
   B00000000,
   B00000000,
   B00000000,
   B00000000,
   B00000000
};

byte initBallPos[] =
{
   B00000000,  
   B00000000,
   B00000000,
   B00010000,
   B00000000,
   B00000000,
   B00000000,
   B00000000
};

byte ballPos[] =
{
   B00000000,  
   B00000000,
   B00000000,
   B00010000,
   B00000000,
   B00000000,
   B00000000,
   B00000000
};

byte paddlePos[] =
{
   B00000000,
   B00000000,
   B10000001,
   B10000001,
   B10000001,
   B00000000,
   B00000000,
   B00000000
};

bool up = true;
bool left = false;
bool gameOver = false;
bool playerWin = false;
int playerScore = 0;
int systemScore = 0;

unsigned long currentMillis = 0;    
unsigned long previousBallUpdate = 0;
unsigned long previousPaddleUpdate = 0;
int ballInterval = 70;
int paddleInterval = 300;

void setup()
{
  lc.shutdown(0,false);  // Wake up displays
  lc.setIntensity(0,1);  // Set intensity levels
  lc.clearDisplay(0);  // Clear Displays
}

void displayPong()
{  
  for (int i = 0; i < 8; i++)  
  {
    lc.setRow(0,i,ballPos[i] | paddlePos[i]);
  }
}

void moveVertBall() {
  if (ballPos[0] > 0) { up = false; }
  if (ballPos[7] > 0) { up = true; }
  
  if (up) { 
    for (int i = 0; i < 7; i++) {
      ballPos[i] = ballPos[i+1];
    } 
    ballPos[7] = 0;
  }
  else if (!up) {
    for (int i = 7; i > 0; i--) {
      ballPos[i] = ballPos[i-1];
    } 
    ballPos[0] = 0;
  }    delay(delayTime);

}

bool isBitSet(int k, byte b) 
{ 
    return (b & (1 << (k - 1)));
} 

void moveHorzBall() {
  for (int i = 0; i < 8; i++) {
    
    //game over conditions
    if (ballPos[i] & B10000000) {
      gameOver = true;
      playerWin = true;
      playerScore += 1;
      return;
    }
    if (ballPos[i] & B00000001) {
      gameOver = true;
      playerWin = false;
      systemScore += 1;
      return;
    }

    //bounce condition
    if (ballPos[i] == B01000000 && paddlePos[i] & B10000000) {
      left = false;
    }
    if (ballPos[i] == B00000010 && paddlePos[i] & B00000001) {
      left = true;
    }
    
    if (left) {
      ballPos[i] = ballPos[i] << 1; 
    }
    else if (!left) {
      ballPos[i] = ballPos[i] >> 1;
    }
  }
}


void movePaddlePos() {
   
  if (up && ! (paddlePos[0] & B10000001)) { 
    for (int i = 0; i < 7; i++) {
      paddlePos[i] = paddlePos[i+1];
    } 
    paddlePos[7] = 0;
  }
  else if (!up && ! (paddlePos[7] & B10000001)) {
    for (int i = 7; i > 0; i--) {
      paddlePos[i] = paddlePos[i-1];
    } 
    paddlePos[0] = 0;
  }
}

void showScore() {
  lc.setColumn(0,0,reinterpret_cast<byte*>((int)pow(2,systemScore)-1));
  lc.setColumn(0,7,reinterpret_cast<byte*>((int)pow(2,playerScore)-1));
}

void showWin() {
  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < 8; i++) {
      lc.setRow(0,i, ((i+j)%2==0) ? (B10101010) : (B01010101) );
    }
    delay(150);
  }
}

void loop()
{
    currentMillis = millis();
    
    displayPong();
    if (!gameOver) {
      if (currentMillis - previousBallUpdate >= ballInterval) {
        moveVertBall();
        moveHorzBall();
        previousBallUpdate = millis();
      }
      if (currentMillis - previousPaddleUpdate >= paddleInterval) {
        movePaddlePos();
        previousPaddleUpdate = millis();
      }
    }
    else if (gameOver) {
      if (systemScore >= 8 || playerScore >= 8) {
        systemScore = 0;
        playerScore = 0;
        showWin();
      }
      else {
        showScore();
      }
      delay(1500);
      gameOver = false;
      memcpy(&ballPos, &initBallPos, sizeof(initBallPos));
      displayPong();
      delay(1000);
    }

}
