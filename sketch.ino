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
// SIM = ruim para idx 3-10 (sintomas)
// NAO = ruim para idx 0-2  (habitos)
const char* perguntas[] = {
  "Pet comendo?",     // idx 0 - NAO = ponto negativo
  "Bebendo agua?",    // idx 1 - NAO = ponto negativo
  "Movimentando?",    // idx 2 - NAO = ponto negativo
  "Teve vomito?",     // idx 3 - SIM = ponto negativo + flag
  "Teve diarreia?",   // idx 4 - SIM = ponto negativo + flag
  "Tem febre?",       // idx 5 - SIM = ponto negativo
  "Dif. respirar?",   // idx 6 - SIM = ponto negativo + flag
  "Convulsoes?",      // idx 7 - SIM = ponto negativo + flag
  "Urinando bem?",    // idx 8 - NAO = ponto negativo + flag
  "Barriga ok?",      // idx 9 - NAO = ponto negativo + flag
  "Coceira/feridas?"  // idx 10 - SIM = ponto negativo
};
const int TOTAL_PERGUNTAS = 11;

// VARIAVEIS DE CONTROLE
int    perguntaAtual     = 0;
int    pontuacao         = 0;
bool   triagemFinalizada = false;
String resultadoFinal    = "Aguardando...";
String nivelRisco        = "aguardando";

// FLAGS DE COMBOS
bool teveVomito    = false;
bool teveDiarreia  = false;
bool teveDifResp   = false;
bool teveConvulsao = false;
bool naoUrina      = false;
bool barigaRuim    = false;

unsigned long ultimoDebounce = 0;
const unsigned long DEBOUNCE_DELAY = 300;

// LIGA E DESLIGA OS LEDS
void acenderLed(int led) {
  digitalWrite(LED_VERDE,   LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERM,    LOW);
  digitalWrite(led, HIGH);
}

// HELPER JSON
String jsonStr(String s) {
  s.replace("\"", "\\\"");
  return "\"" + s + "\"";
}

// CONTA QUANTOS COMBOS ESTAO ATIVOS
int contarCombos() {
  int total = 0;
  if (teveVomito    && teveDiarreia) total++; // Gastroenterite
  if (teveVomito    && barigaRuim)   total++; // Torcao Gastrica
  if (teveConvulsao && teveDifResp)  total++; // Crise Neurologica
  if (naoUrina      && barigaRuim)   total++; // Obstr. Urinaria
  return total;
}

// DASHBOARD HTML
void handleRoot() {
  String cor = "#27ae60";
  if (nivelRisco == "medio")      cor = "#f39c12";
  if (nivelRisco == "alto")       cor = "#e67e22";
  if (nivelRisco == "emergencia") cor = "#e74c3c";
  if (nivelRisco == "critico")    cor = "#c0392b";

  String html = "<html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='3'>";
  html += "<title>CLYVO VET - Dashboard</title>";
  html += "<style>";
  html += "body{font-family:Arial,sans-serif;text-align:center;background:#f4f4f4;padding:30px;}";
  html += "h1{color:#2c3e50;}";
  html += ".card{background:white;border-radius:8px;padding:20px;margin:10px auto;max-width:400px;box-shadow:0 2px 6px rgba(0,0,0,0.1);}";
  html += ".status{font-size:1.3em;font-weight:bold;color:" + cor + ";}";
  html += ".info{color:#555;margin-top:8px;}";
  html += "</style></head><body>";
  html += "<h1>🐾 CLYVO VET</h1>";
  html += "<p>Sistema de Triagem Veterinária IoT</p>";
  html += "<div class='card'>";
  html += "<div class='status'>" + resultadoFinal + "</div>";
  html += "<div class='info'>Pontuação: " + String(pontuacao) + "</div>";
  html += "<div class='info'>Pergunta: " + String(perguntaAtual) + " / " + String(TOTAL_PERGUNTAS) + "</div>";
  html += "</div>";
  html += "<p style='color:#aaa;font-size:0.8em;'>Atualiza a cada 3 segundos</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// API JSON - STATUS
void handleApiStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String json = "{";
  json += "\"finalizada\":"     + String(triagemFinalizada ? "true" : "false") + ",";
  json += "\"perguntaAtual\":"  + String(perguntaAtual) + ",";
  json += "\"totalPerguntas\":" + String(TOTAL_PERGUNTAS) + ",";
  json += "\"pontuacao\":"      + String(pontuacao) + ",";
  json += "\"nivelRisco\":"     + jsonStr(nivelRisco) + ",";
  json += "\"resultado\":"      + jsonStr(resultadoFinal) + ",";
  json += "\"combosAtivos\":"   + String(contarCombos()) + ",";
  json += "\"vomito\":"         + String(teveVomito    ? "true" : "false") + ",";
  json += "\"diarreia\":"       + String(teveDiarreia  ? "true" : "false") + ",";
  json += "\"convulsao\":"      + String(teveConvulsao ? "true" : "false") + ",";
  json += "\"difResp\":"        + String(teveDifResp   ? "true" : "false") + ",";
  json += "\"semUrina\":"       + String(naoUrina      ? "true" : "false") + ",";
  json += "\"barigaRuim\":"     + String(barigaRuim    ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

// API JSON - RESET
void handleApiReset() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  reiniciarTriagem();
  server.send(200, "application/json", "{\"ok\":true,\"message\":\"Triagem reiniciada\"}");
}

// MOSTRA PERGUNTA NO LCD
void mostrarPergunta() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(perguntas[perguntaAtual]);
  lcd.setCursor(0, 1);
  lcd.print("S:Verde N:Verm");
}

// CALCULA E MOSTRA RESULTADO
void mostrarResultado() {
  lcd.clear();
  triagemFinalizada = true;
  int combos = contarCombos();

  // 1. SITUACAO CRITICA — quase tudo negativo
  if (pontuacao >= 8) {
    nivelRisco     = "critico";
    resultadoFinal = "Situacao critica!";
    lcd.setCursor(0, 0); lcd.print("SITUACAO CRITICA");
    lcd.setCursor(0, 1); lcd.print("Corra ao vet!!!");
    acenderLed(LED_VERM);

  // 2. MUITO URGENTE — muita coisa errada
  } else if (pontuacao >= 6) {
    nivelRisco     = "critico";
    resultadoFinal = "Muito urgente!";
    lcd.setCursor(0, 0); lcd.print("MUITO URGENTE!");
    lcd.setCursor(0, 1); lcd.print("Va ao vet AGORA!");
    acenderLed(LED_VERM);

  // 3. MAIS DE UM COMBO — sem nome, so urgencia
  } else if (combos > 1) {
    nivelRisco     = "emergencia";
    resultadoFinal = "Multiplos combos!";
    lcd.setCursor(0, 0); lcd.print("EMERGENCIA!");
    lcd.setCursor(0, 1); lcd.print("Va ao vet AGORA!");
    acenderLed(LED_VERM);

  // 4. EXATAMENTE UM COMBO — mostra nome da doenca
  } else if (combos == 1) {
    nivelRisco = "emergencia";
    acenderLed(LED_VERM);

    if (teveVomito && teveDiarreia) {
      resultadoFinal = "Gastroenterite!";
      lcd.setCursor(0, 0); lcd.print("Gastroenterite!");
      lcd.setCursor(0, 1); lcd.print("Va ao vet hoje!");

    } else if (teveVomito && barigaRuim) {
      resultadoFinal = "Torcao Gastrica?";
      lcd.setCursor(0, 0); lcd.print("Torcao Gastrica?");
      lcd.setCursor(0, 1); lcd.print("Va ao vet AGORA!");

    } else if (teveConvulsao && teveDifResp) {
      resultadoFinal = "Crise Neurologica";
      lcd.setCursor(0, 0); lcd.print("Crise Neurologica");
      lcd.setCursor(0, 1); lcd.print("Va ao vet AGORA!");

    } else if (naoUrina && barigaRuim) {
      resultadoFinal = "Obstr. Urinaria!";
      lcd.setCursor(0, 0); lcd.print("Obstr. Urinaria!");
      lcd.setCursor(0, 1); lcd.print("Va ao vet AGORA!");
    }

  // 5. RISCO BAIXO
  } else if (pontuacao <= 2) {
    nivelRisco     = "baixo";
    resultadoFinal = "Risco Baixo";
    lcd.setCursor(0, 0); lcd.print("Risco: BAIXO");
    lcd.setCursor(0, 1); lcd.print("Pet saudavel!");
    acenderLed(LED_VERDE);

  // 6. RISCO MEDIO
  } else {
    nivelRisco     = "medio";
    resultadoFinal = "Risco Medio";
    lcd.setCursor(0, 0); lcd.print("Risco: MEDIO");
    lcd.setCursor(0, 1); lcd.print("Consulte breve");
    acenderLed(LED_AMARELO);
  }
}

void reiniciarTriagem() {
  perguntaAtual     = 0;
  pontuacao         = 0;
  triagemFinalizada = false;
  nivelRisco        = "aguardando";
  resultadoFinal    = "Triagem em andamento...";
  teveVomito        = false;
  teveDiarreia      = false;
  teveDifResp       = false;
  teveConvulsao     = false;
  naoUrina          = false;
  barigaRuim        = false;
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
    if (perguntaAtual == 3)  teveVomito    = true;
    if (perguntaAtual == 4)  teveDiarreia  = true;
    if (perguntaAtual == 6)  teveDifResp   = true;
    if (perguntaAtual == 7)  teveConvulsao = true;
    if (perguntaAtual >= 3)  pontuacao++;
    perguntaAtual++;
    if (perguntaAtual >= TOTAL_PERGUNTAS) mostrarResultado();
    else mostrarPergunta();
  }

  if (digitalRead(BTN_NAO) == LOW) {
    ultimoDebounce = agora;
    if (perguntaAtual == 8)  naoUrina   = true;
    if (perguntaAtual == 9)  barigaRuim = true;
    if (perguntaAtual < 3)   pontuacao++;
    perguntaAtual++;
    if (perguntaAtual >= TOTAL_PERGUNTAS) mostrarResultado();
    else mostrarPergunta();
  }

  delay(10);
}
