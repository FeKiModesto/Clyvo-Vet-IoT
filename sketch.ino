// BIBLIOTECAS
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// CONFIGURACAO DO HARDWARE
#define BTN_SIM     18
#define BTN_NAO     19
#define LED_VERDE   25
#define LED_AMARELO 26
#define LED_VERM    27

// CONFIGURACAO DO WIFI E SERVIDOR
const char* SSID     = "Wokwi-GUEST";
const char* PASSWORD = "";
WebServer server(80);

// CONFIGURACAO DO LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// PERGUNTAS DA TRIAGEM
const char* perguntas[] = {
  "Pet comendo?",
  "Bebendo agua?",
  "Movimentando?",
  "Teve vomito?",
  "Teve diarreia?",
  "Tem febre?"
};
const int TOTAL_PERGUNTAS = 6;

// VARIAVEIS DE CONTROLE
int    perguntaAtual     = 0;
int    pontuacao         = 0;
bool   triagemFinalizada = false;
bool   teveVomito        = false;
bool   teveDiarreia      = false;
String resultadoFinal    = "Aguardando...";
String nivelRisco        = "aguardando";

unsigned long ultimoDebounce = 0;
const unsigned long DEBOUNCE_DELAY = 300;

// LIGA E DESLIGA OS LEDS
void acenderLed(int led) {
  digitalWrite(LED_VERDE,   LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERM,    LOW);
  digitalWrite(led, HIGH);
}

// HELPER PARA MONTAR STRINGS JSON COM SEGURANCA
String jsonStr(String s) {
  s.replace("\"", "\\\"");
  return "\"" + s + "\"";
}

// CRIA A PAGINA DO DASHBOARD
void handleRoot() {
  String cor = "#27ae60";
  if (nivelRisco == "medio")      cor = "#f39c12";
  if (nivelRisco == "emergencia") cor = "#e74c3c";

  String html = "<html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='3'>";
  html += "<title>CLYVO VET - Dashboard</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background: #f4f4f4; padding: 30px; }";
  html += "h1 { color: #2c3e50; }";
  html += ".card { background: white; border-radius: 8px; padding: 20px; margin: 10px auto; max-width: 400px; box-shadow: 0 2px 6px rgba(0,0,0,0.1); }";
  html += ".status { font-size: 1.3em; font-weight: bold; color: " + cor + "; }";
  html += ".info { color: #555; margin-top: 8px; }";
  html += "</style></head>";
  html += "<body>";
  html += "<h1>🐾 CLYVO VET</h1>";
  html += "<p>Sistema de Triagem Veterinária IoT</p>";
  html += "<div class='card'>";
  html += "<div class='status'>" + resultadoFinal + "</div>";
  html += "<div class='info'>Pontuação de risco: " + String(pontuacao) + "</div>";
  html += "<div class='info'>Pergunta: " + String(perguntaAtual) + " / " + String(TOTAL_PERGUNTAS) + "</div>";
  html += "</div>";
  html += "<p style='color:#aaa; font-size:0.8em;'>Atualiza a cada 3 segundos</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// RETORNA O STATUS COMPLETO DA TRIAGEM EM JSON (Lab07)
void handleApiStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String json = "{";
  json += "\"finalizada\":"     + String(triagemFinalizada ? "true" : "false") + ",";
  json += "\"perguntaAtual\":"  + String(perguntaAtual) + ",";
  json += "\"totalPerguntas\":" + String(TOTAL_PERGUNTAS) + ",";
  json += "\"pontuacao\":"      + String(pontuacao) + ",";
  json += "\"nivelRisco\":"     + jsonStr(nivelRisco) + ",";
  json += "\"resultado\":"      + jsonStr(resultadoFinal) + ",";
  json += "\"vomito\":"         + String(teveVomito   ? "true" : "false") + ",";
  json += "\"diarreia\":"       + String(teveDiarreia ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

// REINICIA A TRIAGEM VIA HTTP
void handleApiReset() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  reiniciarTriagem();
  server.send(200, "application/json", "{\"ok\":true,\"message\":\"Triagem reiniciada\"}");
}

// MOSTRA A PERGUNTA ATUAL NO LCD
void mostrarPergunta() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(perguntaAtual + 1);
  lcd.print("/");
  lcd.print(TOTAL_PERGUNTAS);
  lcd.print(" ");
  lcd.print(perguntas[perguntaAtual]);
  lcd.setCursor(0, 1);
  lcd.print("S:Verde N:Verm");
}

// CALCULA OS PONTOS E MOSTRA O RESULTADO FINAL
void mostrarResultado() {
  lcd.clear();
  triagemFinalizada = true;

  if (teveVomito && teveDiarreia) {
    nivelRisco     = "emergencia";
    resultadoFinal = "EMERGENCIA (Combo detectado)";
    lcd.setCursor(0, 0); lcd.print("EMERGENCIA!");
    lcd.setCursor(0, 1); lcd.print("Vomito+Diarreia");
    acenderLed(LED_VERM);
  } else if (pontuacao <= 1) {
    nivelRisco     = "baixo";
    resultadoFinal = "Risco Baixo - Pet Saudavel";
    lcd.setCursor(0, 0); lcd.print("Risco: BAIXO");
    lcd.setCursor(0, 1); lcd.print("Pet saudavel!");
    acenderLed(LED_VERDE);
  } else if (pontuacao <= 3) {
    nivelRisco     = "medio";
    resultadoFinal = "Risco Medio - Consulte Breve";
    lcd.setCursor(0, 0); lcd.print("Risco: MEDIO");
    lcd.setCursor(0, 1); lcd.print("Consulte breve");
    acenderLed(LED_AMARELO);
  } else {
    nivelRisco     = "emergencia";
    resultadoFinal = "EMERGENCIA - Va ao Vet AGORA!";
    lcd.setCursor(0, 0); lcd.print("EMERGENCIA!");
    lcd.setCursor(0, 1); lcd.print("Va ao vet AGORA!");
    acenderLed(LED_VERM);
  }
}

void reiniciarTriagem() {
  perguntaAtual     = 0;
  pontuacao         = 0;
  teveVomito        = false;
  teveDiarreia      = false;
  triagemFinalizada = false;
  nivelRisco        = "aguardando";
  resultadoFinal    = "Triagem em andamento...";
  acenderLed(LED_VERDE);
  mostrarPergunta();
}

void setup() {
  Serial.begin(115200);

  pinMode(BTN_SIM,     INPUT_PULLUP);
  pinMode(BTN_NAO,     INPUT_PULLUP);
  pinMode(LED_VERDE,   OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERM,    OUTPUT);

  lcd.init();
  lcd.backlight();

  WiFi.begin(SSID, PASSWORD, 6);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP do Dashboard: http://");
  Serial.println(WiFi.localIP());

  server.on("/",           HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.on("/api/reset",  HTTP_GET, handleApiReset);
  server.begin();
  Serial.println("Servidor HTTP iniciado.");

  lcd.setCursor(0, 0); lcd.print("  CLYVO VET");
  lcd.setCursor(0, 1); lcd.print(" Triagem Pet");
  delay(2000);

  reiniciarTriagem();
}

void loop() {
  server.handleClient();

  unsigned long agora = millis();
  if (agora - ultimoDebounce < DEBOUNCE_DELAY) return;

  if (triagemFinalizada) {
    if (digitalRead(BTN_SIM) == LOW || digitalRead(BTN_NAO) == LOW) {
      ultimoDebounce = agora;
      reiniciarTriagem();
    }
    return;
  }

  if (digitalRead(BTN_SIM) == LOW) {
    ultimoDebounce = agora;
    if (perguntaAtual == 3) teveVomito   = true;
    if (perguntaAtual == 4) teveDiarreia = true;
    if (perguntaAtual >= 3) pontuacao++;
    perguntaAtual++;
    if (perguntaAtual >= TOTAL_PERGUNTAS) mostrarResultado();
    else mostrarPergunta();
  }

  if (digitalRead(BTN_NAO) == LOW) {
    ultimoDebounce = agora;
    if (perguntaAtual < 3) pontuacao++;
    perguntaAtual++;
    if (perguntaAtual >= TOTAL_PERGUNTAS) mostrarResultado();
    else mostrarPergunta();
  }

  delay(10);
}
