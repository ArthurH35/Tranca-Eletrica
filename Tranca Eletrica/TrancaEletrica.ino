#include <EEPROM.h>
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

#define TAM_SENHA 6
#define MAX_SENHAS 3
#define INICIO_SENHA_MESTRA 100

int botState = 0;
int flag = 0, flag2 = 0, flag3 = 0, flag4 = 0; 
int flagConfig = 0, flagMem;
int posicao = 0;
int tentativas = 0;
int aux;
int cont, cont2, cont3;
char usuario;
char key;
char senhaA[MAX_SENHAS][TAM_SENHA] = {{"5678*"},
									  {"ABCD*"},
									  {"DCBA*"} 
                                     };
char senhaB[MAX_SENHAS][TAM_SENHA] = {{"1234*"},
									  {"4321*"},
								      {"1A2B*"}
                                     };
char senhaC[MAX_SENHAS][TAM_SENHA] = {{"5678*"},
									  {"ABCD*"},
									  {"DCBA*"} 
                                     };
char senhaD[MAX_SENHAS][TAM_SENHA] = {{"1234*"},
									  {"4321*"},
								      {"1A2B*"}
                                     };
char senhaMomento[TAM_SENHA] = {0};
char senhaIns[TAM_SENHA] = {0};

void ConfiguracaoSenhaMestra(void){
  if(flagConfig == 0){
    lcd.clear();
    lcd.print("Insira a senha");
    lcd.setCursor(0,1);
    lcd.print("mestra");
    
    flagConfig = 1;
    cont = INICIO_SENHA_MESTRA;
  }
  
  key = keypad.getKey();
    
  if(key){
    lcd.clear();
    lcd.print(key);
    EEPROM.write(cont, key);
    cont++;
      
    while(1){
      key = keypad.getKey();
      
      if(key == '*'){
        EEPROM.write(cont, key);
        EEPROM.put(0, 1);
    	EEPROM.get(0, flagMem);
        break;
      }
      
      if(key){
        lcd.print(key);
        EEPROM.write(cont, key);
        cont++;      
      }
    }
  }
}

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
  flag = flag2 = flag3 = flag4 = 0;
  
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
         
        for(cont2 = INICIO_SENHA_MESTRA, cont3 = 0; cont2 <= cont; cont2++, cont3++){
          key = EEPROM.read(cont2);
          senhaMomento[cont3] = key;
        }
        
        if(strcmp(senhaIns, senhaMomento) == 0){
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
    aux = random(0,MAX_SENHAS);
    
    lcd.setCursor(0,1);
    lcd.print("Senha ");
    aux++;
    lcd.print(aux);
    aux--;
    
    switch(usuario){
      case 'A':
        strcpy(senhaMomento, senhaA[aux]);
        break;
      case 'B':
        strcpy(senhaMomento, senhaB[aux]);
        break;
      case 'C':
        strcpy(senhaMomento, senhaC[aux]);
        break;
      case 'D':
        strcpy(senhaMomento, senhaD[aux]);
        break;
      default:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ERROR");
        delay(2000);
        ReiniciaSistema();
    }
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
  Serial.begin(9600);  
  lcd.begin(16,2);
  pinMode(relePin, OUTPUT);
  pinMode(botao, INPUT);
  pinMode(buzz, OUTPUT);
  
  randomSeed(analogRead(A0));
  digitalWrite(relePin, LOW);
}
  
void loop(){
  
  
  EEPROM.get(0, flagMem);
  
  if(flagMem == 0){
    ConfiguracaoSenhaMestra();
  }
  
  if(flag == 0 && flagMem == 1){
    lcd.clear();
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
    delay(2000);
    ReiniciaSistema();
  }
  
  if(flag2 == 0){
    VerificaSenha();
  }
  
  if(flag2 == 1){
    if(flag4 == 0){
      
      while(1){
	    usuario = keypad.getKey();
    
        if(usuario){
          flag4 = 1;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Usuario: ");
          lcd.print(usuario);
          
          break;
        }
      }
    }
    
    VerificaSenha2();
  }
  
  delay(10);
}
