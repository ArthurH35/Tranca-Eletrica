#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <string.h>

#define relePin 12
#define botao 10
#define buzz 11
#define botao2 13

LiquidCrystal lcd(14,15,16,17,18,19);

const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte rowPins[ROWS] = {9,8,7,6}; 
byte colPins[COLS] = {5,4,3,2};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define TAM_SENHA 5
#define MAX_SENHAS 2

int botState = 0;
int flag = 0, flag2 = 0, flag3 = 0;
int posicao = 0;
int botCont = 0;
int tentativas = 0;
char key;
char senha[TAM_SENHA] = "1234";
char senha2[TAM_SENHA] = "5678";
char senha3[TAM_SENHA] = "ABCD";
char senhaIns[TAM_SENHA] = {0};

void VerificaSenha(void){
  key = keypad.getKey();

  if(key){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(key);
    senhaIns[posicao] = key;

    while(1){
      key = keypad.getKey();

      if(key){
        lcd.print(key);
        posicao++;
        senhaIns[posicao] = key;
      }

      if(posicao == TAM_SENHA - 2){

        if(strcmp(senhaIns, senha) == 0){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("NEXT");
          tone(buzz, 500, 700);

          flag2 = 1;
          posicao = 0;
          lcd.setCursor(0,0);

        }else{
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("WRONG");
          tone(buzz, 1000, 700);
          tentativas++;
          posicao = 0;
        }

        if(tentativas == 3){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("BLOCKED");
          tone(buzz, 2000, 10000);

          while(1){
            botState = digitalRead(botao2);

            if(botState && botCont != 2){
              botCont++;
              tentativas = 0;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("RESTARTED");
              noTone(buzz);
              break;
            }
          }
        }

        break;
      }
    }
  }
}

void VerificaSenha2(void){
  key = keypad.getKey();
    
  if(key){
    senhaIns[posicao] = key;
      
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(key);
    
    while(1){
      key = keypad.getKey();

      if(key){
        posicao++;
        senhaIns[posicao] = key;

        lcd.print(key);

        if(posicao == TAM_SENHA - 2){

          if(strcmp(senhaIns, senha2) == 0){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("NEXT");
            tone(buzz, 500, 700);

            flag2 = 2;
            posicao = 0;

          }else{
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WRONG2");
            tone(buzz, 1000, 700);
            tentativas++;
            posicao = 0;
          }

          if(tentativas == 3){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("BLOCKED");
            tone(buzz, 2000, 10000);

            while(1){
              botState = digitalRead(botao2);

              if(botState && botCont != 2){
                botCont++;
                tentativas = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("RESTARTED");
                noTone(buzz);
                break;
              }
            }
          }

         break;
        }
      }
    }
  }
}

void VerificaSenha3(void){
  key = keypad.getKey();
    
  if(key){
    senhaIns[posicao] = key;
      
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(key);
    
    while(1){
      key = keypad.getKey();

      if(key){
        posicao++;
        senhaIns[posicao] = key;

        lcd.print(key);

        if(posicao == TAM_SENHA - 2){

          if(strcmp(senhaIns, senha3) == 0){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("OPEN");
            tone(buzz, 500, 700);
            
			digitalWrite(relePin, HIGH);
            delay(2000);
            digitalWrite(relePin, LOW);
            posicao = 0;

          }else{
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("WRONG3");
            tone(buzz, 1000, 700);
            tentativas++;
            posicao = 0;
          }

          if(tentativas == 3){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("BLOCKED");
            tone(buzz, 2000, 10000);

            while(1){
              botState = digitalRead(botao2);

              if(botState && botCont != 2){
                botCont++;
                tentativas = 0;
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("RESTARTED");
                noTone(buzz);
                break;
              }
            }
          }

         break;
        }
      }
    }
  }
}

void setup(){
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(relePin, OUTPUT);
  pinMode(botao, INPUT);
  pinMode(botao2, INPUT);
  pinMode(buzz, OUTPUT);
  
  digitalWrite(relePin, LOW);
}
  
void loop(){
  
  if(flag == 0){
    flag = 1;
    lcd.print("CLOSED");
  }
  
  botState = digitalRead(botao);
  
  if(botState){
    digitalWrite(relePin, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CLOSED");
    tone(buzz, 900, 700);
  }
  
  if(flag2 == 0){
    VerificaSenha();
  }
  
  if(flag2 == 1){
    VerificaSenha2();
  }
  
  if(flag2 == 2){
    VerificaSenha3();
  }
  
  delay(10);
}