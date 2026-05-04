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

const char* SSID = "Wokwi-GUEST";
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

int perguntaAtual = 0;
int pontuacao = 0;
bool triagemFinalizada = false;
bool teveVomito = false;
bool teveDiarreia = false;
String resultadoFinal = "Aguardando...";

unsigned long ultimoDebounce = 0;
const unsigned long DEBOUNCE_DELAY = 300;

// LIGA E DESLIGA OS LEDS
void acenderLed(int led) {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERM, LOW);
  digitalWrite(led, HIGH);
}

// CRIA A PAGINA DO DASHBOARD
void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'><title>CLYVO VET - DASHBOARD</title></head>";
  html += "<body style='font-family: Arial; text-align: center;'>";
  html += "<h1>Triagem Pet - Dashboard</h1>";
  html += "<h3>Status Atual: " + resultadoFinal + "</h3>";
  html += "<p>Pontuacao de risco: " + String(pontuacao) + "</p>";
  html += "<script>setTimeout(function(){location.reload();}, 3000);</script>"; // Atualiza a cada 3s
  html += "</body></html>";
  server.send(200, "text/html", html);
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
    resultadoFinal = "EMERGENCIA (Combo detectado)";
    lcd.setCursor(0, 0);
    lcd.print("EMERGENCIA!");
    lcd.setCursor(0, 1);
    lcd.print("Vomito+Diarreia");
    acenderLed(LED_VERM);
  } else if (pontuacao <= 1) {
    resultadoFinal = "Risco Baixo - Pet Saudavel";
    lcd.setCursor(0, 0);
    lcd.print("Risco: BAIXO");
    lcd.setCursor(0, 1);
    lcd.print("Pet saudavel!");
    acenderLed(LED_VERDE);
  } else if (pontuacao <= 3) {
    resultadoFinal = "Risco Medio - Consulte Breve";
    lcd.setCursor(0, 0);
    lcd.print("Risco: MEDIO");
    lcd.setCursor(0, 1);
    lcd.print("Consulte breve");
    acenderLed(LED_AMARELO);
  } else {
    resultadoFinal = "EMERGENCIA - Va ao Vet AGORA!";
    lcd.setCursor(0, 0);
    lcd.print("EMERGENCIA!");
    lcd.setCursor(0, 1);
    lcd.print("Va ao vet AGORA!");
    acenderLed(LED_VERM);
  }
}

void reiniciarTriagem() {
  perguntaAtual = 0;
  pontuacao = 0;
  teveVomito = false;
  teveDiarreia = false;
  triagemFinalizada = false;
  resultadoFinal = "Triagem em andamento...";
  acenderLed(LED_VERDE);
  mostrarPergunta();
}

void setup() {
  Serial.begin(115200);

  pinMode(BTN_SIM, INPUT_PULLUP);
  pinMode(BTN_NAO, INPUT_PULLUP);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERM, OUTPUT);

  lcd.init();
  lcd.backlight();

  // INICIA CONEXAO WIFI
  WiFi.begin(SSID, PASSWORD, 6);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP do Dashboard: ");
  Serial.println(WiFi.localIP());

  // CONFIGURA AS ROTAS DO SERVIDOR
  server.on("/", handleRoot);
  server.begin();

  lcd.setCursor(0, 0);
  lcd.print("  CLYVO VET");
  lcd.setCursor(0, 1);
  lcd.print(" Triagem Pet");
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
    if (perguntaAtual == 3) teveVomito = true;
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
