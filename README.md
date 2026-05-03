# TCC – Sistema Embarcado para Feedback Háptico em Sim Racing

Repositório do Trabalho de Conclusão de Curso desenvolvido na modalidade
**Relatório Técnico de Software**. O projeto propõe um firmware para **ESP32**
capaz de receber telemetria de simuladores de corrida via **UDP**, interpretar
esses dados e acionar instrumentos físicos e interface visual em tempo real.

## Panorama Atual

O estado atual do repositório já cobre o núcleo técnico do projeto:

- recepção de telemetria por UDP em rede local;
- decodificação do protocolo usado no simulador;
- atualização de velocímetro, conta-giros, combustível e temperatura;
- exibição de marcha, status e dados de telemetria em display TFT;
- fluxo assíncrono de configuração Wi‑Fi com portal embarcado;
- arquitetura modular organizada por portas e adaptadores;
- suíte automatizada host-side e validações embarcadas iniciais.

Em termos de validação:

- `18` suítes de teste estão implementadas;
- `73` casos de teste estão definidos;
- `70` casos já possuem execução concluída com evidência coletada;
- o `smoke test` no ESP32 já foi validado em hardware real;
- a integração UDP real no ESP32 foi implementada, compilada e levada a upload,
  mas o fechamento experimental ainda depende da bancada elétrica.

## Escopo Atual do Firmware

O firmware presente neste repositório está centrado no **MVP funcional** do
painel háptico e visual. O que já está materializado no código é:

- conexão e configuração de rede via portal Wi‑Fi;
- recepção contínua de pacotes UDP;
- serviço de telemetria com tratamento de timeout, descarte e recuperação;
- cluster de instrumentos com despacho por tipo de sinal;
- instrumentos físicos para velocidade, RPM, combustível e temperatura;
- serviço de display com layouts intercambiáveis e navegação por botão.

Pontos que permanecem como evolução futura ou validação complementar:

- compatibilidade formal por plataforma (`Xbox`, `PS4`, `PS5`, `PC`);
- efeitos de vibração (`RF14` e `RF15`);
- fechamento da validação eletromecânica completa do protótipo.

## Arquitetura

O firmware segue uma organização em camadas para reduzir acoplamento e facilitar
manutenção e testes:

- `composition/`: ponto de composição e inicialização;
- `application/`: casos de uso, orquestração e serviços de aplicação;
- `domain/`: modelos e estado neutros;
- `ports/`: contratos entre camadas;
- `adapters/`: integrações concretas com Wi‑Fi, UDP, display, GPIO e tempo.

Direção principal das dependências:

```text
composition -> application -> domain / ports
adapters -> ports
```

Documentos e diagramas relacionados:

- [Arquitetura do Firmware](docs/arquitetura/arquitetura-firmware.md)
- [Projeto Técnico (Design)](docs/arquitetura/projeto-tecnico-design.md)
- [Protocolo](docs/arquitetura/protocolo.md)
- [Diagramas](docs/arquitetura/diagramas/README.md)

## Estrutura do Repositório

```text
.
├── src/        Firmware principal
├── test/       Suítes automatizadas e fixtures
├── scripts/    Automação auxiliar, incluindo cobertura
├── docs/       Documentação do TCC organizada por tema
├── include/    Configurações auxiliares do projeto
└── platformio.ini
```

## Build e Execução

Pré-requisitos principais:

- `PlatformIO CLI`
- toolchain C/C++
- placa compatível com `esp32dev` para ensaios embarcados

Comandos mais usados:

```bash
pio run -e esp32dev
pio test -e test_native
pio test -e test_esp32_smoke
pio test -e test_esp32_udp_integration
./scripts/run_native_coverage.sh
```

Observação:
- a execução da integração UDP real em hardware ainda depende de uma bancada
  que sustente o Wi‑Fi ativo sem `brownout` e, idealmente, preserve
  observabilidade serial simultânea.

## Testes e Validação

A estratégia de testes combina execução rápida no host com validação incremental
no microcontrolador:

- testes unitários;
- integração host-side;
- teste de componente da aplicação;
- smoke test embarcado;
- integração UDP real no ESP32.

Resumo do estado atual:

| Tipo | Estado |
| --- | --- |
| Unitário | implementado, executado e aprovado |
| Integração host-side | implementado, executado e aprovado |
| Componente | implementado, executado e aprovado |
| Smoke embarcado | implementado, executado e aprovado |
| Integração UDP real | implementado; build/upload validados; execução final limitada pela bancada |

Referências:

- [Quadro Geral dos Testes](docs/validacao/testes/quadro-geral-dos-testes.md)
- [Rastreabilidade de Testes](docs/validacao/testes/rastreabilidade-de-testes.md)
- [Execução e Evidências](docs/validacao/testes/execucao-e-evidencias-dos-testes.md)
- [Resultados da Execução Automatizada](docs/validacao/testes/resultados-da-execucao-automatizada.md)

## Documentação

A documentação foi reorganizada por tema e possui índice em
[docs/README.md](docs/README.md):

- `docs/analise/`
- `docs/requisitos/`
- `docs/arquitetura/`
- `docs/planejamento/`
- `docs/validacao/`

## Tecnologias Utilizadas

- `ESP32`
- `C/C++`
- `Arduino Framework`
- `PlatformIO`
- `UDP`
- `TFT_eSPI`
- `Unity`
- `PlantUML`
- `Markdown`

## Observação

Este repositório representa um projeto acadêmico com foco em **validação
técnica**, **organização arquitetural** e **evidência experimental**. O código
e a documentação refletem um MVP robusto para banca e evolução futura, não um
produto final comercial.
