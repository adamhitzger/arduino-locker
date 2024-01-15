#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>

#define CERVENA 11
#define ZELENA 12
#define BUZZER 13
const byte ROWS = 5; 
const byte COLS = 4; 
char hexaKeys[ROWS][COLS] = {
  {'A','B','#','*'},
  {'1','2','3','U'},
  {'4','5','6','D'},
  {'7','8','9','C'},
  {'L','0','R','E'},
};
byte rowPins[ROWS] = {10,9,8,7,6};
byte colPins[COLS] = {2,3,4,5}; 
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
LiquidCrystal_I2C lcd(0x27,16,2);
char key;
int attempts = 3;
int sekundy = 60;
String originalCode = "1111";
String originSettingCode = "147369258";
String code = "1111";
String user = "";

void setup() {
  pinMode(ZELENA, OUTPUT);
  pinMode(CERVENA, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("CODE: ");
  lcd.blink();
}

void countdown(){
  for(sekundy;sekundy >= 0;sekundy--){
    lcd.clear();
    lcd.setCursor(6, 1);
    lcd.print("seconds");
    lcd.setCursor(3, 1);
    lcd.print(sekundy);
    delay(1000);
    if(sekundy == 0){
      break;
    }
  }
  myInit();
  user == code;
}

void myInit(){
  lcd.clear(); 
  lcd.home();
  lcd.print("CODE: ");
}

void error(){
  lcd.clear();
  lcd.home();
  lcd.setCursor(5, 0);
  lcd.print("ERROR");
}

void unlocked(){
  lcd.clear();
  lcd.home();
  lcd.setCursor(5, 0);
  lcd.print("ODEMCENO");
}

void lock(){
  lcd.clear();
  lcd.home();
  lcd.setCursor(5, 0);
  lcd.print("ZAMKNUTO");
}

void passWord(){
  lcd.clear();
  lcd.home();
  lcd.setCursor(5, 1);
  lcd.print(code);
}

void deleteCode(){
  user = "";
  myInit();
}

void savetToEEPROM(String s, int addr){
  int length_pass = s.length();
  for(int i=addr; i<addr+length_pass; i++){
    EEPROM.write(i, s[i-addr]);
  }
  EEPROM.write(addr+length_pass+1, '|');
}

String readFromEEPROM(int addr){
  code = "";
  do{
    char c = (char)EEPROM.read(addr++);
    if(c == '|') break;
    else code += c;
  }
  while (1);
    return code;
  }

void loop() {
  key = keypad.getKey();

  if(key){
    switch (key){
      case 'C':
          deleteCode();
        break;
      case 'A':
          digitalWrite(ZELENA, 0);
          unlocked();
          delay(1500);
          deleteCode();
        break;
      case 'E':
          if(user.equals(code) == true){
            digitalWrite(ZELENA,1);
            lock();
          }else if(user.equals(originSettingCode) == true){
            lcd.noBacklight();
            delay(1500);
            lcd.backlight();
            myInit();
            code == originalCode;
          
          }else{
            error();
            digitalWrite(CERVENA, 1);
            tone(BUZZER, 1000);
            delay(1500);
            myInit();
            noTone(BUZZER);
            digitalWrite(CERVENA, 0);
            attempts--;
            if(attempts == 0){
              countdown();
            }
          }
        break ;
      case '*':
          if(user){
            savetToEEPROM(user, 0);
            readFromEEPROM(0);
            passWord();
            delay(1000);
            deleteCode();
          }
        break;
      case 'B':
        Serial.println(code);
      break;
      default:
          user += key;
          lcd.print(key);
    }
  }
  delay(10);
}