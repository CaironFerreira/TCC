# Execução e Evidências dos Testes

## Pré-requisitos

### Host

- `PlatformIO CLI` instalado.
- `gcovr` instalado para medicao real de cobertura.
- compilador C/C++ disponível no sistema operacional.
- projeto aberto na raiz do repositório.

### Hardware

- placa `ESP32 Dev Module` compatível com `esp32dev`.
- cabo USB funcional.
- porta serial disponível.
- para ensaios com Wi‑Fi real, considerar a limitação elétrica da bancada: o
  protótipo pode exigir fonte externa para sustentar o rádio, e no setup atual
  isso inviabiliza a observabilidade serial simultânea.

## Ambientes configurados

Os ambientes definidos em `platformio.ini` são:
- `esp32dev`: firmware principal;
- `test_native`: suíte host-side com Unity;
- `test_native_coverage`: ambiente nativo instrumentado para cobertura;
- `test_esp32_smoke`: smoke test executado no microcontrolador;
- `test_esp32_udp_integration`: integração UDP real no microcontrolador.

## Comandos de execução

### Testes no host

```bash
pio test -e test_native
```

### Smoke test no ESP32

```bash
pio test -e test_esp32_smoke
```

### Integração UDP real no ESP32

```bash
pio test -e test_esp32_udp_integration
```

### Cobertura real de código no host

Antes da primeira medição de cobertura, garantir que a dependência do Unity já
foi baixada pelo PlatformIO:

```bash
pio test -e test_native
```

Depois disso, executar:

```bash
./scripts/run_native_coverage.sh
```

Arquivos gerados:
- `coverage/reports/unit/summary.txt`
- `coverage/reports/integration/summary.txt`
- `coverage/reports/component/summary.txt`
- `coverage/reports/all/summary.txt`
- `coverage/reports/*/files.txt`
- `coverage/reports/*/summary.json`

## Evidências que devem ser coletadas

### Evidências mínimas para a banca

- saída completa do comando `pio test -e test_native`;
- saída completa do comando `pio test -e test_esp32_smoke`;
- saída completa do comando `pio test -e test_esp32_udp_integration`, quando a
  bancada permitir observabilidade serial;
- saída completa do comando `./scripts/run_native_coverage.sh`;
- identificação da revisão testada, por exemplo `git rev-parse --short HEAD`;
- data e hora da execução;
- foto ou vídeo curto da placa conectada durante o smoke test;
- observações de falhas, se existirem.

### Registro sugerido por execução

| Campo | Valor a preencher |
| --- | --- |
| Data | |
| Responsável | |
| Commit | |
| Ambiente | `test_native`, `test_esp32_smoke` ou `test_esp32_udp_integration` |
| Resultado | aprovado / reprovado |
| Quantidade de testes | |
| Falhas | |
| Observações | |

## Critérios objetivos de aprovação

Uma execução é considerada aprovada quando:
- todos os testes do ambiente retornam `PASSED`;
- não há travamento do processo de teste;
- no smoke test, o ESP32 executa a suíte e devolve o relatório pela serial.

Para o ensaio de integração UDP real, quando a bancada não permitir alimentação
externa e serial simultâneas, o critério mínimo aceitável passa a ser:
- build e upload concluídos com sucesso;
- evidência observável do comportamento do firmware por meio alternativo
  previamente definido, como display, vídeo ou UART externa dedicada.

## Limitações assumidas nesta fase

- os testes automáticos não substituem a validação física completa do painel;
- compatibilidade com Xbox, PlayStation e PC continua dependendo de ensaio manual;
- latência real fim a fim ainda deve ser medida em cenário com jogo e hardware.
- a integração Wi‑Fi/UDP real pode depender de alimentação externa do protótipo,
  o que exige planejamento adicional de instrumentação na bancada.

## Estado atual do repositório

A infraestrutura e os casos de teste foram adicionados ao repositório nas pastas
`test/`, `docs/` e `scripts/`.

A primeira verificação executada nesta iteração está registrada em
`docs/validacao/testes/resultados-da-execucao-automatizada.md`.

No estado atual, a suíte `test_native` já possui `68` casos automatizados
aprovados.

No estado atual, a medição de cobertura real também já pode ser reproduzida no
host e gera relatórios separados por tipo de teste.

Na última medição consolidada, a suíte host-side atingiu `76.9%` de cobertura
de linhas e `46.2%` de cobertura de branches.

Esse número consolidado passou a considerar também `DisplayService` e
`WiFiConfigPortal`, ampliando o escopo medido além do núcleo previamente
host-safe.

O smoke test embarcado também já foi validado em hardware real com a porta
`/dev/cu.usbserial-0001`, conforme registrado em
`docs/validacao/testes/resultados-da-execucao-automatizada.md`.

O ambiente `test_esp32_udp_integration` já foi implementado e compilado, porém a
execução em hardware ficou limitada pela bancada atual: com USB apenas, o
protótipo entra em brownout ao ativar Wi‑Fi AP; com a fonte externa usada no
projeto, a observabilidade serial simultânea não estava disponível na sessão
registrada.

Novas execuções dependem de um ambiente local com `PlatformIO CLI` disponível.
Se a ferramenta ainda não estiver instalada, a primeira execução deve começar por:

```bash
platformio --version
```

ou

```bash
pio --version
```

## Roteiro de apresentação no TCC

Sugestão de narrativa para defesa:
- mostrar a estratégia de testes e a separação entre unitário, componente e smoke;
- executar `test_native` para demonstrar regressão rápida;
- executar `test_esp32_smoke` com a placa conectada;
- relacionar a saída dos testes com os requisitos em
  `docs/validacao/testes/rastreabilidade-de-testes.md`.
