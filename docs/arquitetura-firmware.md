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
  drivers/              Adaptadores de baixo nível para hardware e rede
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
- `telemetry/decoders/` concentra detalhes de protocolo e offsets do simulador.
- `instruments/` traduz valores de domínio para posições físicas dos ponteiros.
- `drivers/` encapsula APIs de infraestrutura, como `WiFi`, `WebServer`, `WiFiUDP` e GPIO.
- `ui/display/` concentra renderização e layouts; layouts dependem de `UiStatus`, não de `DisplayService`.
- Includes devem usar caminhos relativos à raiz de `src/`, por exemplo `telemetry/TelemetryService.h`.
