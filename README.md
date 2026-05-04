# CLYVO VET — Sistema de Triagem IoT

Projeto desenvolvido para o Challenge FIAP 2026 em parceria com a Clyvo Vet. O sistema integra hardware, visão computacional e conectividade para triagem veterinária preliminar.

## Sobre o Projeto
Sistema de triagem veterinária doméstica utilizando ESP32, que auxilia o responsável pelo pet a identificar o nível de risco do animal através de uma lógica de pontuação e detecção de sintomas críticos. O projeto agora conta com validação por imagem e monitoramento remoto via dashboard.

## Arquitetura do Sistema
O ecossistema é composto por três camadas integradas:
1. **Visão Computacional (AI):** Validação de presença do pet via script Python antes do início da triagem.
2. **Hardware IoT (ESP32):** Interface física para coleta de dados clínicos e indicação visual por LEDs e LCD.
3. **Dashboard Web:** Servidor local (HTTP) que transmite os dados da triagem para visualização em navegadores.

## Módulo de Visão Computacional (AI)
Localizado no diretório `/Clyvo_IA`, o módulo utiliza processamento de imagem para identificar o paciente.

### Requisitos
- Python 3.x
- OpenCV (`pip install opencv-python`)

### Execução
1. Acesse o diretório via terminal:
   `cd Clyvo_IA`
2. Execute o script:
   `python main.py`

O sistema processará o arquivo `pet.jpg` e exibirá a confirmação de detecção na tela.

## Conectividade e Dashboard
O ESP32 atua como um servidor Web na rede local. Ao iniciar o hardware, o endereço IP de acesso será exibido no Serial Monitor.

### Acesso ao Dashboard:
1. Verifique o IP gerado no terminal do simulador.
2. Acesse o endereço no navegador para visualizar o status em tempo real.
3. O dashboard exibe: Nível de Risco, Pontuação e Status do Protocolo.

## Componentes Utilizados
- ESP32
- LCD 16x2 (I2C)
- 3 LEDs (Verde, Amarelo, Vermelho)
- 2 Push Buttons
- Resistores 220Ω e 10kΩ

## Simulação no Wokwi
Acesse o ambiente de simulação completo através do link abaixo:
[Clique aqui para acessar o projeto no Wokwi](https://wokwi.com/projects/462584624524506113)

## Equipe
- Enrico Delesporte RM: 565760
- Felipe Kirschner Modesto RM: 561810
- Vitor Dias dos Santos RM: 565422
