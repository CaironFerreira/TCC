# Arquitetura do Firmware

Este documento descreve a organização das pastas do firmware e o papel de cada
camada. A estrutura foi separada por responsabilidade para reduzir acoplamento,
facilitar testes futuros e manter a evolução do projeto previsível.

## Estrutura de Pastas

```text
src/
  composition/          Ponto de composição e inicialização do sistema
  application/          Casos de uso, orquestração e lógica de aplicação
    services/           Serviços de aplicação e coordenação entre módulos
    instruments/        Instrumentos e tradução de sinais para atuadores
    ui/                 Wrappers finos e compatibilidade de interface
  config/               Constantes de placa, pinos, escalas e limites
  domain/               Modelos de dados e view-models neutros, sem dependência de hardware
  ports/                Interfaces usadas entre camadas e adaptadores concretos
  adapters/             Implementações concretas das portas
    input/              Leitura de botões e entradas digitais
    motors/             Driver do motor/ponteiro
    network/            UDP, Wi-Fi e portal de configuração
    telemetry/          Decoders e integrações concretas de telemetria
    display/            Serviço de display, layouts e helpers de renderização
    time/               Relógio concreto baseado em Arduino
```

## Regras de Organização

- `composition/` é o ponto de composição e a única camada que instancia classes concretas e monta catálogos/listas de extensões.
- `application/` coordena os módulos, mas não implementa protocolo, renderização concreta ou controle direto de hardware.
- `domain/` deve permanecer independente de Esp32, Wi-Fi, display ou motor sempre que possível.
- `ports/` define os contratos usados pelas camadas superiores para acessar recursos externos.
- `application/services/` concentra serviços de aplicação, como recepção tratada de telemetria e despacho de sinais para instrumentos.
- `application/instruments/` contém instrumentos de aplicação que dependem apenas de portas.
- `adapters/` encapsula APIs de infraestrutura, como `WiFi`, `WebServer`, `WiFiUDP`, `TFT_eSPI`, GPIO e tempo do Arduino.
- `adapters/display/` concentra renderização e layouts; layouts dependem de `UiStatus`, não de `DisplayService`.
- `UiStatus` pertence a `domain/` para evitar acoplamento circular entre `application/` e `ports/`.
- `application/`, `domain/` e `ports/` não devem depender de `TFT_eSPI`, `WiFi`, `WebServer` ou outras APIs de framework.
- Novas telas e instrumentos devem entrar por registro/composição, não por edição de listas estáticas internas de serviços.
- Includes devem usar caminhos relativos à raiz de `src/`, por exemplo `application/services/TelemetryService.h`.
- `IWifiConfigPortal` opera de forma assíncrona: `begin()` apenas inicia o fluxo, enquanto o resultado efetivo é observado em `tick()`, `isConnected()` e `isPortalActive()`.

## Direção das Dependências

As camadas superiores dependem de interfaces, não de implementações concretas.
O arquivo `composition/main.cpp` é o ponto de composição: ele cria adaptadores,
layouts e instrumentos concretos, e injeta essas dependências
na aplicação.

```text
composition -> application -> domain / ports
adapters -> ports
adapters/display/layouts -> adapters/display
```

Exemplos:

- `application/services/TelemetryService` depende de `IPacketReceiver`; `adapters/network/udp/UdpReceiver` implementa essa porta.
- Instrumentos em `application/instruments` dependem de `IGaugeMotor`; `adapters/motors/GaugeMotorTmc2208` implementa essa porta.
- `App` depende de `IWifiConfigPortal` e `IButtonInput`; os adaptadores concretos ficam em `adapters/`.
- `adapters/display/DisplayService` recebe um catálogo de `IDisplayLayout` montado em `composition/main.cpp`.
- `InstrumentCluster` recebe uma lista de instrumentos registrados e faz o despacho por tipo de sinal.

## Telas do Display

Os layouts de telemetria são compostos em `composition/main.cpp`. Para adicionar uma nova
tela, crie uma classe que implemente `IDisplayLayout`, instancie essa classe no
ponto de composição e registre a instância no catálogo entregue ao
`DisplayService`. A navegação entre telas é acionada pelo botão configurado em
`BoardConfig.h`.

## Instrumentos

Os instrumentos físicos são registrados no `InstrumentCluster` por meio de uma
lista de `IInstrument`. Cada instrumento declara quais sinais aceita
(`velocidade`, `rpm`, `combustível`, `temperatura`) e o cluster faz o despacho
sem conhecer classes concretas. Para adicionar um novo instrumento, a regra é:

- implementar `IInstrument`;
- depender apenas das portas necessárias (`IGaugeMotor`, `IClock` etc.);
- registrar a instância em `composition/main.cpp`.
