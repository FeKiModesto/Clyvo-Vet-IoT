# CLYVO VET — Sistema de Triagem Veterinária IoT

Projeto desenvolvido para o Challenge FIAP 2026 em parceria com a Clyvo Vet.  
Disciplina: Disruptive Architectures: IoT, IoB & Generative IA.

---

## Sobre o Projeto

A jornada de saúde dos pets é fragmentada — o tutor só leva o animal ao veterinário em emergências. Pensando nisso, desenvolvemos um ecossistema completo de triagem veterinária doméstica composto por três camadas integradas: visão computacional, hardware IoT e dashboard web.

---

## Arquitetura do Sistema

1. **Visão Computacional:** Validação de presença do pet via script Python com OpenCV antes do início da triagem.
2. **Hardware IoT (ESP32):** Interface física para coleta de dados clínicos e indicação visual por LEDs e LCD.
3. **Dashboard Web:** Servidor HTTP integrado ao ESP32 que transmite os dados da triagem em tempo real, incluindo uma API REST com endpoints JSON.

---

## Como Funciona

O sistema realiza 11 perguntas sobre o comportamento e sintomas do pet. Com base nas respostas, calcula um nível de risco, detecta combinações de sintomas preocupantes (combos) e exibe o resultado no LCD com indicação visual por LED.

### Tabela de Risco

| Condição | Resultado | LED |
|---|---|---|
| Pontuação ≥ 8 | Situação Crítica — Corra ao vet!!! | 🔴 Vermelho |
| Pontuação ≥ 6 | Muito Urgente — Vá ao vet AGORA! | 🔴 Vermelho |
| Múltiplos combos | Emergência — Vá ao vet AGORA! | 🔴 Vermelho |
| 1 combo detectado | Nome da doença + orientação | 🔴 Vermelho |
| Pontuação 3–5 | Risco Médio — Consulte breve | 🟡 Amarelo |
| Pontuação 0–2 | Risco Baixo — Pet saudável! | 🟢 Verde |

### Combos de Sintomas

Quando exatamente um combo é detectado, o sistema exibe o nome clínico da condição:

| Combo | Condição |
|---|---|
| Vômito + Diarreia | Gastroenterite |
| Vômito + Barriga não ok | Torção Gástrica |
| Convulsões + Dif. respirar | Crise Neurológica |
| Não urina + Barriga não ok | Obstrução Urinária |

---

## API REST

O ESP32 serve os seguintes endpoints HTTP:

| Método | Endpoint | Descrição |
|---|---|---|
| GET | `/` | Dashboard HTML da triagem |
| GET | `/api/status` | Estado completo da triagem em JSON |
| GET | `/api/reset` | Reinicia a triagem remotamente |

Exemplo de resposta do `GET /api/status`:
```json
{
  "finalizada": true,
  "perguntaAtual": 11,
  "totalPerguntas": 11,
  "pontuacao": 3,
  "nivelRisco": "emergencia",
  "resultado": "Gastroenterite!",
  "combosAtivos": 1,
  "vomito": true,
  "diarreia": true,
  "convulsao": false,
  "difResp": false,
  "semUrina": false,
  "barigaRuim": false
}
```

### Dashboard Web

O dashboard e a API REST estão implementados no `sketch.ino` e funcionam corretamente durante a simulação. O acesso via browser requer licença do Wokwi for VS Code ou hardware ESP32 físico.

Para visualizar, após iniciar a simulação:
1. Verifique o IP gerado no **Serial Monitor**
2. Acesse `http://<IP>` no navegador
3. O dashboard atualiza automaticamente a cada 3 segundos
4. Os endpoints podem ser testados via Postman

---

## Como Executar

### Visão Computacional

1. Acesse o diretório:
```bash
cd Clyvo_IA
```
2. Instale a dependência:
```bash
pip install opencv-python
```
3. Execute com qualquer imagem da pasta:
```bash
python main.py pet.jpg
python main.py peixoto.jpg
python main.py pet_lorito_elegante.jpg
python main.py PrEppY_MoUsE.jpg
python main.py pet_happyDog.jpg
```

### Hardware IoT

1. Acesse o projeto no Wokwi pelo link abaixo
2. Clique em **Play** (▶) para iniciar a simulação
3. Aguarde a tela de boas-vindas **CLYVO VET**
4. Use o **botão verde** para responder **SIM**
5. Use o **botão vermelho** para responder **NÃO**
6. Ao final das 11 perguntas, veja o resultado no LCD
7. Pressione qualquer botão para reiniciar a triagem

[Clique aqui para acessar o projeto no Wokwi](https://wokwi.com/projects/462584624524506113)

---

## Demonstração em Vídeo

Em breve.

---

## Estrutura do Repositório
```
Clyvo-Vet-IoT/
├── Clyvo_IA/
│   ├── main.py              → Script de visão computacional
│   ├── pet.jpg              → Imagem de teste (gato)
│   ├── peixoto.jpg          → Imagem de teste (peixe)
│   ├── pet_lorito_elegante.jpg  → Imagem de teste (periquito)
│   ├── PrEppY_MoUsE.jpg     → Imagem de teste (hamster)
│   └── pet_happyDog.jpg         → Imagem de teste (cachorro)
├── docs/
│   └── componentes.md       → Detalhes dos componentes
├── diagram.json             → Diagrama do circuito no Wokwi
├── wokwi.toml               → Configuração do Wokwi for VS Code
├── sketch.ino               → Código principal do ESP32
└── README.md                → Documentação principal
```

---

## Equipe

| Nome | RM |
|---|---|
| Enrico Delesporte | 565760 |
| Felipe Kirschner Modesto | 561810 |
| Vitor Dias dos Santos | 565422 |

---

## Informações Acadêmicas

- **Instituição:** FIAP
- **Curso:** Análise e Desenvolvimento de Sistemas
- **Disciplina:** Disruptive Architectures: IoT, IoB & Generative IA
- **Ano:** 2026
