# Quadro Geral dos Testes

## Objetivo

Consolidar, em um único documento, o que foi testado no projeto, como cada
teste foi executado, a qual tipo de teste ele pertence e qual é o estado atual
da evidência produzida.

## Visão executiva

| Tipo de teste | Suítes | Casos | Ambiente | Estado atual |
| --- | --- | ---: | --- | --- |
| Unitário | `12` | `51` | `test_native` | executado e aprovado |
| Integração host-side | `3` | `9` | `test_native` | executado e aprovado |
| Componente | `1` | `8` | `test_native` | executado e aprovado |
| Smoke embarcado | `1` | `2` | `test_esp32_smoke` | executado e aprovado |
| Integração embarcada real | `1` | `3` | `test_esp32_udp_integration` | implementado; build/upload validados; execução final limitada pela bancada |

Totais relevantes:
- `18` suítes implementadas;
- `73` casos definidos;
- `70` casos com execução concluída e evidência coletada nesta fase;
- `3` casos de integração UDP real implementados, mas sem fechamento final por
  limitação elétrica e de observabilidade da bancada.

## Ferramentas e ambientes

- `PlatformIO`: build, upload e execução dos testes.
- `Unity`: framework de teste usado no host e no ESP32.
- `test_native`: ambiente principal dos testes automatizados host-side.
- `test_native_coverage`: ambiente usado para medir cobertura real de código.
- `test_esp32_smoke`: smoke test embarcado no `ESP32 Dev Module`.
- `test_esp32_udp_integration`: integração UDP real no `ESP32 Dev Module`.

## Quadro por suíte

| Suíte | Tipo | O que foi testado | Como foi testado | Ambiente | Estado |
| --- | --- | --- | --- | --- | --- |
| `test_forza7_decoder` | Unitário | Decoder do protocolo Forza 7, offsets, conversões e regra de ré | Pacotes artificiais em memória, sem hardware | `test_native` | aprovado |
| `test_telemetry_service` | Unitário | Reset de estado, getters derivados, timeout, descartes, erros e clamps | `FakePacketReceiver`, `FakeTelemetryDecoder` e `FakeClock` | `test_native` | aprovado |
| `test_ui_status` | Unitário | Cópia segura, truncamento e limpeza de `ssid` e `ip` | Chamada direta das rotinas de `UiStatus` | `test_native` | aprovado |
| `test_display_service` | Unitário | Seleção de layout, fallback, throttling de render e mensagens no display | `TFT_eSPI` stubado + layouts fake | `test_native` | aprovado |
| `test_wifi_config_portal` | Unitário | Fluxo assíncrono do portal Wi‑Fi, timeout de conexão e abertura do AP | Stubs host-side de `WiFi`, `Preferences`, `WebServer` e `DNSServer` | `test_native` | aprovado |
| `test_speed_gauge` | Unitário | Calibração, suavização, subida/queda brusca e despacho de sinal | `FakeGaugeMotor` e `FakeClock` | `test_native` | aprovado |
| `test_rpm_gauge` | Unitário | Calibração, suavização, aceleração forte, queda forte e filtro de sinal | `FakeGaugeMotor` e `FakeClock` | `test_native` | aprovado |
| `test_fuel_gauge` | Unitário | Calibração, primeira leitura, suavização, queda brusca e clamp | `FakeGaugeMotor` e `FakeClock` | `test_native` | aprovado |
| `test_tire_temp_gauge` | Unitário | Calibração com backoff, suavização, inversão e clamp | `FakeGaugeMotor` e `FakeClock` | `test_native` | aprovado |
| `test_udp_receiver` | Unitário | `begin()`, ausência de pacote, leitura válida, oversized e erro | Stub host-side de `WiFiUDP` | `test_native` | aprovado |
| `test_gpio_button_input` | Unitário | Pull-up/pull-down, debounce, bounce e pino inválido | Stub host-side de `Arduino` | `test_native` | aprovado |
| `test_instrument_cluster` | Unitário | Encaminhamento para instrumentos e filtro por tipo suportado | `FakeInstrument` | `test_native` | aprovado |
| `test_telemetry_pipeline` | Integração | Colaboração entre receiver, decoder real e `TelemetryService` | `FakePacketReceiver` + `Forza7Decoder` real + `TelemetryService` real | `test_native` | aprovado |
| `test_protocol_regression` | Integração | Regressão do protocolo com cenários nominal, ré e alta velocidade | Fixtures binárias persistidas em `test/fixtures/` | `test_native` | aprovado |
| `test_telemetry_robustness` | Integração | Burst, limite de drenagem, timeout e recuperação repetida | Receivers e decoders específicos para estresse do pipeline | `test_native` | aprovado |
| `test_app` | Componente | Fluxo da aplicação, portal Wi‑Fi, runtime, UI, botão e atualização de instrumentos | Doubles para Wi‑Fi, botão, display, telemetria, clock e cluster | `test_native` | aprovado |
| `test_smoke_esp32_runtime` | Smoke embarcado | Inicialização do `App` e execução de fluxo básico no ESP32 | Execução do mesmo fluxo em hardware real via serial | `test_esp32_smoke` | aprovado |
| `test_udp_pipeline_esp32` | Integração embarcada real | UDP real, `WiFiUDP` real, decoder real, timeout e recuperação no ESP32 | SoftAP local + envio UDP real para `UdpReceiver` | `test_esp32_udp_integration` | implementado; execução final limitada pela bancada |

## Como cada tipo de teste foi aplicado

### Testes unitários

Foco:
- validar regras locais de cálculo;
- validar conversões, clamps, debounce e despacho interno;
- isolar dependências de hardware.

Técnica usada:
- doubles (`FakeClock`, `FakeGaugeMotor`, `FakePacketReceiver`, `FakeStatusDisplay`);
- stubs host-side para APIs Arduino, `WiFiUDP`, `WiFi`, `WebServer`, `Preferences` e `TFT_eSPI`;
- execução rápida em `test_native`.

### Testes de integração host-side

Foco:
- validar a colaboração entre módulos reais;
- reduzir risco de incompatibilidade entre decoder, receiver e serviço;
- capturar regressões de protocolo.

Técnica usada:
- decoder real e serviço real;
- fixtures binárias persistidas;
- cenários de burst, timeout, pacote inválido e recuperação.

### Teste de componente

Foco:
- validar o comportamento do orquestrador principal da aplicação.

Técnica usada:
- `App` real;
- doubles nas bordas;
- cenários de transição de estado, atualização de UI e ação do botão.

### Smoke test embarcado

Foco:
- confirmar que o firmware de teste sobe no ESP32 e executa o fluxo básico.

Técnica usada:
- upload para a placa;
- execução pela serial;
- verificação do resultado da suíte `Unity`.

### Integração embarcada real

Foco:
- validar a recepção UDP real no ESP32 com o rádio ativo.

Técnica usada:
- `WiFi.softAP()` real;
- `WiFiUDP` real;
- `UdpReceiver`, `Forza7Decoder` e `TelemetryService` reais.

Limite atual:
- o protótipo entra em `brownout` quando o rádio Wi‑Fi é ativado com
  alimentação USB isolada;
- a fonte externa usada no projeto resolve a alimentação, mas o setup da sessão
  não permitia observabilidade serial simultânea.

## Como reproduzir

### Host

```bash
pio test -e test_native
./scripts/run_native_coverage.sh
```

### ESP32

```bash
pio test -e test_esp32_smoke
pio test -e test_esp32_udp_integration
```

Na prática, para este projeto:
- `test_esp32_smoke` já possui evidência concluída;
- `test_esp32_udp_integration` depende de uma bancada que permita Wi‑Fi ativo
  sem brownout e, idealmente, observabilidade serial ou UART dedicada.

## Evidências já produzidas

- resultados consolidados: `docs/validacao/testes/resultados-da-execucao-automatizada.md`
- estratégia: `docs/validacao/testes/estrategia-de-testes.md`
- rastreabilidade: `docs/validacao/testes/rastreabilidade-de-testes.md`
- execução e evidências: `docs/validacao/testes/execucao-e-evidencias-dos-testes.md`
- casos detalhados: `docs/validacao/testes/casos-de-teste-automatizados.md`

## Interpretação correta para o TCC

- a lógica de software está fortemente coberta por testes automatizados;
- o smoke embarcado já foi validado em hardware real;
- a integração UDP real também foi implementada e levada até upload e início de
  execução;
- o bloqueio remanescente é de instrumentação elétrica da bancada, não de
  ausência de teste nem de falha de compilação.

## Próxima etapa natural

A próxima etapa não é ampliar unitário, e sim fechar a validação experimental do
protótipo com:
- jogo real;
- telemetria real;
- alimentação final do projeto;
- evidência observável do comportamento do painel.
