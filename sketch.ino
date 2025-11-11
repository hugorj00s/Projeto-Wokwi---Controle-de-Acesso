#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define RELAY_PIN 8

// ------------------- VARIÁVEIS -------------------
String senhaAcesso = "";
String senhaDigitada = "";
String novaSenha = "";
String confirmacaoSenha = "";
bool modoDefinirSenha = false;
bool modoAutenticacao = false;
bool modoAlterarSenha = false;
bool aguardandoConfirmacao = false;

// ------------------- LCD -------------------
LiquidCrystal_I2C lcd(0x27,16,2);

// ------------------- TECLADO -------------------
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2,3,4,5};
byte colPins[COLS] = {6,7,A0,A1};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ------------------- LOG VIA SERIAL -------------------
void registrarLog(String evento) {
  Serial.print("[LOG] ");
  Serial.println(evento);
}

// ------------------- LIBERA ACESSO -------------------
void liberarAcesso() {
  lcd.clear();
  lcd.print("ACESSO LIBERADO");
  digitalWrite(RELAY_PIN, HIGH);
  registrarLog("Acesso liberado");
  delay(3000);
  digitalWrite(RELAY_PIN, LOW);
  lcd.clear();
  lcd.print("Porta fechada");
  delay(1000);
  senhaDigitada = "";
  lcd.clear();
  lcd.print("Digite a senha:");
}

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Iniciando...");
  delay(1500);
  lcd.clear();

  if (senhaAcesso == "") {
    modoDefinirSenha = true;
    lcd.print("Defina a senha:");
  } else {
    modoAutenticacao = true;
    lcd.print("Digite a senha:");
  }
}

// ------------------- LOOP PRINCIPAL -------------------
void loop() {
  char tecla = keypad.getKey();
  if (!tecla) return;

  // MODO DEFINIR SENHA
  if (modoDefinirSenha) {
    if (tecla >= '0' && tecla <= '9') {
      novaSenha += tecla;
      lcd.setCursor(novaSenha.length()-1,1);
      lcd.print("*");
    } else if (tecla == '#') {
      if (novaSenha.length() >= 4) {
        senhaAcesso = novaSenha;
        registrarLog("Senha inicial definida");
        lcd.clear();
        lcd.print("Senha salva!");
        delay(1500);
        novaSenha = "";
        modoDefinirSenha = false;
        modoAutenticacao = true;
        lcd.clear();
        lcd.print("Digite a senha:");
      } else {
        lcd.clear();
        lcd.print("Min 4 digitos");
        delay(1500);
        lcd.clear();
        lcd.print("Defina a senha:");
        novaSenha = "";
      }
    }
    return;
  }

  // MODO AUTENTICAÇÃO
  if (modoAutenticacao) {
    if (tecla >= '0' && tecla <= '9') {
      senhaDigitada += tecla;
      lcd.clear();
      lcd.print("Senha:");
      lcd.setCursor(0,1);
      for (int i=0; i<senhaDigitada.length(); i++) lcd.print("*");
    } else if (tecla == '#') {
      if (senhaDigitada == senhaAcesso) {
        registrarLog("Acesso permitido");
        liberarAcesso();
      } else {
        registrarLog("Acesso negado");
        lcd.clear();
        lcd.print("Senha incorreta");
        delay(1500);
        lcd.clear();
        lcd.print("Digite a senha:");
      }
      senhaDigitada = "";
    } else if (tecla == '*') {
      modoAutenticacao = false;
      modoAlterarSenha = true;
      senhaDigitada = "";
      lcd.clear();
      lcd.print("Senha atual:");
    }
    return;
  }

  // MODO ALTERAR SENHA
  if (modoAlterarSenha) {
    if (!aguardandoConfirmacao) {
      if (tecla >= '0' && tecla <= '9') {
        senhaDigitada += tecla;
        lcd.setCursor(senhaDigitada.length()-1,1);
        lcd.print("*");
      } else if (tecla == '#') {
        if (senhaDigitada == senhaAcesso) {
          lcd.clear();
          lcd.print("Nova senha:");
          senhaDigitada = "";
          aguardandoConfirmacao = true;
        } else {
          lcd.clear();
          lcd.print("Senha incorreta");
          delay(1500);
          lcd.clear();
          lcd.print("Senha atual:");
          senhaDigitada = "";
        }
      }
    } else {
      if (tecla >= '0' && tecla <= '9') {
        novaSenha += tecla;
        lcd.setCursor(novaSenha.length()-1,1);
        lcd.print("*");
      } else if (tecla == '#') {
        if (novaSenha.length() >= 4) {
          senhaAcesso = novaSenha;
          registrarLog("Senha alterada");
          lcd.clear();
          lcd.print("Senha salva!");
          delay(1500);
          novaSenha = "";
          aguardandoConfirmacao = false;
          modoAlterarSenha = false;
          modoAutenticacao = true;
          lcd.clear();
          lcd.print("Digite a senha:");
        } else {
          lcd.clear();
          lcd.print("Min 4 digitos");
          delay(1500);
          lcd.clear();
          lcd.print("Nova senha:");
          novaSenha = "";
        }
      }
    }
  }
}
