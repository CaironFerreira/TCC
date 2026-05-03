# Projeto Técnico (Design)

Este documento consolida as decisões de design do firmware do projeto,
apresentando as tecnologias escolhidas, a justificativa técnica dessas
escolhas, os padrões arquiteturais adotados e a estrutura de pastas do código.

## Objetivo do Design

O design do firmware foi orientado por quatro objetivos principais:

- manter baixo acoplamento entre regra de negócio e hardware;
- facilitar evolução incremental do protótipo;
- permitir testes automatizados fora do ESP32 sempre que possível;
- organizar o código de modo compreensível para manutenção acadêmica e técnica.

## Tecnologias Escolhidas

| Tecnologia | Papel no projeto |
| --- | --- |
| `ESP32 Dev Module` | plataforma de execução do firmware |
| `C/C++` | implementação do firmware embarcado |
| `Arduino Framework` | acesso ao ecossistema embarcado, ciclo `setup/loop` e APIs de hardware |
| `PlatformIO` | build, upload, gerenciamento de ambientes e execução de testes |
| `UDP` | transporte da telemetria do simulador para o dispositivo |
| `WiFi`, `WiFiUDP`, `WebServer` | conectividade local, recepção de pacotes e portal de configuração |
| `TFT_eSPI` | renderização no display TFT |
| `Unity` | framework de testes automatizados no host e no microcontrolador |
| `PlantUML` | modelagem dos diagramas técnicos |
| `Markdown` | documentação textual do projeto |

## Justificativa das Escolhas

### `ESP32 Dev Module`

Foi adotado por combinar:

- conectividade Wi‑Fi nativa;
- custo acessível para prototipagem acadêmica;
- suporte maduro no ecossistema Arduino e PlatformIO;
- capacidade suficiente para telemetria, display e controle de instrumentos.

### `C/C++` com `Arduino Framework`

Essa combinação foi escolhida porque:

- é a base mais comum para firmware com ESP32;
- oferece acesso direto e previsível ao hardware;
- simplifica integração com bibliotecas já consolidadas;
- reduz a complexidade de bootstrap do projeto.

### `PlatformIO`

Foi escolhido para padronizar o ciclo de desenvolvimento:

- centraliza build, upload e testes;
- permite múltiplos ambientes (`esp32dev`, `test_native`, `test_esp32_smoke`);
- facilita reprodução dos resultados em diferentes máquinas;
- melhora a rastreabilidade da validação técnica.

### `UDP`

O protocolo foi escolhido porque atende bem ao cenário de sim racing:

- baixa latência;
- overhead reduzido;
- modelo compatível com telemetria contínua em rede local;
- aderência ao formato normalmente oferecido por simuladores.

### `TFT_eSPI`

Foi adotada como biblioteca de display por:

- possuir uso amplo no ecossistema ESP32;
- oferecer desempenho adequado para renderização simples;
- simplificar a integração com o display TFT do protótipo.

### `Unity`

Foi escolhido para testes por:

- funcionar tanto em ambiente nativo quanto embarcado;
- permitir isolar lógica de aplicação do hardware;
- ser compatível com o fluxo do PlatformIO.

### `PlantUML` e `Markdown`

Essas tecnologias foram adotadas porque:

- permitem documentação versionável em texto;
- facilitam manutenção no mesmo repositório do código;
- reduzem dependência de ferramentas gráficas proprietárias.

## Padrões e Decisões de Projeto

O projeto não segue MVC. A organização adotada é mais próxima de uma
arquitetura em camadas com forte influência de **Ports and Adapters
(Hexagonal)** e princípios de **Dependency Inversion**.

### 1. Ports and Adapters

As regras de aplicação dependem de contratos, não de implementações concretas.
Exemplos:

- `TelemetryService` depende de `IPacketReceiver` e `ITelemetryDecoder`;
- `App` depende de `IWifiConfigPortal`, `IButtonInput`, `IStatusDisplay` e `IInstrumentCluster`;
- os instrumentos dependem de `IGaugeMotor` e `IClock`.

Isso permite trocar infraestrutura sem reescrever a lógica principal.

### 2. Composition Root

O arquivo `src/composition/main.cpp` funciona como **ponto de composição** do
sistema:

- instancia adaptadores concretos;
- cria layouts e instrumentos;
- monta catálogos;
- injeta dependências na aplicação.

Essa decisão concentra a composição do sistema em um único ponto e evita que as
camadas internas conheçam detalhes de hardware.

### 3. Injeção de Dependências

As dependências principais são passadas por construtor. Exemplos:

- `App(...)`
- `TelemetryService(receiver, decoder, clock)`
- `SpeedGauge(motor, clock, cfg)`

Essa abordagem:

- reduz acoplamento;
- facilita criação de doubles para testes;
- melhora previsibilidade das dependências de cada classe.

### 4. Strategy por Interface

O projeto usa polimorfismo via interfaces para variar comportamento sem alterar
o fluxo principal. Isso aparece em:

- `ITelemetryDecoder`: permite trocar o decoder do protocolo;
- `IDisplayLayout`: permite múltiplos layouts de tela;
- `IInstrument`: permite registrar instrumentos distintos no cluster.

Na prática, isso aproxima o desenho do padrão **Strategy**, ainda que aplicado
de forma simples.

### 5. Registro por Catálogo

O sistema usa listas registradas no ponto de composição para expansão controlada:

- catálogo de layouts entregue ao `DisplayService`;
- lista de instrumentos entregue ao `InstrumentCluster`.

Essa decisão reduz condicionais fixas e favorece extensibilidade.

## Estrutura de Pastas

### Estrutura do repositório

```text
.
├── src/        Firmware principal
├── test/       Testes automatizados e fixtures
├── scripts/    Scripts auxiliares
├── docs/       Documentação técnica e acadêmica
├── include/    Arquivos auxiliares de configuração
└── platformio.ini
```

### Estrutura principal do firmware

```text
src/
  composition/          Ponto de composição e inicialização
  application/          Orquestração e serviços de aplicação
    services/           Fluxos principais, como telemetria e cluster
    instruments/        Instrumentos físicos orientados a sinal
    ui/                 Compatibilidade e wrappers de UI
  config/               Constantes de placa, pinos e limites
  domain/               Modelos e estado sem dependência de hardware
  ports/                Interfaces entre aplicação e infraestrutura
  adapters/             Implementações concretas
    input/              Entrada de botão
    motors/             Controle dos motores dos ponteiros
    network/            Wi‑Fi, portal e UDP
    telemetry/          Decoders de telemetria
    display/            Serviço de display e layouts
    time/               Relógio concreto
```

## Benefícios da Estrutura Adotada

- facilita testes unitários e de integração host-side;
- isola APIs de hardware das regras centrais;
- melhora legibilidade para manutenção e banca;
- permite adicionar novos instrumentos, layouts e decoders com impacto local.

## Referências Relacionadas

- [Arquitetura do Firmware](./arquitetura-firmware.md)
- [Protocolo](./protocolo.md)
- [Diagramas](./diagramas/README.md)
