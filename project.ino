#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define SPEAKER A3
#define JUMP_BUTTON 2
#define CROUCH_BUTTON 3

#define CHAR_MAN 0
#define CHAR_OBSTACLE 1 
#define CHAR_EMPTY " "
#define CHAR_MAN_JUMP 3

#define OBSTACLES_NUMBER 2

struct JUMP {
  bool isExecuted;
  int left;
};

const int position = 1;
const int delayTime = 500;
JUMP jump = {false, 0};
volatile bool interrupt_jump_detected = false;
volatile bool interrupt_crouch_detected = true;

int obstacles[OBSTACLES_NUMBER] = {
  12,11
};

byte man[8] = {
  B01100,
  B01100,
  B00000,
  B01110,
  B11100,
  B01100,
  B11010,
  B10011
};

byte manJump[8] = {
  B01100,
  B01100,
  B00000,
  B11110,
  B01101,
  B11111,
  B10000,
  B00000,
};

byte obstacle[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void moveObstacles() {
  for(int i = 0; i < OBSTACLES_NUMBER; ++i) {
     lcd.setCursor(obstacles[i],1);
    lcd.write(CHAR_EMPTY);
    --obstacles[i]; 
  }
  
  for(int i = 0; i < OBSTACLES_NUMBER; ++i) {
    lcd.setCursor(obstacles[i],1);
    lcd.write(byte(CHAR_OBSTACLE));
  }
}

void buttonJumpPressed() {
  interrupt_jump_detected = false;
  tone(SPEAKER, 500, 100);
  lcd.setCursor(position, 1);
  lcd.print(CHAR_EMPTY);
  
  lcd.setCursor(position,0);
  lcd.write(byte(CHAR_MAN_JUMP));
  
  jump.isExecuted = true;
  jump.left = 4;
}

void handleJumpInterrupt() {
  interrupt_jump_detected = true;
}

void handleCrouchInterrupt() {
  interrupt_crouch_detected = true;
}

void makeJump() {
 --jump.left;
  if(jump.left == 0) {
    jump.isExecuted = false;
    lcd.setCursor(position, 0);
    lcd.write(CHAR_EMPTY);
    lcd.setCursor(position, 1);
    lcd.write(byte(CHAR_MAN));
  }
}

void checkCollision() {
  
  for(int i=0; i < OBSTACLES_NUMBER; ++i) {
    if(!jump.isExecuted && obstacles[i] == position) {
      lcd.setCursor(0,0);
        lcd.print("Koniec gry!");
        break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  
  pinMode(SPEAKER, OUTPUT);
  
  pinMode(JUMP_BUTTON, INPUT_PULLUP);
  pinMode(CROUCH_BUTTON, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(JUMP_BUTTON), 
                  handleJumpInterrupt, FALLING);

   attachInterrupt(digitalPinToInterrupt(CROUCH_BUTTON), 
                  handleCrouchInterrupt, FALLING);

  lcd.createChar(CHAR_MAN, man);
  lcd.createChar(CHAR_OBSTACLE, obstacle);
  lcd.createChar(CHAR_MAN_JUMP, manJump);
  
  lcd.setCursor(position,1);
  lcd.write(byte(CHAR_MAN));
  
  for(int i = 0; i < OBSTACLES_NUMBER; ++i) {
    lcd.setCursor(obstacles[i],1);
    lcd.write(byte(CHAR_OBSTACLE));
  }
}

void loop() {
 
  delay(delayTime);
  moveObstacles();
  if(interrupt_jump_detected) buttonJumpPressed();
  if(interrupt_crouch_detected) {
    Serial.println("Crouch");
    interrupt_crouch_detected = false;
  }
  if(jump.isExecuted) {
  makeJump(); 
  }
  checkCollision();
  Serial.println("Dzialam!");
}
