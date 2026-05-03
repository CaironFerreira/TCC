# Estratégia de Testes Automatizados

## Objetivo

Definir a estratégia de testes automatizados do firmware do TCC, cobrindo a lógica
crítica do sistema com execução rápida no computador e uma validação básica no
ESP32.

## Ferramentas adotadas

- `PlatformIO`: ambiente de build, organização dos ambientes de teste e execução
  com `pio test`.
- `Unity`: framework de testes automatizados escolhido por funcionar tanto em
  `native` quanto em `embedded`.
- `platform = native`: execução dos testes unitários e de componente no host.
- `board = esp32dev`: execução do smoke test no microcontrolador.
- `fixtures binárias`: regressão persistente do protocolo com pacotes de referência.

## Ambientes de teste

### `test_native`

Ambiente principal para desenvolvimento da suíte.

Uso esperado:
- testes unitários;
- testes de componente;
- regressões rápidas a cada alteração de código.

Vantagens:
- execução rápida;
- depuração simples;
- não depende da placa conectada.

### `test_esp32_smoke`

Ambiente de smoke test executado diretamente no ESP32.

Uso esperado:
- validar inicialização do fluxo principal no microcontrolador;
- garantir que o binário de teste sobe e roda sem falha grave;
- detectar regressões de compilação ou runtime embarcado.

Limite atual:
- não valida o comportamento elétrico real de display, Wi-Fi, rede UDP e motores;
  esses cenários continuam exigindo validação manual assistida.

### `test_esp32_udp_integration`

Ambiente embarcado para validar recepção UDP real, decoder real e
`TelemetryService` no ESP32.

Uso esperado:
- verificar `WiFiUDP` real;
- validar descarte de pacote oversized;
- validar timeout e recuperação no hardware.

Limite atual:
- depende de alimentação estável da placa ao ativar Wi‑Fi;
- no protótipo atual, USB sozinho pode provocar brownout quando o rádio sobe;
- a fonte externa usada no projeto resolve a alimentação, mas no setup de bancada
  da sessão registrada não permitia observabilidade serial simultânea.

## Tipos de teste adotados

### Testes unitários

Cobrem classes e funções isoladas.

Módulos cobertos na primeira fase:
- `src/adapters/telemetry/Forza7Decoder.cpp`;
- `src/adapters/network/udp/UdpReceiver.cpp`;
- `src/adapters/input/GpioButtonInput.cpp`;
- `src/adapters/display/DisplayService.cpp`;
- `src/adapters/network/wifi/WiFiConfigPortal.cpp`;
- `src/application/services/TelemetryService.cpp`;
- `src/domain/UiStatus.h`;
- `src/application/instruments/SpeedGauge.cpp`;
- `src/application/instruments/RpmGauge.cpp`;
- `src/application/instruments/FuelGauge.cpp`;
- `src/application/instruments/TireTempGauge.cpp`;
- `src/application/services/InstrumentCluster.cpp`.

### Testes de integração

Cobrem a colaboração entre módulos reais do pipeline de telemetria, mantendo
fake apenas nas bordas de infraestrutura.

Módulo coberto na fase atual:
- `src/adapters/telemetry/Forza7Decoder.cpp` integrado com
  `src/application/services/TelemetryService.cpp`.
- regressão do protocolo com fixtures binárias persistidas em `test/fixtures/`.
- robustez do `TelemetryService` sob burst, timeout e recuperação.

### Testes de componente

Cobrem a coordenação entre módulos sem usar hardware real.

Módulo coberto na primeira fase:
- `src/application/App.cpp` com doubles para Wi-Fi, botão, display,
  telemetria, clock e cluster de instrumentos.

### Smoke test embarcado

Cobre a subida básica do fluxo principal no ESP32 com doubles controlados.

Módulo coberto na primeira fase:
- `src/application/App.cpp` executado no ambiente `test_esp32_smoke`.
- integração UDP real executada em `test_esp32_udp_integration`.

## Critérios de aceitação

A suíte inicial será considerada adequada quando:
- os testes `native` executarem sem falha;
- o smoke test embarcado executar no ESP32 sem travamento;
- os testes cobrirem as regras de negócio mais sensíveis do MVP;
- a documentação permitir repetir os testes por outra pessoa da banca.

## Escopo funcional coberto

Cobertura direta inicial:
- `RF02`: comunicação e tratamento da entrada UDP;
- `RF03`: interpretação dos dados recebidos;
- `RF04`: controle do indicador de velocidade;
- `RF05`: controle do indicador de rotação;
- `RF08`: inicialização automática do sistema;
- `RF09`: indicação de estado operacional.

Cobertura parcial inicial:
- `RNF06`: robustez diante de falhas pequenas de comunicação;
- `RNF09`: manutenibilidade pela automação da regressão.

Cobertura ampliada na fase atual:
- regressão de protocolo com pacotes binários versionados;
- robustez do pipeline contra burst acima do `PACKET_DRAIN_LIMIT`;
- tentativa de integração UDP real no ESP32 com rede local via SoftAP.

Observação metodológica:
- a suíte `test_esp32_udp_integration` foi mantida porque ela verifica um risco
  real do sistema, mas sua execução final depende da instrumentação elétrica da
  bancada e não apenas do software.

## Estrutura criada em `test/`

- `test/support/`: doubles e utilitários compartilhados.
- `test/test_forza7_decoder/`: testes unitários do decoder.
- `test/test_telemetry_pipeline/`: testes de integração do pipeline de telemetria.
- `test/test_protocol_regression/`: regressão de protocolo com fixtures binárias.
- `test/test_telemetry_robustness/`: robustez do serviço de telemetria.
- `test/test_telemetry_service/`: testes unitários do serviço de telemetria.
- `test/test_ui_status/`: testes unitários do estado de UI.
- `test/test_speed_gauge/`: testes unitários do velocímetro.
- `test/test_rpm_gauge/`: testes unitários do conta-giros.
- `test/test_fuel_gauge/`: testes unitários do medidor de combustível.
- `test/test_tire_temp_gauge/`: testes unitários do indicador de temperatura.
- `test/test_udp_receiver/`: testes unitários do adaptador UDP.
- `test/test_gpio_button_input/`: testes unitários do adaptador de botão.
- `test/test_instrument_cluster/`: testes unitários do cluster de instrumentos.
- `test/test_app/`: testes de componente da aplicação.
- `test/test_smoke_esp32_runtime/`: smoke test no ESP32.
- `test/test_udp_pipeline_esp32/`: integração UDP real no ESP32.

## Decisões de projeto para testabilidade

A suíte usa os contratos já existentes do projeto:
- `src/ports/IClock.h`;
- `src/ports/IPacketReceiver.h`;
- `src/ports/ITelemetryDecoder.h`;
- `src/ports/IStatusDisplay.h`;
- `src/ports/IInstrumentCluster.h`;
- `src/ports/IWifiConfigPortal.h`;
- `src/ports/IButtonInput.h`.

Isso reduz acoplamento com hardware e permite testar a lógica de domínio e de
aplicação sem depender do ESP32 durante o ciclo principal de desenvolvimento.
