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

#define TAM_SENHA 16
#define MAX_SENHAS 3
#define MAX_USUARIOS 4

#define INICIO_SENHA_MESTRA 100
#define CONTADOR_SENHA_MESTRA 10
#define INICIO_SENHA_A 200
#define CONTADOR_SENHA_A 20
#define INICIO_SENHA_B 300
#define CONTADOR_SENHA_B 30
#define INICIO_SENHA_C 400
#define CONTADOR_SENHA_C 40
#define INICIO_SENHA_D 500
#define CONTADOR_SENHA_D 50
#define QUANTIDADE_SENHAS 60
#define QUANTIDADE_USUARIOS 70

int botState = 0;
int flag = 0, flag2 = 0, flag3 = 0, flag4 = 0; 
int flagConfig = 0, flagMem;
int posicao = 0;
int tentativas = 0;
int aux, aux2, aux3;
int cont, cont2, cont3, cont4;
int enderecoQuantSenhas;
int momentoAlarme;
char auxQuant;
char quantUsuarios = 0;
char quantSenhas;
char contUsuarios;
char auxUsuario = 'A';
char usuario;
char key;
char senhaMomento[TAM_SENHA] = {0};
char senhaIns[TAM_SENHA] = {0};

void LimiteSenha(void){
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Limite");
  lcd.setCursor(2,1);
  lcd.print("Ultrapassado");
  delay(1000);
}

void ConfiguracaoSenhaMestra(void){
  EEPROM.put(2, 0);
  if(flagConfig == 0){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Defina a Senha");
    lcd.setCursor(5,1);
    lcd.print("Mestra");
    
    flagConfig = 1;
    cont = INICIO_SENHA_MESTRA;
  }
  
  key = keypad.getKey();
    
  if(key){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print(key);
    EEPROM.write(cont, key);
    cont++;
      
    while(1){
      key = keypad.getKey();
      
      if(key == '*'){
        EEPROM.write(cont, key);
        EEPROM.put(CONTADOR_SENHA_MESTRA, cont);
        EEPROM.put(0, 1);
    	EEPROM.get(0, flagMem);
        EEPROM.put(2, 1);
        break;
      }
      
      if(key){
        lcd.print(key);
        EEPROM.write(cont, key);
        cont++;      
      }
      
      if(cont - INICIO_SENHA_MESTRA > TAM_SENHA - 2){
        LimiteSenha();
        
        flagConfig = 0;
        break;
      }
    }
  }
}

void ConfiguracaoUsuarios(void){
  EEPROM.get(2, flagConfig);
  if(flagConfig == 1){
    while(1){
      if(flagConfig == 1){
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("Insira a Quant");
        lcd.setCursor(2,1);
        lcd.print("de Usuarios");

        flagConfig = 2;
      }

      key = keypad.getKey();

      if(key){
        if(key > '0' && key <= '4'){
          quantUsuarios = key;
          EEPROM.put(QUANTIDADE_USUARIOS, quantUsuarios);
          EEPROM.put(2, 2);
          break;
        }else{
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("Valor Invalido");
          delay(1000);

          flagConfig = 1;
        }
      }
    }
  }
  
  EEPROM.get(QUANTIDADE_USUARIOS, quantUsuarios);
  EEPROM.get(2, flagConfig);
  if(flagConfig == 2){
    enderecoQuantSenhas = QUANTIDADE_SENHAS;
    
    for(auxQuant = '0'; auxQuant < quantUsuarios; auxQuant++){
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Insira a Quant");
      lcd.setCursor(2,1);
      lcd.print("de Senhas ");
      lcd.print(auxUsuario);

      while(1){
        key = keypad.getKey();

        if(key){
          if(key > '0' && key <= '3'){
            EEPROM.put(enderecoQuantSenhas, key);
          
            enderecoQuantSenhas++;
            auxUsuario++;
            break;
          }else{
            lcd.clear();
            lcd.setCursor(1,0);
        	lcd.print("Valor invalido");
        	delay(1000);
            auxQuant--;
            break;
          }
        }
      }
      
      if(auxQuant == quantUsuarios - 1){
        EEPROM.put(0, 2);
        EEPROM.get(0, flagMem);
      }
    }
  }
  
  flagConfig = 3;
}

void ConfiguracaoSenhaA(void){ 
  aux = 1;
  aux2 = 0;
  cont = INICIO_SENHA_A;
  
  EEPROM.get(QUANTIDADE_SENHAS, quantSenhas);

  for(auxQuant = '0'; auxQuant < quantSenhas; auxQuant++){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Defina a Senha");
    lcd.setCursor(1,1);
    lcd.print(aux);
    lcd.print(" do Usuario A");

    while(1){
      key = keypad.getKey();

      if(key){
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print(key);
        EEPROM.write(cont, key);
        cont++;

        while(1){
          key = keypad.getKey();

          if(key == '*'){
            EEPROM.write(cont, key);
            EEPROM.put(CONTADOR_SENHA_A + aux2, cont);
            
            if(auxQuant == quantSenhas - 1){
              EEPROM.put(0, 3);
              EEPROM.get(0, flagMem);
            }
              
            aux++;
            cont = (INICIO_SENHA_A + ((TAM_SENHA - 1) * (aux - 1)));
            aux2 += 2;
            break;
          }

          if(key){
            lcd.print(key);
            EEPROM.write(cont, key);
            cont++;      
          }
          
          aux3 = (INICIO_SENHA_A + ((TAM_SENHA - 1) * (aux - 1)));

          if(cont - aux3 > TAM_SENHA - 2){
            LimiteSenha();
            
            auxQuant -= 1;
            cont = aux3;
            break;
          }
        }
        
        break;
      }
    }
  }
}

void ConfiguracaoSenhaB(void){ 
  aux = 1;
  aux2 = 0;
  cont = INICIO_SENHA_B;
  
  EEPROM.get(QUANTIDADE_SENHAS + 1, quantSenhas);

  for(auxQuant = '0'; auxQuant < quantSenhas; auxQuant++){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Defina a Senha");
    lcd.setCursor(1,1);
    lcd.print(aux);
    lcd.print(" do Usuario B");

    while(1){
      key = keypad.getKey();

      if(key){
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print(key);
        EEPROM.write(cont, key);
        cont++;

        while(1){
          key = keypad.getKey();

          if(key == '*'){
            EEPROM.write(cont, key);
            EEPROM.put(CONTADOR_SENHA_B + aux2, cont);
            
            if(auxQuant == quantSenhas - 1){
              EEPROM.put(0, 4);
              EEPROM.get(0, flagMem);
            }
            
            aux++;
            cont = (INICIO_SENHA_B + ((TAM_SENHA - 1) * (aux - 1)));
            aux2 += 2;
            break;
          }

          if(key){
            lcd.print(key);
            EEPROM.write(cont, key);
            cont++;      
          }
          
          aux3 = (INICIO_SENHA_B + ((TAM_SENHA - 1) * (aux - 1)));

          if(cont - aux3 > TAM_SENHA - 2){
            LimiteSenha();
            
            auxQuant -= 1;
            cont = aux3;
            break;
          }
        }
        
        break;
      }
    }
  }
}

void ConfiguracaoSenhaC(void){ 
  aux = 1;
  aux2 = 0;
  cont = INICIO_SENHA_C;
  
  EEPROM.get(QUANTIDADE_SENHAS + 2, quantSenhas);

  for(auxQuant = '0'; auxQuant < quantSenhas; auxQuant++){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Defina a Senha");
    lcd.setCursor(1,1);
    lcd.print(aux);
    lcd.print(" do Usuario C");

    while(1){
      key = keypad.getKey();

      if(key){
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print(key);
        EEPROM.write(cont, key);
        cont++;

        while(1){
          key = keypad.getKey();

          if(key == '*'){
            EEPROM.write(cont, key);
            EEPROM.put(CONTADOR_SENHA_C + aux2, cont);
            
            if(auxQuant == quantSenhas - 1){
              EEPROM.put(0, 5);
              EEPROM.get(0, flagMem);
            }
            
            aux++;
            cont = (INICIO_SENHA_C + ((TAM_SENHA - 1) * (aux - 1)));
            aux2 += 2;
            break;
          }

          if(key){
            lcd.print(key);
            EEPROM.write(cont, key);
            cont++;      
          }
          
          aux3 = (INICIO_SENHA_C + ((TAM_SENHA - 1) * (aux - 1)));

          if(cont - aux3 > TAM_SENHA - 2){
            LimiteSenha();

            auxQuant -= 1;
            cont = aux3;
            break;
          }
        }
        
        break;
      }
    }
  }
}

void ConfiguracaoSenhaD(void){ 
  aux = 1;
  aux2 = 0;
  cont = INICIO_SENHA_D;
  
  EEPROM.get(QUANTIDADE_SENHAS + 3, quantSenhas);

  for(auxQuant = '0'; auxQuant < quantSenhas; auxQuant++){
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Defina a Senha");
    lcd.setCursor(1,1);
    lcd.print(aux);
    lcd.print(" do Usuario D");

    while(1){
      key = keypad.getKey();

      if(key){
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print(key);
        EEPROM.write(cont, key);
        cont++;

        while(1){
          key = keypad.getKey();

          if(key == '*'){
            EEPROM.write(cont, key);
            EEPROM.put(CONTADOR_SENHA_D + aux2, cont);
            
            if(auxQuant == quantSenhas - 1){
              EEPROM.put(0, 6);
              EEPROM.get(0, flagMem);
            }
            
            aux++;
            cont = (INICIO_SENHA_D + ((TAM_SENHA - 1) * (aux - 1)));
            aux2 += 2;
            break;
          }

          if(key){
            lcd.print(key);
            EEPROM.write(cont, key);
            cont++;      
          }
          
          aux3 = (INICIO_SENHA_D + ((TAM_SENHA - 1) * (aux - 1)));

          if(cont - aux3 > TAM_SENHA - 2){
            LimiteSenha();

            auxQuant -= 1;
            cont = aux3;
            break;
          }
        }
        
        break;
      }
    }
  }
}

void LimpaSenhaMomento(void){
  for(cont4 = 0; cont4 < TAM_SENHA - 1; cont4++){
    senhaMomento[cont4] = 0;
  }
}

void LimpaSenhaIns(void){
  for(cont4 = 0; cont4 < TAM_SENHA - 1; cont4++){
    senhaIns[cont4] = 0;
  }
}

void SenhaErrada(void){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Senha Invalida");
  tone(buzz, 700, 400);
  tentativas++;
  posicao = 0;
}

void Alarme(void){
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Acesso");
  lcd.setCursor(4,1);
  lcd.print("Bloqueado");
  momentoAlarme = millis();
  
  while(1){
    tone(buzz, 1300, 200);
    delay(400);
  
    if(millis() - momentoAlarme >= 10000){
      break;
    }
  }
  
  ReiniciaSistema();
}

void UsuarioInvalido(void){
  lcd.clear();
  lcd.print("Usuario Invalido");
  delay(2000);
  ReiniciaSistema();
}

void SenhaAleatoria(void){
  aux2 = quantSenhas - 48;    
  aux = random(0,aux2);
  lcd.setCursor(5,1);
  lcd.print("Senha ");
  aux++;
  lcd.print(aux);
  aux--;
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
    LimpaSenhaMomento();
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print(key);
    senhaIns[posicao] = key;

    while(1){
      key = keypad.getKey();

      if(key){
        lcd.print(key);
        posicao++;
        senhaIns[posicao] = key;
      
        if(posicao == TAM_SENHA - 2 && key != '*'){
          SenhaErrada();
          LimpaSenhaIns();

          if(tentativas == 3){
            Alarme();
          }

          break;
        }

        if(senhaIns[posicao] == '*'){

          EEPROM.get(CONTADOR_SENHA_MESTRA, cont);

          for(cont2 = INICIO_SENHA_MESTRA, cont3 = 0; cont2 <= cont; cont2++, cont3++){
            key = EEPROM.read(cont2);
            senhaMomento[cont3] = key;
          }

          if(strcmp(senhaIns, senhaMomento) == 0){
            Serial.println(senhaMomento);
            Serial.println(senhaIns);
            lcd.clear();
            lcd.setCursor(3,0);
            lcd.print("Informe o");
            lcd.setCursor(4,1);
            lcd.print("Usuario");
            tone(buzz, 500, 400);

            flag2 = 1;
            posicao = 0;
            lcd.setCursor(0,0);
            LimpaSenhaIns();
            LimpaSenhaMomento();

          }else{
            SenhaErrada();
            LimpaSenhaIns();
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

void VerificaSenha2(void){
  if(flag4 == 0){
    
    flag4 = 1;
    EEPROM.get(QUANTIDADE_USUARIOS, quantUsuarios);
    
    switch(usuario){
      case 'A':
        EEPROM.get(QUANTIDADE_SENHAS, quantSenhas);
        
        SenhaAleatoria();
      
        EEPROM.get(CONTADOR_SENHA_A + aux * 2, cont);
         
        for(cont2 = INICIO_SENHA_A + ((TAM_SENHA - 1) * aux), cont3 = 0; cont2 <= cont; cont2++, cont3++){
          key = EEPROM.read(cont2);
          senhaMomento[cont3] = key;
        }
      
        Serial.println(senhaMomento);
        break;
      case 'B':
        if(quantUsuarios >= '2'){
          EEPROM.get(QUANTIDADE_SENHAS + 1, quantSenhas);
          
          SenhaAleatoria();

          EEPROM.get(CONTADOR_SENHA_B + aux * 2, cont);

          for(cont2 = INICIO_SENHA_B + ((TAM_SENHA - 1) * aux), cont3 = 0; cont2 <= cont; cont2++, cont3++){
            key = EEPROM.read(cont2);
            senhaMomento[cont3] = key;
          }
        }else{
          UsuarioInvalido();
        }
        
        Serial.println(senhaMomento);
        break;
      case 'C':
        if(quantUsuarios >= '3'){
          EEPROM.get(QUANTIDADE_SENHAS + 2, quantSenhas);
          
          SenhaAleatoria();

          EEPROM.get(CONTADOR_SENHA_C + aux * 2, cont);

          for(cont2 = INICIO_SENHA_C + ((TAM_SENHA - 1) * aux), cont3 = 0; cont2 <= cont; cont2++, cont3++){
            key = EEPROM.read(cont2);
            senhaMomento[cont3] = key;
          }
        }else{
          UsuarioInvalido();
        }
        
        Serial.println(senhaMomento);
        break;
      case 'D':
        if(quantUsuarios == '4'){
          EEPROM.get(QUANTIDADE_SENHAS + 3, quantSenhas);
          aux2 = quantSenhas - 48;

          SenhaAleatoria();

          EEPROM.get(CONTADOR_SENHA_D + aux * 2, cont);

          for(cont2 = INICIO_SENHA_D + ((TAM_SENHA - 1) * aux), cont3 = 0; cont2 <= cont; cont2++, cont3++){
            key = EEPROM.read(cont2);
            senhaMomento[cont3] = key;
          }
        }else{
          UsuarioInvalido();
        }
        
        Serial.println(senhaMomento);
        break;
      default:
        UsuarioInvalido();
    }
  }
  
  key = keypad.getKey();
    
  if(key){
    senhaIns[posicao] = key;
      
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print(key);
    
    while(1){
      key = keypad.getKey();

      if(key){
        posicao++;
        senhaIns[posicao] = key;
        lcd.print(key);
        
        if(posicao == TAM_SENHA - 2 && key != '*'){
          SenhaErrada();
          LimpaSenhaIns();
                  
          if(tentativas == 3){
            Alarme();
          }
          break;
        }

        if(senhaIns[posicao] == '*'){

          if(strcmp(senhaIns, senhaMomento) == 0){
            lcd.clear();
            lcd.setCursor(5,0);
            lcd.print("Aberto");
            digitalWrite(relePin, HIGH);
            tone(buzz, 500, 400);
            
            delay(2000);
            LimpaSenhaMomento();
            LimpaSenhaIns();
            ReiniciaSistema();

          }else{
            SenhaErrada();
            LimpaSenhaIns();
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

void Configuracao(void){
  EEPROM.get(0, flagMem);
  
  if(flagMem == 0){
    ConfiguracaoSenhaMestra();
  } 
 
  if(flagMem == 1){
    ConfiguracaoUsuarios();
  }
  
  EEPROM.get(QUANTIDADE_USUARIOS, quantUsuarios);
    
  if(flagMem == 2){
    ConfiguracaoSenhaA();
  }
    
  if(flagMem == 3 && quantUsuarios >= 2){
    ConfiguracaoSenhaB();
  }
    
  if(flagMem == 4 && quantUsuarios >= 3){
    ConfiguracaoSenhaC();
  }
    
  if(flagMem == 5 && quantUsuarios >= 4){
    ConfiguracaoSenhaD();
  }
}

void EstadoInicial(void){
  EEPROM.get(0, flagMem);
  
  if(flag == 0 && flagMem >= 2){
    lcd.clear();
    flag = 1;
    lcd.setCursor(4,0);
    lcd.print("Fechado");
  }
}

void AbrirPortaBotao(void){
  botState = digitalRead(botao);
  
  if(botState){
    digitalWrite(relePin, HIGH);
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("Aberto");
    tone(buzz, 500, 400);
    delay(2000);
    ReiniciaSistema();
  }
}

void RecebeUsuario(void){
  if(flag3 == 0){    
    while(1){
	  usuario = keypad.getKey();
    
      if(usuario){
        flag3 = 1;
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Usuario: ");
        lcd.print(usuario);
          
        break;
      }
    }
  }   
  VerificaSenha2();
}

void setup(){
  Serial.begin(9600);  
  lcd.begin(16,2);
  pinMode(relePin, OUTPUT);
  pinMode(botao, INPUT);
  pinMode(buzz, OUTPUT);
  
  /*for (int i = 0 ; i < EEPROM.length() ; i++){
    EEPROM.write(i, 0);
  }*/
  
  randomSeed(analogRead(A0));
  digitalWrite(relePin, LOW);
}
  
void loop(){ 
  Configuracao();
  EstadoInicial();
  AbrirPortaBotao();
  
  if(flag2 == 0){
    VerificaSenha();
  }
  
  if(flag2 == 1){
	RecebeUsuario();
  }
  
  delay(10);
}
