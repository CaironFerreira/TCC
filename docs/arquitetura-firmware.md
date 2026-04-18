# Arquitetura do Firmware

Este documento descreve a organização das pastas do firmware e o papel de cada
camada. A estrutura foi separada por responsabilidade para reduzir acoplamento,
facilitar testes futuros e manter a evolução do projeto previsível.

## Estrutura de Pastas

```text
src/
  app/                  Orquestra o ciclo principal da aplicação
  config/               Constantes de placa, pinos, escalas e limites
  domain/               Modelos de dados sem dependência de hardware
  ports/                Interfaces usadas entre camadas e drivers concretos
  drivers/              Adaptadores de baixo nível para hardware e rede
    input/              Leitura de botões e entradas digitais
    motors/             Driver do motor/ponteiro
    network/            UDP, Wi-Fi e portal de configuração
  instruments/          Mapeia telemetria para instrumentos físicos
  telemetry/            Recepção, validação e decodificação de telemetria
    decoders/           Decoders específicos de simuladores/protocolos
  ui/
    display/            Serviço de display, layouts e estado de tela
```

## Regras de Organização

- `app/` coordena os módulos, mas não implementa protocolo, renderização ou controle de motor.
- `domain/` deve permanecer independente de Esp32, Wi-Fi, display ou motor sempre que possível.
- `ports/` define os contratos usados pelas camadas superiores para acessar recursos externos.
- `telemetry/decoders/` concentra detalhes de protocolo e offsets do simulador.
- `instruments/` traduz valores de domínio para posições físicas dos ponteiros.
- `drivers/` encapsula APIs de infraestrutura, como `WiFi`, `WebServer`, `WiFiUDP` e GPIO.
- `ui/display/` concentra renderização e layouts; layouts dependem de `UiStatus`, não de `DisplayService`.
- Includes devem usar caminhos relativos à raiz de `src/`, por exemplo `telemetry/TelemetryService.h`.

## Direção das Dependências

As camadas superiores dependem de interfaces, não de drivers concretos. O
arquivo `main.cpp` é o ponto de composição: ele cria os drivers reais e injeta
essas dependências na aplicação.

```text
main.cpp -> app -> telemetry / instruments / ui -> domain / ports
drivers -> ports
```

Exemplos:

- `TelemetryService` depende de `IPacketReceiver`; `UdpReceiver` implementa essa porta.
- Instrumentos dependem de `IGaugeMotor`; `GaugeMotorTmc2208` implementa essa porta.
- `App` depende de `IWifiConfigPortal` e `IButtonInput`; os drivers concretos ficam em `drivers/`.

## Telas do Display

Os layouts de telemetria são registrados em `DisplayService`. Para adicionar
uma nova tela, crie uma classe que implemente `IDisplayLayout` e registre a
instância na lista de layouts do serviço. A navegação entre telas é acionada
pelo botão configurado em `BoardConfig.h`.
