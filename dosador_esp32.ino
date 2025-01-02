#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>

#define INC_PIN 6  
#define U_D_PIN 7  
#define CS_PIN 8   
#define MOTOR_PIN 18  
#define PINO_LV 10  
#define CONFIG_SENT_FLAG_ADDR 10  

int potValue = 0;    
int valueOLD = 0;    

bool requisicaoFeita = false;
const char* defaultSSID = "Promultt_Diretoria";
const char* defaultPassword = "pro!11852";
const char* serverUrl = "https://www.mexdistribuidora.com.br/api/dosadorArduino.php";
const char* serverUrlDosagem = "https://www.mexdistribuidora.com.br/api/dosadorArduinoDosagem.php";
const char* serverUrlConfig = "https://www.mexdistribuidora.com.br/api/dosadorArduinoConfiguracao.php";
const char* dosadorSerial = "DSA2024001";

Preferences preferences;
WebServer server(80);
DNSServer dnsServer;

String ssid;
String password;
String maquinaSerial;
int detergente = 0;
int dosagemSecante = 0;

const char* apSSID = "DOSADOR_MEX_AP";
const char* apPassword = "mexdesenv!2024";
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void saveToPreferences(const char* key, String value) {
  preferences.begin("dosador", false);
  preferences.putString(key, value);
  preferences.end();
}

String loadFromPreferences(const char* key, const char* defaultValue = "") {
  preferences.begin("dosador", true);
  String value = preferences.getString(key, defaultValue);
  preferences.end();
  return value;
}

void handleRoot() {
  server.send(200, "text/html",
    "<!DOCTYPE html>"
    "<html lang='pt-BR'>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<title>Configuração do Dosador</title>"
    "<style>"
    "body {"
    "  font-family: Arial, sans-serif;"
    "  margin: 0;"
    "  padding: 0;"
    "  display: flex;"
    "  align-items: center;"
    "  justify-content: center;"
    "  height: 100vh;"
    "  background-color: #f4f4f9;"
    "}"
    "form {"
    "  background: #fff;"
    "  padding: 20px;"
    "  border-radius: 8px;"
    "  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);"
    "  max-width: 400px;"
    "  width: 100%;"
    "}"
    "input[type='text'], input[type='password'], input[type='number'] {"
    "  width: 100%;"
    "  padding: 10px;"
    "  margin: 10px 0;"
    "  border: 1px solid #ccc;"
    "  border-radius: 4px;"
    "  font-size: 16px;"
    "}"
    "input[type='submit'] {"
    "  background-color: #007BFF;"
    "  color: white;"
    "  border: none;"
    "  padding: 10px 15px;"
    "  font-size: 16px;"
    "  border-radius: 4px;"
    "  cursor: pointer;"
    "}"
    "input[type='submit']:hover {"
    "  background-color: #0056b3;"
    "}"
    "h2 {"
    "  text-align: center;"
    "  margin-bottom: 20px;"
    "  color: #333;"
    "}"
    "</style>"
    "</head>"
    "<body>"
    "<form action='/save' method='POST'>"
    "<h2>Configuração do Dosador</h2>"
    "<label for='ssid'>SSID:</label>"
    "<input id='ssid' name='ssid' type='text' placeholder='Digite o nome da rede Wi-Fi' required><br>"
    "<label for='password'>Senha:</label>"
    "<input id='password' name='password' type='password' placeholder='Digite a senha da rede Wi-Fi' required><br>"
    "<label for='maquinaSerial'>Número de série:</label>"
    "<input id='maquinaSerial' name='maquinaSerial' type='text' placeholder='Digite o número de série' required><br>"
    "<label for='detergente'>Dosagem Detergente:</label>"
    "<input id='detergente' name='detergente' type='number' min='1' placeholder='Digite a dosagem de detergente' required><br>"
    "<label for='dosagemSecante'>Dosagem Secante:</label>"
    "<input id='dosagemSecante' name='dosagemSecante' type='number' min='1' placeholder='Digite a dosagem de secante' required><br>"
    "<input type='submit' value='Salvar'>"
    "</form>"
    "</body>"
    "</html>");
}

void handleSave() {
  ssid = server.arg("ssid");
  password = server.arg("password");
  maquinaSerial = server.arg("maquinaSerial");
  dosagemSecante = server.arg("dosagemSecante").toInt();
  detergente = server.arg("detergente").toInt();

  saveToPreferences("ssid", ssid);
  saveToPreferences("password", password);
  saveToPreferences("maquinaSerial", maquinaSerial);
  saveToPreferences("dosagemSecante", String(dosagemSecante));
  saveToPreferences("detergente", String(detergente));

  server.send(200, "text/html",
  "<!DOCTYPE html>"
  "<html lang='pt-BR'>"
  "<head>"
  "<meta charset='UTF-8'>"
  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
  "<title>Configuração Salva</title>"
  "<style>"
  "body {"
  "  font-family: Arial, sans-serif;"
  "  margin: 0;"
  "  padding: 0;"
  "  display: flex;"
  "  align-items: center;"
  "  justify-content: center;"
  "  height: 100vh;"
  "  background-color: #f4f4f9;"
  "  text-align: center;"
  "  color: #333;"
  "}"
  ".message {"
  "  background: #fff;"
  "  padding: 20px;"
  "  border-radius: 8px;"
  "  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);"
  "  max-width: 400px;"
  "  width: 90%;"
  "}"
  "h1 {"
  "  font-size: 24px;"
  "  margin-bottom: 10px;"
  "}"
  "p {"
  "  font-size: 16px;"
  "  margin-top: 0;"
  "}"
  "</style>"
  "</head>"
  "<body>"
  "<div class='message'>"
  "<h1>Configuração Salva!</h1>"
  "<p>O dispositivo será reiniciado em instantes.</p>"
  "</div>"
  "</body>"
  "</html>");
  
  delay(3000);
  ESP.restart();
}

void handleNotFound() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "Redirecionando...");
}

void startLocalServer() {
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.onNotFound(handleNotFound);
  server.begin();
}

void sendDataToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String requestBody = "{\"num_serie\":\"" + String(maquinaSerial) + "\",\"dosagem_secante\":" + String(dosagemSecante) +
                         ",\"dosagem_detergente\":" + String(detergente) + ",\"contador\":1""\}";

    Serial.println("Enviando dados:");
    Serial.println(requestBody);

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    } else {
      Serial.printf("Erro ao enviar dados: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Wi-Fi desconectado. Verifique sua conexão.");
  }
}

void sendConfigToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String requestBody = "{\"num_serie_maquina\":\"" + String(maquinaSerial) + 
                         "\",\"dosagem_secante\":" + String(dosagemSecante) + 
                         ",\"dosagem_detergente\":" + String(detergente) + 
                         ",\"num_serie_dosador\":\"Mxd20240001\"}";

    Serial.println("Enviando dados:");
    Serial.println(requestBody);

    http.begin(serverUrlConfig);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    } else {
      Serial.printf("Erro ao enviar dados: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Wi-Fi desconectado. Verifique sua conexão.");
  }
}

void split(const String& str, char delimiter, String result[], int maxTokens) {
  int start = 0;
  int end = str.indexOf(delimiter);
  int index = 0;

  while (end != -1 && index < maxTokens) {
    result[index++] = str.substring(start, end);
    start = end + 1;
    end = str.indexOf(delimiter, start);
  }
  result[index] = str.substring(start);
}

void getDosagem() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String requestBody = "{\"num_serie_dosador\":\"Mxd20240001\"}"; 

    Serial.println("Enviando dados:");
    Serial.println(requestBody);

    http.begin(serverUrlDosagem);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);

      String result[3];
      split(payload, ',', result, 3);

      String numeroSerie = result[0];
      int secanteNovo = result[1].toInt();
      int detergenteNovo = result[2].toInt();

      saveToPreferences("maquinaSerial", numeroSerie);
      saveToPreferences("dosagemSecante", String(secanteNovo));
      saveToPreferences("detergente", String(detergenteNovo));
  
      ajustarPotenciometro(secanteNovo);
      Serial.println(secanteNovo);
      Serial.println(detergenteNovo);
      Serial.println(potValue);

    }
  } else {
    Serial.println("Wi-Fi desconectado. Verifique sua conexão.");
  }
}

void ajustarPotenciometro(int novoValor) {
  int valueNEW = map(novoValor, 0, 100, 0, 255);

  if (valueNEW > valueOLD) {
    digitalWrite(U_D_PIN, HIGH);
  } else if (valueNEW < valueOLD) {
    digitalWrite(U_D_PIN, LOW);
  }

  for (int i = 0; i < abs(valueNEW - valueOLD); i++) {
    digitalWrite(CS_PIN, LOW);
    digitalWrite(INC_PIN, LOW);
    delay(10);
    digitalWrite(INC_PIN, HIGH);
    delay(10);
    digitalWrite(CS_PIN, HIGH);
  }

  valueOLD = valueNEW;
  potValue = valueNEW;

  EEPROM.write(0, potValue);
  EEPROM.commit();  

  Serial.print("Novo valor ajustado: ");
  Serial.println(novoValor);

  analogWrite(MOTOR_PIN, potValue);  
}

void setup() {
  EEPROM.begin(512);

  potValue = EEPROM.read(0);
  valueOLD = potValue;

  pinMode(INC_PIN, OUTPUT);
  pinMode(U_D_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT); 

  pinMode(PINO_LV, INPUT_PULLDOWN); 

  Serial.begin(115200);

  analogWrite(MOTOR_PIN, 0);  

  ssid = loadFromPreferences("ssid", defaultSSID);
  password = loadFromPreferences("password", defaultPassword);
  maquinaSerial = loadFromPreferences("maquinaSerial", dosadorSerial);

  dosagemSecante = loadFromPreferences("dosagemSecante").toInt();
  detergente = loadFromPreferences("detergente").toInt();
  
  WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFalha na conexão! Configurando como Access Point...");
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(apSSID, apPassword);

    dnsServer.start(53, "*", apIP);
    Serial.println("Captive Portal ativo.");
    EEPROM.write(CONFIG_SENT_FLAG_ADDR, 0);
    startLocalServer();
  } else {
    Serial.println("Conectado ao Wi-Fi!");

    if (EEPROM.read(CONFIG_SENT_FLAG_ADDR) != 1) {
      sendConfigToServer();
      EEPROM.write(CONFIG_SENT_FLAG_ADDR, 1);
      EEPROM.commit();
    }
  }
}

void loop() {
  int estado = digitalRead(PINO_LV);
  if (estado == HIGH) {
    if (!requisicaoFeita){
      analogWrite(MOTOR_PIN, dosagemSecante);
      Serial.println(dosagemSecante);
      potValue = dosagemSecante;
      Serial.println("High");
      requisicaoFeita = true;
      Serial.println("Entrou");
      if (WiFi.status() == WL_CONNECTED){
        getDosagem();
        Serial.println("Enviado getDosagem");
        delay(3000);
        sendDataToServer();
        Serial.println("Enviado sendDataToServer");
      }
    }
  } else {
    if (requisicaoFeita) {
      requisicaoFeita = false;
      analogWrite(MOTOR_PIN, 0);  
      Serial.println("Desligado");
    }
  }
  delay(100);
  if (WiFi.status() != WL_CONNECTED) {
    dnsServer.processNextRequest(); 
    server.handleClient();          
  }
}