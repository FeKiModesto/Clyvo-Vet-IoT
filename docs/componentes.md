# Componentes Utilizados

## ESP32
- **Função:** Cérebro do sistema
- **Descrição:** Microcontrolador responsável por processar
  todas as informações, ler os botões e controlar o LCD e LEDs
- **Pinos utilizados:** GPIO18, GPIO19, GPIO21, GPIO22,
  GPIO25, GPIO26, GPIO27

---

## LCD 16x2 (I2C)
- **Função:** Display de informações
- **Descrição:** Exibe as perguntas da triagem e o resultado
  final para o usuário. Comunicação via protocolo I2C
- **Pinos:** SDA → GPIO21, SCL → GPIO22
- **Alimentação:** 3.3V

---

## LED Verde
- **Função:** Indicador visual de risco baixo
- **Descrição:** Acende quando o pet está saudável
- **Pino:** GPIO25
- **Resistor:** 220Ω

---

## LED Amarelo
- **Função:** Indicador visual de risco médio
- **Descrição:** Acende quando o pet precisa de atenção
- **Pino:** GPIO26
- **Resistor:** 220Ω

---

## LED Vermelho
- **Função:** Indicador visual de emergência
- **Descrição:** Acende quando o pet precisa ir ao
  veterinário imediatamente
- **Pino:** GPIO27
- **Resistor:** 220Ω

---

## Botão Verde (SIM)
- **Função:** Resposta positiva
- **Descrição:** Usado pelo tutor para responder SIM
  nas perguntas da triagem
- **Pino:** GPIO18
- **Resistor:** 10kΩ (pull-down)

---

## Botão Vermelho (NÃO)
- **Função:** Resposta negativa
- **Descrição:** Usado pelo tutor para responder NÃO
  nas perguntas da triagem
- **Pino:** GPIO19
- **Resistor:** 10kΩ (pull-down)

---

## Resistores 220Ω
- **Quantidade:** 3
- **Função:** Limitar a corrente dos LEDs,
  protegendo os componentes

---

## Resistores 10kΩ
- **Quantidade:** 2
- **Função:** Pull-down dos botões, garantindo
  leitura estável dos pinos
