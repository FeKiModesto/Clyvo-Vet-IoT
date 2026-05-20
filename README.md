# CLYVO VET — Sistema de Triagem IoT

Projeto desenvolvido para o Challenge FIAP 2026 em parceria com a Clyvo Vet.
Disciplina: Disruptive Architectures: IoT, IoB & Generative IA.

---

## Sobre o Projeto

A jornada de saúde dos pets é fragmentada — o tutor só leva
o animal ao veterinário em emergências. Pensando nisso,
desenvolvemos um ecossistema completo de triagem veterinária
doméstica composto por três camadas integradas: visão
computacional, hardware IoT e dashboard web.

---

## Arquitetura do Sistema

1. **Visão Computacional:** Validação de presença do pet
   via script Python com OpenCV antes do início da triagem.
2. **Hardware IoT (ESP32):** Interface física para coleta
   de dados clínicos e indicação visual por LEDs e LCD.
3. **Dashboard Web:** Servidor HTTP integrado ao ESP32 que
   transmite os dados da triagem em tempo real para
   visualização em qualquer navegador.

---

## Como Funciona

O sistema faz 6 perguntas sobre o comportamento do pet.
Com base nas respostas, calcula um nível de risco e exibe
o resultado no LCD com indicação visual por LED.

| Pontuação | Resultado | LED |
|-----------|-----------|-----|
| Vômito + Diarreia | Emergência — Combo detectado | 🔴 Vermelho |
| 0 a 1 | Risco Baixo — Pet saudável | 🟢 Verde |
| 2 a 3 | Risco Médio — Consulte em breve | 🟡 Amarelo |
| 4 a 6 | Emergência — Vá ao vet AGORA! | 🔴 Vermelho |

---

## Como Executar

### Visão Computacional
1. Acesse o diretório via terminal:
```bash
cd Clyvo_IA
```

2. Instale a dependência:
```bash
pip install opencv-python
```

3. Execute o script:
```bash
python main.py
```

### Hardware IoT
1. Acesse o projeto no Wokwi pelo link abaixo
2. Clique no botão **Play** (▶) para iniciar
3. Aguarde a tela de boas vindas **CLYVO VET**
4. Use o **botão verde** para responder **SIM**
5. Use o **botão vermelho** para responder **NÃO**
6. Ao final das 6 perguntas, veja o resultado no LCD
7. Pressione qualquer botão para reiniciar a triagem

### Dashboard Web
1. Inicie a simulação no Wokwi
2. Verifique o IP gerado no **Serial Monitor**
3. Acesse o endereço no navegador
4. O dashboard atualiza automaticamente a cada 3 segundos

[Clique aqui para acessar o projeto no Wokwi](https://wokwi.com/projects/462584624524506113)

---

## Estrutura do Repositório
```
Clyvo-Vet-IoT/
├── sketch.ino          → Código principal do ESP32
├── diagram.json        → Diagrama do circuito no Wokwi
├── README.md           → Documentação principal
├── Clyvo_IA/
│   ├── main.py         → Script de visão computacional
│   └── pet.jpg         → Imagem de teste
└── docs/
└── componentes.md  → Detalhes dos componentes
```

---

## Demonstração em Vídeo

Link do vídeo no YouTube em breve.

---

## Equipe

| Nome | RM |
|------|----|
| Enrico Delesporte | 565760 |
| Felipe Kirschner Modesto | 561810 |
| Vitor Dias dos Santos | 565422 |

---

## Informações Acadêmicas

- **Instituição:** FIAP
- **Curso:** Análise e Desenvolvimento de Sistemas
- **Disciplina:** Disruptive Architectures: IoT, IoB & Generative IA
- **Ano:** 2026
