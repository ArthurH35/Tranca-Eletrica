#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <string.h>

#define botao 10
#define buzz 11
#define relePin 12

LiquidCrystal lcd(13,15,16,17,18,19);

const byte LINHAS = 4; 
const byte COLUNAS = 4; 
char keys[LINHAS][COLUNAS] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte linPins[LINHAS] = {9,8,7,6}; 
byte colPins[COLUNAS] = {5,4,3,2};

Keypad keypad = Keypad( makeKeymap(keys), linPins, colPins, LINHAS, COLUNAS );

/*  A constante define o tamanho limite da senha como 16,
  mas o tamanho real eh 14, sendo um espaco reservado para
  o caracter '*' (que possui a funcao de confirmar) e o outro
  para o '\0' da string.*/
#define TAM_SENHA 16
#define MAX_SENHAS 3
#define MAX_USUARIOS 4

/*  OBS: Todas as seguintes constantes consistem em enderecos da memoria 
  EEPROM.
    As constates que possuem "CONTADOR" no nome correspondem aos
  enderecos que armazenam onde as senhas terminam (valores que
  tambem sao enderecos da memoria EEPROM/Flash, os quais iniciam-se
  nas constantes com o nome "INICIO").*/
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
#define ENDERECO_MEM 0
#define ENDERECO_CONFIG 2

int botState = 0;
int flag = 0, flag2 = 0, flag3 = 0, flag4 = 0; 
int flagConfig = 0, flagMem;
int posicao = 0;
int tentativas = 0;
int aux, aux2, aux3;
int cont, cont2, cont3, cont4;
int enderecoQuantSenhas;
unsigned long int momentoAlarme;
char auxQuant;
char quantUsuarios;
char quantSenhas;
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
  EEPROM.put(ENDERECO_CONFIG, 0);
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
        EEPROM.put(ENDERECO_MEM, 1);
    	EEPROM.get(ENDERECO_MEM, flagMem);
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
  EEPROM.get(ENDERECO_CONFIG, flagConfig);
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
          EEPROM.put(ENDERECO_CONFIG, 2);
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
  EEPROM.get(ENDERECO_CONFIG, flagConfig);
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
        EEPROM.put(ENDERECO_MEM, 2);
        EEPROM.get(ENDERECO_MEM, flagMem);
      }
    }
  }
  
  flagConfig = 3;
}

void ConfiguracaoSenhaA(void){
  /*  A mesma logica aplicada nessa funcao sera utilizada nas
    demais configuracoes de senhas de usuarios, variando apenas
    as constantes simbolicas e a quantidade de senhas.*/
  aux = 1;//Representa a senha da iteracao atual
  aux2 = 0;//Auxilia no controle do endereco em que cont sera armazenada
  cont = INICIO_SENHA_A;//Representa o endereco em que os caracteres serao armazenados
  
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
              EEPROM.put(ENDERECO_MEM, 3);
              EEPROM.get(ENDERECO_MEM, flagMem);
            }
            
            /*  Ao concluir a configuracao da primeira senha, atualiza
              as variaveis para que elas passem a representar a senha
              seguinte.
                OBS: aux2 recebe o valor 2 pelo fato de que um valor do
              tipo int ocupa 2 bytes na memoria EEPROM.*/
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
          
          //aux3 representa o endereco inicial da senha atual.
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
              EEPROM.put(ENDERECO_MEM, 4);
              EEPROM.get(ENDERECO_MEM, flagMem);
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
              EEPROM.put(ENDERECO_MEM, 5);
              EEPROM.get(ENDERECO_MEM, flagMem);
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
              EEPROM.put(ENDERECO_MEM, 6);
              EEPROM.get(ENDERECO_MEM, flagMem);
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
  /*  O keypad gera valores do tipo char, enquanto a funcao random
   necessita de valores do tipo int. Diante disso, aux2 recebe o valor
   convertido (com base na tabela ASCII) da quantidade de senhas.*/
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
        
        //Recebe o endereco do ultimo caracter da senha "sorteada"
        EEPROM.get(CONTADOR_SENHA_A + aux * 2, cont);
        
        //Ja cont2 recebe o endereco do primeiro caracter da senha sorteada
        for(cont2 = INICIO_SENHA_A + ((TAM_SENHA - 1) * aux), cont3 = 0; cont2 <= cont; cont2++, cont3++){
          key = EEPROM.read(cont2);
          senhaMomento[cont3] = key;
        }
      
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
  EEPROM.get(ENDERECO_MEM, flagMem);
  
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
    
  if(flagMem == 3 && quantUsuarios >= '2'){
    ConfiguracaoSenhaB();
  }
    
  if(flagMem == 4 && quantUsuarios >= '3'){
    ConfiguracaoSenhaC();
  }
    
  if(flagMem == 5 && quantUsuarios >= '4'){
    ConfiguracaoSenhaD();
  }
}

void EstadoInicial(void){
  EEPROM.get(ENDERECO_MEM, flagMem);
  
  if(flag == 0 && flagMem >= 2){
    lcd.clear();
    flag = 1;
    lcd.setCursor(4,0);
    lcd.print("Fechado");
  }
}

void AbrirPortaBotao(void){
  botState = digitalRead(botao);
  EEPROM.get(ENDERECO_MEM, flagMem);
  
  if(botState && flagMem >= 2){
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
  lcd.begin(16,2);
  pinMode(relePin, OUTPUT);
  pinMode(botao, INPUT);
  pinMode(buzz, OUTPUT);
  
  //Funcao para limpeza da memoria EEPROM
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
  
  if(flag2 == 0 && flagMem >= 2){
    VerificaSenha();
  }
  
  if(flag2 == 1){
	RecebeUsuario();
  }
  
  delay(10);
}
