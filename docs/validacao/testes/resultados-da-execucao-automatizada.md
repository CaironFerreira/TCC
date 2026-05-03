# Resultados da Execução Automatizada

## Data da verificação

- `26/04/2026`

## Execução realizada no host

Comando executado:

```bash
PLATFORMIO_CORE_DIR=$PWD/.platformio-core ./.venv/bin/platformio test -e test_native
```

Resultado:
- ambiente `test_native` aprovado;
- `68` casos de teste executados;
- `68` casos aprovados;
- duração total observada: `00:00:13.944`.

Resumo observado:

| Ambiente | Suíte | Resultado |
| --- | --- | --- |
| `test_native` | `test_gpio_button_input` | aprovado |
| `test_native` | `test_forza7_decoder` | aprovado |
| `test_native` | `test_udp_receiver` | aprovado |
| `test_native` | `test_telemetry_pipeline` | aprovado |
| `test_native` | `test_app` | aprovado |
| `test_native` | `test_protocol_regression` | aprovado |
| `test_native` | `test_speed_gauge` | aprovado |
| `test_native` | `test_telemetry_robustness` | aprovado |
| `test_native` | `test_ui_status` | aprovado |
| `test_native` | `test_display_service` | aprovado |
| `test_native` | `test_wifi_config_portal` | aprovado |
| `test_native` | `test_telemetry_service` | aprovado |
| `test_native` | `test_fuel_gauge` | aprovado |
| `test_native` | `test_rpm_gauge` | aprovado |
| `test_native` | `test_tire_temp_gauge` | aprovado |
| `test_native` | `test_instrument_cluster` | aprovado |

## Medição real de cobertura de código no host

Comando executado:

```bash
./scripts/run_native_coverage.sh
```

Resultado:
- relatórios de cobertura gerados em `coverage/reports/`;
- cobertura separada por tipo de teste;
- relatório consolidado da suíte host-side disponível em
  `coverage/reports/all/summary.txt`.

Resumo medido:

| Grupo | Linhas | Funções | Branches |
| --- | --- | --- | --- |
| `unit` | `66.1%` (`931/1409`) | `79.5%` (`151/190`) | `41.5%` (`262/632`) |
| `integration` | `13.4%` (`182/1359`) | `21.1%` (`36/171`) | `6.6%` (`42/632`) |
| `component` | `12.7%` (`173/1362`) | `15.3%` (`26/170`) | `4.9%` (`31/632`) |
| `all` | `76.9%` (`1086/1412`) | `84.8%` (`162/191`) | `46.2%` (`292/632`) |

Leitura técnica:
- a cobertura unitária está concentrada nas regras de negócio e adaptadores
  host-safe;
- nesta rodada, o escopo medido foi ampliado para incluir `DisplayService` e
  `WiFiConfigPortal`, o que reduz o percentual consolidado, mas aumenta a
  representatividade arquitetural da medição;
- os percentuais de integração e componente são menores porque cada grupo cobre
  uma fatia mais estreita do sistema;
- o valor mais relevante para defesa é o consolidado `all`, pois representa a
  união das suítes host-side automatizadas hoje disponíveis.
- nesta rodada, `src/application/App.cpp` e
  `src/application/services/TelemetryService.cpp` atingiram `100%` de cobertura
  de linhas na medição consolidada.

Arquivos de evidência gerados:
- `coverage/reports/unit/summary.txt`
- `coverage/reports/integration/summary.txt`
- `coverage/reports/component/summary.txt`
- `coverage/reports/all/summary.txt`

## Integração UDP real no ESP32

Comando executado:

```bash
PLATFORMIO_CORE_DIR=$PWD/.platformio-core ./.venv/bin/platformio test -e test_esp32_udp_integration
```

Porta serial utilizada:
- `/dev/cu.usbserial-0001`

Resultado observado:
- build, link e upload concluídos com sucesso;
- a suíte entrou em execução no ESP32;
- a placa reiniciou em loop com mensagem `Brownout detector was triggered`
  durante a ativação do Wi‑Fi AP usada pelo teste;
- a execução não pôde ser validada até o fim no setup elétrico disponível nesta
  sessão.

Interpretação técnica:
- o ambiente `test_esp32_udp_integration` foi implementado corretamente;
- o bloqueio atual é físico/elétrico, não de compilação nem de upload;
- no protótipo atual, a alimentação USB isolada não sustenta a carga ao ativar
  o rádio Wi‑Fi;
- a fonte externa usada no projeto resolve a alimentação, mas o setup de bancada
  desta sessão não permitia manter observabilidade serial simultânea.

## Verificação do ambiente embarcado

Comando executado:

```bash
PLATFORMIO_CORE_DIR=$PWD/.platformio-core ./.venv/bin/platformio test -e test_esp32_smoke
```

Porta serial utilizada:
- `/dev/cu.usbserial-0001`

Resultado observado:
- ambiente `test_esp32_smoke` aprovado;
- upload e execução serial concluídos com sucesso;
- `2` casos de teste executados;
- `2` casos aprovados;
- duração total observada: `00:00:14.725`.

Resumo observado:

| Ambiente | Suíte | Resultado |
| --- | --- | --- |
| `test_esp32_smoke` | `test_smoke_esp32_runtime` | aprovado |

## Interpretação

O estado atual da automação é:
- suíte host-side validada com sucesso;
- smoke test embarcado executado com sucesso em hardware real;
- evidência de serial, build, upload e execução já disponível para o smoke test;
- a integração UDP real foi validada até build, upload e início de execução, com
  limite físico de alimentação/observabilidade devidamente registrado.
