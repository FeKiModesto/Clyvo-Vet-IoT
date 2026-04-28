// BIBLIOTECAS

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// CONFIGURACAO DO HARDWARE

#define BTN_SIM     18
#define BTN_NAO     19
#define LED_VERDE   25
#define LED_AMARELO 26
#define LED_VERM    27

// CONFIGURACAO DO LCD

LiquidCrystal_I2C lcd(0x27, 16, 2);

// PERGUNTAS DA TRIAGEM

const char* perguntas[] = {
  "Pet comendo?",
  "Bebendo agua?",
  "Se movimenta?",
  "Vomito/Diarreia?",
  "Temperatura alta?"
};
const int TOTAL_PERGUNTAS = 5;

// VARIAVEIS DE CONTROLE

int perguntaAtual = 0;
int pontuacao = 0;
bool triagemFinalizada = false;

unsigned long ultimoDebounce = 0;
const unsigned long DEBOUNCE_DELAY = 300;

// FUNCOES AUXILIARES

void acenderLed(int led) {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERM, LOW);
  digitalWrite(led, HIGH);
}
