#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

/*
*  ----LCD-----
*  VCC -> 5 V
*  GND -> GND
*  SCL -> A5
*  SDA -> A4
*/

// ###################################

#define SPEAKER A3
#define JUMP_BUTTON 2
#define CROUCH_BUTTON 3
#define ROW_UP 0
#define ROW_DOWN 1

// MUZYKA

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0

int melody[] = {
  
  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4
};

// MUZYKA


const int delayTime = 500;
volatile bool interrupt_jump = false;
volatile bool interrupt_crouch = false;

enum ActionType {JUMP, CROUCH};

class Action {
  	public:
  	  ActionType type;
  	  bool isRunning;
  	  int left;
};

// -------------------------

class Mylcd {
  public:
  	const unsigned man = 1;
  	const unsigned manJump = 2;
  	const unsigned obstacle_down = 3;
    const unsigned obstacle_up = 4;
    const unsigned manCrouch = 5;
  	const unsigned empty = 32;
  
    void initialize() {
    	lcd.begin(16, 2);
  		lcd.clear();
  		lcd.setCursor(0, 0);
      
      	byte man_char[8] = {B01100,B01100,B00000,B01110,B11100,B01100,B11010,B10011};
		byte manJump_char[8] = {B01100,B01100,B00000,B11110,B01101,B11111,B10000,B00000};
        byte manCrouch_char[8] = {B00000,B00000,B00000,B01110,B11100,B01100,B11010,B10011};
		byte obstacle_down_char[8] = {B00101,B10101,B10101,B10101,B10110,B01100,B00100,B00100};
        byte obstacle_up_char[8] = {B00100,B00100,B01100,B10110,B10101,B10101,B10101,B00101};
  		
      	lcd.createChar(man, man_char);
  		lcd.createChar(manJump, manJump_char);
  		lcd.createChar(obstacle_down, obstacle_down_char);
        lcd.createChar(obstacle_up, obstacle_up_char);
        lcd.createChar(manCrouch, manCrouch_char);
  	}
  
  	void draw(const unsigned nr) {
    	lcd.write(nr);
  	}
  
  	void drawAt(const unsigned nr, const unsigned column, const unsigned row) {
    	lcd.setCursor(column, row);
      	lcd.write(nr);
  	}
  	
  	void printAt(const char* text, const unsigned column, const unsigned row) {
    	lcd.setCursor(column, row);
      	lcd.print(text);
  	}
  	
};

Mylcd mylcd;


// -----------------------
class Player {
	public:
  	  Action action;
  	  const unsigned xPos = 1;
  	  const unsigned actionLength = 4;
	  
  	  void initialize() {
        action.isRunning = false;
        action.left = 0;
        action.type = JUMP;
        mylcd.drawAt(mylcd.man, xPos, ROW_DOWN);
  	  }
  	  
  	  void update() {
        if(!action.isRunning) {
          if(interrupt_jump) {
             //tone(SPEAKER, 500, 100);
             action.isRunning = true;
             action.type = JUMP;
             action.left = actionLength;
             mylcd.drawAt(mylcd.empty, xPos, ROW_DOWN);
             mylcd.drawAt(mylcd.manJump, xPos, ROW_UP);
          } else if (interrupt_crouch) {
             //tone(SPEAKER, 400, 100);
             action.isRunning = true;
             action.type = CROUCH;
             action.left = actionLength;
             mylcd.drawAt(mylcd.manCrouch, xPos, ROW_DOWN);
          }
        } else {
           --action.left;
            if(action.left == 0) {
              action.isRunning = false;
              if(action.type == JUMP) {
              	mylcd.drawAt(mylcd.empty, xPos, ROW_UP);
              	mylcd.drawAt(mylcd.man, xPos, ROW_DOWN);
              } else {
                mylcd.drawAt(mylcd.man, xPos, ROW_DOWN); 
              }
              
          	}
        }
        
        interrupt_crouch = false;
        interrupt_jump = false;
  	  }
 	
};


Player player;

class Obstacle {
  public:
  	int xPos;
  	int yPos;
  	
  	void initialize(int xpos, int ypos) {
      xPos = xpos;
      yPos = ypos;
      if(yPos == ROW_UP) {
      	mylcd.drawAt(mylcd.obstacle_up, xPos, yPos);
      } else {
        mylcd.drawAt(mylcd.obstacle_down, xPos, yPos); 
      }

  	}
  
  	void update() {
      if(xPos >= 0 and xPos <= 15) mylcd.drawAt(mylcd.empty, xPos, yPos);
      --xPos;
      if(xPos >= 0 and xPos <= 15) {
        if(yPos == ROW_UP)
        	mylcd.drawAt(mylcd.obstacle_up, xPos, yPos);
        else
           mylcd.drawAt(mylcd.obstacle_down, xPos, yPos);
      }
  	}
};

Obstacle obstacle[3];

const int places[3][3] = {{16,-10,-10}, {16, 17, -10}, {16, 17, 18}};

// --------------------------

void handleJumpInterrupt() {
  interrupt_jump = true;
}

void handleCrouchInterrupt() {
  interrupt_crouch = true;
}

int SCORES = 0;

void changeObstacles() {
  for(int i = 0; i < 3; ++i) {
    if(obstacle[i].xPos >= 0) {
       return; 
    }
  }
 
  int randomX = random(0,3);
  int randomY = random(0,2);
  
  for(int i = 0; i < 3; ++i) {
    obstacle[i].xPos = places[randomX][i];
    obstacle[i].yPos = randomY;
  }
}

void playMusic() {
  int tempo = 144;
  int notes = sizeof(melody) / sizeof(melody[0]) / 2;
  int wholenote = (60000 * 4) / tempo;
  int divider = 0, noteDuration = 0;

  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }
    tone(SPEAKER, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(SPEAKER);
  }
}

void endGame() {
   lcd.clear();
   lcd.print("Koniec gry!");
   lcd.setCursor(0, ROW_DOWN);
   lcd.print("Wynik:");
   lcd.print(SCORES);
   //tone(SPEAKER, 500, 500);
   delay(5000);
   
   Serial.println(SCORES);
  
   int i = 1;
   unsigned long timeLoop = millis();
   while(1) {
     if(millis() - timeLoop > 15000) break;
     if(Serial.available() > 0) {
        String data = Serial.readStringUntil('\n');
        lcd.clear();
        lcd.print(i);
        lcd.print(".Wynik:");
        lcd.print(data);
        ++i;
        delay(5000);
        if(i > 3) break;
        timeLoop = millis();
     }
   }
   lcd.clear();
   lcd.print("Restart gry...");
   //playMusic();
  
   lcd.clear();
   lcd.setCursor(0, 0);
   SCORES = 0;
   
    player.initialize();

  	for(int i = 0; i < 3; ++i) {
    	obstacle[i].initialize(13+i, 1);
  	}
  
   interrupt_crouch = false;
   interrupt_jump = false;
}

void checkCollision() {
  for(int i = 0; i < 3; ++i) {
    if(player.xPos == obstacle[i].xPos) {
        if(!player.action.isRunning) endGame();
        else if(player.action.left == 1) endGame();
        else if(obstacle[i].yPos == ROW_UP && player.action.type == JUMP) endGame();
        else if(obstacle[i].yPos == ROW_DOWN && player.action.type == CROUCH) endGame();
    }
    
  }
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  pinMode(SPEAKER, OUTPUT);
  pinMode(JUMP_BUTTON, INPUT_PULLUP);
  pinMode(CROUCH_BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(JUMP_BUTTON), handleJumpInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(CROUCH_BUTTON), handleCrouchInterrupt, FALLING);
  
  lcd.clear();
  lcd.print("Start...");
  //playMusic();
  
  mylcd.initialize();
  player.initialize();

  for(int i = 0; i < 3; ++i) {
    obstacle[i].initialize(13+i, 1);
  }
  
}

void loop() {
  
    delay(delayTime);
    player.update();
    for(int i = 0; i < 3; ++i) {
      obstacle[i].update();
    }
    checkCollision();
    changeObstacles();
    ++SCORES;
}
