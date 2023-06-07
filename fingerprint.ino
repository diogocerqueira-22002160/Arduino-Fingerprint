#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <PushButton.h>
#include <LiquidCrystal_I2C.h>
//Pino do botão
#define pinBtn 11
#define endereco 0x27
#define colunas 16
#define linhas 2

SoftwareSerial mySerial(2,3);
LiquidCrystal_I2C lcd(endereco, colunas, linhas);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
PushButton botao(pinBtn);

bool gravar=false;
uint8_t id;

uint8_t createFinger();

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.begin(9600);
  finger.begin(57600);
  if (finger.verifyPassword()){
    Serial.println("Sensor biometrico encontrado!");
  }else{
    Serial.println("Sensor biometrico não encontrado! Verifique a conexão e reinicie o sistema");
    while(true) { 
      delay(1); 
    }
  }
  Serial.println("Fim do Setup!");
}

void loop() {
  botao.button_loop();

  lcd.setCursor(0,0);
  lcd.print("Use o sensor ou");
  lcd.setCursor(0,1);
  lcd.print("clique no botao");

  if(botao.pressed()){
    gravar = true;
  }

  if(gravar){
    createFinger();
    gravar = false;
  }

  getFingerId();
}

uint8_t createFinger(){
  Serial.println("Digite o ID # (entre 1 e 127) da digital que vai gravar...");
  lcd.clear();
  lcd.print("Qual o id onde");
  lcd.setCursor(0,1);
  lcd.print("quer gravar?");
  id = lerNumero();
  if (id == 0){
    return;
  }
  lcd.clear();
  lcd.print("Registando");
  lcd.setCursor(0,1);
  lcd.print("ID: ");
  lcd.setCursor(4,1);
  lcd.print(id);
  delay(5000);

  lcd.clear();
  lcd.print("Insira o dedo");
  lcd.setCursor(0,1);
  lcd.print("no sensor");
  int p = -1;
  while (p != FINGERPRINT_OK) {
      p = finger.getImage();
      switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Leitura concluída");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        delay(200);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Erro comunicação");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Erro de leitura");
        break;
      default:
        Serial.println("Erro desconhecido");
        break;
      }
    }

    // OK SUCESSO

    p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }

  lcd.clear();
  lcd.print("Retire o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  lcd.clear();
  lcd.print("Coloque o mesmo");
  lcd.setCursor(0,1);
  lcd.print("dedo novamente");
  while (p != FINGERPRINT_OK) {
      p = finger.getImage();
      switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Leitura concluída");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        delay(200);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Erro comunicação");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Erro de leitura");
        break;
      default:
        Serial.println("Erro desconhecido");
        break;
      }
    }

    // OK SUCESSO

    p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }

  // OK convertido!
  Serial.print("Criando modelo para #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("digitais iguais");
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.clear();
    lcd.print("digitais nao");
    lcd.setCursor(0,1);
    lcd.print("batem");
    delay(2000);
    return p;
  } else {
    Serial.println("Erro desconhecido");
    return p;
  }  

   Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Armazenado com sucesso!");
    lcd.clear();
    lcd.print("Digital");
    lcd.setCursor(0, 1);
    lcd.print("armazenada");
    delay(2000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}

uint8_t lerNumero(void){
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

int getFingerId(){
  uint8_t p = finger.getImage();
   if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  //Encontrou uma digital!
  lcd.clear();
  lcd.print("ID: ");
  lcd.setCursor(4, 0);
  lcd.print(finger.fingerID);
  lcd.setCursor(0, 1);
  lcd.print("confianca:");
  lcd.setCursor(11, 1);
  lcd.print(finger.confidence);
  Serial.print("ID encontrado #"); Serial.print(finger.fingerID); 
  Serial.print(" com confiança de "); Serial.println(finger.confidence);
  delay(5000);
  return finger.fingerID;
}
