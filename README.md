# TCC – Sistema Embarcado para Feedback Háptico em Sim Racing

Firmware para `ESP32` que recebe telemetria de simuladores de corrida via `UDP`,
processa os dados em tempo real e aciona:

- instrumentos físicos de velocidade, RPM, combustível e temperatura;
- um display `TFT ST7789` com múltiplos layouts;
- um fluxo embarcado de configuração Wi‑Fi por portal local.

O repositório foi organizado como base técnica do TCC e já contém o MVP
funcional do painel, com arquitetura modular, testes automatizados e
documentação técnica.

## Estado Atual

Hoje o firmware está preparado para:

- receber telemetria por `UDP` em rede local;
- decodificar o formato `Forza Motorsport 7 (Data Out)`;
- atualizar velocímetro, conta-giros, combustível e temperatura média dos pneus;
- exibir marcha, volta, posição, rede e status de telemetria no display;
- configurar Wi‑Fi diretamente pelo ESP32, sem regravar firmware.

Limites importantes do estado atual:

- o decoder ativo no código é o `Forza7Decoder`;
- a seleção do jogo ainda não é dinâmica;
- a integração UDP real no ESP32 depende de uma bancada elétrica estável para
  fechar a validação experimental completa.

## Estrutura do Projeto

```text
.
├── src/        Firmware principal
├── test/       Suítes automatizadas, doubles e fixtures
├── scripts/    Automação auxiliar
├── docs/       Documentação do TCC organizada por tema
├── include/    Configurações auxiliares
└── platformio.ini
```

Estrutura principal do firmware:

```text
src/
  composition/   Ponto de composição e inicialização
  application/   Orquestração, serviços e instrumentos
  domain/        Modelos e estado neutros
  ports/         Contratos entre aplicação e infraestrutura
  adapters/      Wi‑Fi, UDP, display, botões, motores e decoders
  config/        Pinos, limites, escalas e constantes da placa
```

Direção principal das dependências:

```text
composition -> application -> domain / ports
adapters -> ports
```

## Arquivos de Configuração Mais Importantes

Antes de compilar, revise estes arquivos:

- [src/config/BoardConfig.h](src/config/BoardConfig.h): porta UDP, pinos dos
  motores, botão, escalas dos instrumentos e velocidades de calibração.
- [include/User_Setup.h](include/User_Setup.h): configuração do display
  `ST7789` usada pela biblioteca `TFT_eSPI`.
- [src/composition/main.cpp](src/composition/main.cpp): composição do sistema,
  SSID do portal Wi‑Fi, decoder concreto registrado, layouts e instrumentos.
- [platformio.ini](platformio.ini): ambientes de build, teste e flags do projeto.

Se o seu hardware não for igual ao protótipo atual, o primeiro ajuste deve ser
feito em `BoardConfig.h` e `User_Setup.h`.

## Hardware Esperado

O código atual foi montado considerando:

- `ESP32 Dev Module`;
- display `TFT ST7789 240x320` via `TFT_eSPI`;
- quatro atuadores controlados por `TMC2208`;
- um botão de troca de layout no `GPIO 16`;
- telemetria chegando por `UDP` na porta `5300`.

Pinos usados por padrão:

- display: definidos em [include/User_Setup.h](include/User_Setup.h);
- motores e botão: definidos em [src/config/BoardConfig.h](src/config/BoardConfig.h).

## Build, Upload e Testes

Pré-requisitos:

- `PlatformIO CLI` ou extensão PlatformIO para VS Code;
- toolchain de C/C++ do ambiente do PlatformIO;
- placa compatível com `esp32dev`.

Comandos principais:

```bash
pio run -e esp32dev
pio run -e esp32dev -t upload
pio device monitor -b 115200

pio test -e test_native
pio test -e test_esp32_smoke
pio test -e test_esp32_udp_integration

./scripts/run_native_coverage.sh
```

Ambientes disponíveis em [platformio.ini](platformio.ini):

- `esp32dev`: build principal do firmware;
- `test_native`: testes host-side com `Unity`;
- `test_native_coverage`: testes host-side com cobertura;
- `test_esp32_smoke`: smoke test no ESP32;
- `test_esp32_udp_integration`: integração UDP real no ESP32.

## Como Usar o Firmware

### 1. Ajuste a configuração do hardware

Revise:

- [src/config/BoardConfig.h](src/config/BoardConfig.h) para pinos, escalas,
  velocidades, sentido dos motores e porta UDP;
- [include/User_Setup.h](include/User_Setup.h) para os pinos do display.

### 2. Compile e grave o firmware

```bash
pio run -e esp32dev
pio run -e esp32dev -t upload
pio device monitor -b 115200
```

### 3. Configure o Wi‑Fi do ESP32

No estado atual do código:

- o ESP32 tenta usar credenciais salvas;
- se falhar, sobe um ponto de acesso chamado `SimHub`;
- o SSID do portal é definido em [src/composition/main.cpp](src/composition/main.cpp);
- a senha do AP está desabilitada no código atual;
- após conectar ao AP, abra `http://simhub` e informe a rede local.

### 4. Configure a origem da telemetria

O firmware atual espera:

- transporte `UDP`;
- destino igual ao IP do ESP32 na sua rede local;
- porta `5300` por padrão;
- payload compatível com `Forza Motorsport 7 (Data Out / Dash)`;
- pacote com tamanho mínimo de `311` bytes, como validado no decoder atual.

Em outras palavras: subir o firmware e conectar ao Wi‑Fi não é suficiente. A
fonte externa de telemetria precisa enviar pacotes UDP no formato esperado pelo
decoder atual.

### 5. Verifique o comportamento do painel

Quando os pacotes válidos começarem a chegar, o firmware deve:

- atualizar o display com rede, marcha, velocidade, volta e posição;
- mover os gauges de velocidade e RPM continuamente;
- atualizar combustível;
- atualizar temperatura média dos pneus em janelas mais lentas;
- trocar layouts ao pressionar o botão físico configurado no `GPIO 16`.

## Compatibilidade com Novos Jogos

### O que já facilita a extensão

A base atual já separa bem a lógica de protocolo:

- [src/ports/ITelemetryDecoder.h](src/ports/ITelemetryDecoder.h) define o
  contrato do decoder;
- [src/application/services/TelemetryService.cpp](src/application/services/TelemetryService.cpp)
  consome qualquer implementação de `ITelemetryDecoder`;
- [src/composition/main.cpp](src/composition/main.cpp) injeta o decoder concreto;
- os instrumentos e a UI consomem dados normalizados, não o pacote bruto.

Se o novo jogo também enviar telemetria por `UDP`, a evolução tende a ser local.

### O que precisa ser alterado para suportar outro jogo

1. Criar um novo decoder em `src/adapters/telemetry/`, por exemplo
   `NovoJogoDecoder.h/.cpp`, implementando `ITelemetryDecoder`.
2. Mapear o pacote bruto do jogo para o contrato de
   [src/domain/TelemetryFrame.h](src/domain/TelemetryFrame.h).
3. Trocar a instância concreta em
   [src/composition/main.cpp](src/composition/main.cpp), onde hoje existe:
   `static Forza7Decoder decoder;`
4. Adicionar testes unitários e de integração para o novo decoder em `test/`.
5. Se o jogo usar outra porta UDP, ajustar
   [src/config/BoardConfig.h](src/config/BoardConfig.h).
6. Se o jogo não usar UDP, implementar um novo adaptador para
   [src/ports/IPacketReceiver.h](src/ports/IPacketReceiver.h).

### Contrato esperado hoje pelo restante do firmware

Ao implementar um novo decoder, os dados precisam chegar ao restante da
aplicação neste formato lógico:

- `speedKmh`: velocidade em km/h;
- `rpm`: rotação do motor;
- `gear`: marcha atual;
- `lapNumber`: volta atual;
- `racePosition`: posição na corrida;
- `fuel`: nível de combustível;
- `tireTempFL/FR/RL/RR` e `tireTempAvg`: temperaturas dos pneus.

O ponto importante é que o resto do firmware espera valores já convertidos para
um contrato coerente. Se o novo jogo enviar unidades diferentes, o decoder deve
normalizar isso ou o contrato interno precisa ser revisto.

### Acoplamentos atuais ao Forza que convém revisar

Hoje ainda existem alguns pressupostos específicos do `Forza Motorsport 7`:

- [src/composition/main.cpp](src/composition/main.cpp) instancia
  diretamente `Forza7Decoder`;
- [src/domain/TelemetryFrame.h](src/domain/TelemetryFrame.h) traz comentário de
  marcha reversa baseado no comportamento do Forza;
- [src/application/services/TelemetryService.cpp](src/application/services/TelemetryService.cpp)
  converte temperaturas de Fahrenheit para Celsius;
- os testes de protocolo e fixtures atuais são centrados no pacote do Forza.

Isso significa que um novo jogo pode exigir mais do que apenas "trocar offsets".
Se ele usar:

- outra unidade de temperatura;
- combustível em litros em vez de fração;
- convenção diferente para marcha;
- campos extras ou ausentes;

então o contrato interno precisará ser normalizado de forma explícita.

### Recomendação para compatibilidade multi-jogo de verdade

Se a intenção for suportar vários jogos de forma sustentável, o ideal é fazer
estes ajustes arquiteturais:

1. Definir `TelemetryFrame` como contrato normalizado e agnóstico de jogo.
   Exemplo: temperatura sempre em Celsius e combustível sempre em faixa
   conhecida.
2. Mover toda conversão específica de protocolo para o decoder.
   O `TelemetryService` deve apenas receber pacotes, aplicar timeout e expor o
   último frame válido.
3. Introduzir um conceito de perfil de jogo.
   Exemplo: `Forza7`, `Assetto Corsa`, `iRacing`, com seleção salva no ESP32.
4. Selecionar o decoder e a porta a partir desse perfil, em vez de fixar isso
   em `main.cpp`.
5. Manter uma suíte de teste por protocolo, com fixtures reais de cada jogo.

Sem isso, o projeto até consegue trocar de jogo manualmente, mas ainda fica
com suporte estático, um protocolo por build.

## Testes e Validação

O projeto possui `18` suítes de teste em `test/`, cobrindo:

- decoder do protocolo;
- serviço de telemetria;
- pipeline com receiver e decoder reais;
- robustez sob burst, descarte, timeout e recuperação;
- gauges;
- display;
- portal Wi‑Fi;
- smoke test embarcado;
- integração UDP real no ESP32.

Referências:

- [test/README](test/README)
- [Quadro Geral dos Testes](docs/validacao/testes/quadro-geral-dos-testes.md)
- [Rastreabilidade de Testes](docs/validacao/testes/rastreabilidade-de-testes.md)
- [Execução e Evidências](docs/validacao/testes/execucao-e-evidencias-dos-testes.md)
- [Resultados da Execução Automatizada](docs/validacao/testes/resultados-da-execucao-automatizada.md)

## Documentação Relacionada

- [docs/README.md](docs/README.md)
- [Arquitetura do Firmware](docs/arquitetura/arquitetura-firmware.md)
- [Projeto Técnico (Design)](docs/arquitetura/projeto-tecnico-design.md)
- [Protocolo](docs/arquitetura/protocolo.md)
- [Diagramas](docs/arquitetura/diagramas/README.md)

## Observação Final

Este repositório representa um projeto acadêmico com foco em validação técnica,
arquitetura de firmware e evidência experimental. O código atual já é útil como
MVP funcional e como base de evolução, mas o suporte a múltiplos jogos ainda
deve ser tratado como uma próxima etapa de engenharia.
