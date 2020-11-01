#include <LiquidCrystal.h>
#include <Keypad.h>
#include <string.h>

#define relePin 12
#define botao 10
#define buzz 11

LiquidCrystal lcd(13,15,16,17,18,19);

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
#define MAX_SENHAS 4

int botState = 0;
int flag = 0, flag2 = 0, flag3 = 0;
int posicao = 0;
int tentativas = 0;
int aux;
char key;
char senha[TAM_SENHA] = "1234";
char senhaLista[MAX_SENHAS][TAM_SENHA] = {{"5678"},
										  {"ABCD"},
										  {"DCBA"}, 
										  {"8765"}
                                         };
char senhaMomento[TAM_SENHA] = {0};
char senhaIns[TAM_SENHA] = {0};

void SenhaErrada(void){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WRONG");
  tone(buzz, 1000, 700);
  tentativas++;
  posicao = 0;
}

void Alarme(void){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BLOCKED");
  tone(buzz, 2000, 10000);
  delay(10000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  posicao = 0;
  tentativas = 0;
  flag = 0;
}

void ReiniciaSistema(void){
  lcd.clear();
  lcd.setCursor(0,0);
  digitalWrite(relePin, LOW);
  
  posicao = 0;
  tentativas = 0;
  flag = flag2 = flag3 = 0;
  
}

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
          SenhaErrada();
        }

        if(tentativas == 3){
		  Alarme();
        }

        break;
      }
    }
  }
}

void VerificaSenha2(void){
  if(flag3 == 0){
    flag3 = 1;
    aux = random(0,4);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Senha ");
    aux++;
    lcd.print(aux);
    aux--;
    
    strcpy(senhaMomento, senhaLista[aux]);
  }
  
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

          if(strcmp(senhaIns, senhaMomento) == 0){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("OPEN");
            digitalWrite(relePin, HIGH);
            tone(buzz, 500, 700);
            
            delay(2000);
            ReiniciaSistema();

          }else{
            SenhaErrada();
          }

          if(tentativas == 3){
            Alarme();
          }
          
         break;
        }
      }
    }
  }
}

void setup(){
  lcd.begin(16,2);
  pinMode(relePin, OUTPUT);
  pinMode(botao, INPUT);
  pinMode(buzz, OUTPUT);
  
  randomSeed(analogRead(A0));
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
    lcd.print("OPEN");
    tone(buzz, 900, 700);
  }
  
  if(flag2 == 0){
    VerificaSenha();
  }
  
  if(flag2 == 1){
    VerificaSenha2();
  }
  
  delay(10);
}
