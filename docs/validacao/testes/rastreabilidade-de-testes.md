# Rastreabilidade de Testes

## Relação entre requisitos e testes automatizados

| Requisito | Descrição resumida | Cobertura automatizada | Suítes principais |
| --- | --- | --- | --- |
| `RF02` | Comunicação com o jogo via UDP | Alta | `test_udp_receiver`, `test_telemetry_service`, `test_telemetry_pipeline`, `test_udp_pipeline_esp32` |
| `RF03` | Interpretação dos dados recebidos | Alta | `test_forza7_decoder`, `test_telemetry_service`, `test_telemetry_pipeline`, `test_protocol_regression` |
| `RF04` | Controle do indicador de velocidade | Alta | `test_speed_gauge`, `test_app` |
| `RF05` | Controle do indicador de rotação | Alta | `test_rpm_gauge`, `test_app`, `test_instrument_cluster` |
| `RF06` | Indicação de marcha no display | Parcial | `test_forza7_decoder`, `test_app` |
| `RF08` | Inicialização automática do sistema | Alta | `test_app`, `test_smoke_esp32_runtime` |
| `RF09` | Indicação de status operacional | Alta | `test_app`, `test_smoke_esp32_runtime` |
| `RF14` | Vibração em faixa zebrada | Não coberto nesta fase | validação futura |
| `RF15` | Vibração ao dar partida | Não coberto nesta fase | validação futura |
| `RNF06` | Robustez | Alta | `test_udp_receiver`, `test_telemetry_service`, `test_telemetry_pipeline`, `test_telemetry_robustness`, `test_app`, `test_udp_pipeline_esp32` |
| `RNF09` | Manutenibilidade | Alta | toda a suíte automatizada |

## Justificativa da cobertura parcial

### `RF02`

A lógica de recepção e o contrato UDP já estão cobertos automaticamente, inclusive
com uma suíte de integração real para ESP32. O ponto pendente não é funcional,
mas físico: estabilidade de alimentação da placa durante testes longos com Wi‑Fi
e disponibilidade de observabilidade serial quando a fonte externa do protótipo
está em uso.

### `RF06`

A telemetria de marcha é interpretada e propagada para a UI, porém a renderização
visual concreta do display continua dependendo de validação manual do adaptador.

### `RNF06`

A robustez está bem coberta no host para falhas de pacote, timeout, burst e
recuperação. O que permanece fora da automação está ligado a instabilidade física
de alimentação, rede, ruído elétrico e instrumentação de bancada.

## Itens não cobertos nesta fase

Os requisitos abaixo permanecem fora da automação inicial por dependerem de
plataformas externas, periféricos físicos completos ou cenários de uso reais:

- `RF10`, `RF11`, `RF12`, `RF13`;
- `RNF05`, `RNF11`, `RNF12`, `RNF13`;
- `RNF10` em nível eletromecânico.

## Estratégia complementar para a banca

Para os itens não cobertos automaticamente, a recomendação é apresentar:
- roteiro de testes manuais por plataforma;
- vídeo curto de demonstração do painel em operação;
- evidências fotográficas da montagem física;
- saída dos testes automatizados executados no host e no ESP32;
- justificativa explícita quando um ensaio embarcado exigir fonte externa e não
  puder expor serial no mesmo setup.
