# Capítulo de Validação e Testes

## Objetivo da validação

O processo de validação do firmware foi estruturado com dois objetivos
complementares. O primeiro consistiu em verificar, de forma automatizada, a
correção das regras de negócio, das conversões de protocolo, da atualização dos
instrumentos e da orquestração principal da aplicação. O segundo consistiu em
aproximar a verificação do contexto real de uso do protótipo, isto é, sua
execução em hardware embarcado e, posteriormente, sua operação com telemetria
oriunda do jogo.

Essa divisão foi adotada porque, em sistemas embarcados, nem todos os riscos
podem ser cobertos de maneira eficiente apenas com testes em hardware real.
Testes automatizados executados no host oferecem rapidez, reprodutibilidade e
maior capacidade de regressão. Já os testes em microcontrolador permitem
observar efeitos de inicialização, integração com periféricos e limitações
físicas da bancada.

## Estratégia adotada

A estratégia de testes foi organizada em cinco camadas:

1. **Testes unitários**, voltados à verificação isolada de funções e classes.
2. **Testes de integração host-side**, voltados à colaboração entre módulos
   reais do pipeline de telemetria.
3. **Teste de componente**, voltado ao comportamento do orquestrador principal
   da aplicação.
4. **Smoke test embarcado**, voltado à confirmação de que o firmware de teste
   sobe corretamente no ESP32 e executa o fluxo básico.
5. **Integração embarcada real**, voltada à recepção UDP real com o rádio
   Wi-Fi ativo no microcontrolador.

Essa decomposição permitiu equilibrar profundidade e viabilidade. As regras
críticas de software foram cobertas prioritariamente por testes automatizados
no host, enquanto o hardware foi validado nos pontos em que sua participação é
de fato indispensável.

## Ferramentas e ambientes utilizados

O ambiente de testes adotado foi composto por `PlatformIO` e `Unity`. O
`PlatformIO` foi utilizado como infraestrutura de build, upload e execução das
suítes, enquanto o `Unity` foi utilizado como framework de testes tanto no host
quanto no ESP32.

Foram configurados os seguintes ambientes:

- `test_native`: execução principal dos testes automatizados no computador;
- `test_native_coverage`: execução instrumentada para medição de cobertura;
- `test_esp32_smoke`: execução do smoke test no microcontrolador;
- `test_esp32_udp_integration`: execução da integração UDP real no
  microcontrolador.

Essa escolha se mostrou adequada ao projeto porque o firmware já se encontrava
estruturado em `PlatformIO` e apresentava isolamento suficiente entre domínio,
serviços e adaptadores, o que favoreceu a construção de doubles e a execução de
testes no host.

## Escopo coberto por tipo de teste

Os testes unitários concentraram-se na lógica de maior criticidade, em especial
na decodificação do protocolo, nas regras de negócio derivadas da telemetria,
na atualização dos instrumentos e nos adaptadores que puderam ser exercitados
de forma segura fora do hardware real. Nessa camada foram verificados, entre
outros aspectos, conversões de valores, aplicação de limites, debounce de
entrada, despacho de sinais e tratamento de pacotes inválidos.

Os testes de integração host-side cobriram a colaboração entre módulos reais,
com destaque para o pipeline formado por receptor de pacotes, decodificador e
serviço de telemetria. Também foram incluídos testes de regressão de protocolo
baseados em fixtures binárias persistidas e cenários de robustez, como bursts
de pacotes, timeout e recuperação repetida de sinal.

O teste de componente foi concentrado na classe principal da aplicação, com
ênfase na transição entre os estados de portal Wi-Fi e execução normal,
atualização da interface, leitura de botão e encaminhamento de valores para o
cluster de instrumentos.

O smoke test embarcado foi usado para verificar que o firmware de teste compila,
é transferido para a placa e executa corretamente o fluxo básico no ESP32. Já a
integração embarcada real foi preparada para validar o uso do `WiFiUDP` real,
do `UdpReceiver` real e do pipeline de telemetria com o rádio ativo.

## Resultados obtidos

Ao final da etapa automatizada, foram implementadas `18` suítes de teste,
totalizando `73` casos definidos. Desses, `70` casos tiveram execução concluída
com evidência coletada nesta fase, enquanto `3` casos pertencentes à suíte de
integração UDP real permaneceram condicionados à instrumentação da bancada.

A Tabela 1 resume a distribuição por tipo de teste.

| Tipo de teste | Suítes | Casos | Situação |
| --- | --- | ---: | --- |
| Unitário | `12` | `51` | executado e aprovado |
| Integração host-side | `3` | `9` | executado e aprovado |
| Componente | `1` | `8` | executado e aprovado |
| Smoke embarcado | `1` | `2` | executado e aprovado |
| Integração embarcada real | `1` | `3` | implementado; dependente de bancada |

Na execução host-side, o ambiente `test_native` foi concluído com sucesso,
totalizando `68` casos aprovados. No hardware embarcado, o ambiente
`test_esp32_smoke` também foi executado com sucesso, com `2` casos aprovados em
placa real.

Além da execução funcional, foi realizada medição de cobertura real de código
no host. O resultado consolidado da suíte host-side atingiu `76,9%` de cobertura
de linhas, `84,8%` de cobertura de funções e `46,2%` de cobertura de branches.
A Tabela 2 apresenta o resumo dessa medição.

| Grupo | Linhas | Funções | Branches |
| --- | --- | --- | --- |
| Unitário | `66,1%` | `79,5%` | `41,5%` |
| Integração | `13,4%` | `21,1%` | `6,6%` |
| Componente | `12,7%` | `15,3%` | `4,9%` |
| Consolidado host-side | `76,9%` | `84,8%` | `46,2%` |

É importante observar que os percentuais de integração e componente não devem
ser interpretados isoladamente como fraqueza da estratégia, pois cada grupo foi
medido sobre um conjunto amplo de arquivos-fonte compartilhados. Além disso, a
rodada atual ampliou o escopo de medição para incluir adaptadores concretos como
`DisplayService` e `WiFiConfigPortal`, o que reduz o percentual consolidado em
comparação com a rodada anterior, mas aumenta a representatividade arquitetural
da medição. Assim, o indicador mais representativo da cobertura real da
automação continua sendo o valor consolidado da suíte host-side.

## Discussão dos resultados

Os resultados indicam que a camada de regras de negócio do firmware foi coberta
de forma consistente. O conjunto de testes unitários e de integração host-side
reduziu o risco de regressões em pontos críticos, como interpretação de pacotes,
tratamento de timeout, conversão de unidades e atualização dos instrumentos.

O teste de componente reforçou essa avaliação ao validar o comportamento da
classe `App` em cenários de transição de estado, atualização periódica e reação
às bordas externas simuladas. Com isso, a aplicação principal deixou de ser
avaliada apenas por observação manual e passou a possuir verificação
reprodutível.

O smoke test executado no ESP32 forneceu evidência objetiva de que a suíte de
teste embarcada é capaz de compilar, ser transferida para a placa e executar
com sucesso em hardware real. Esse resultado é relevante porque reduz o risco
de discrepância entre o ambiente host-side e o microcontrolador efetivamente
utilizado no protótipo.

## Limitações observadas

A principal limitação identificada nesta etapa não foi de software, mas de
bancada. A suíte `test_esp32_udp_integration` foi implementada, compilada,
vinculada e transferida corretamente para o ESP32, porém a execução final da
integração UDP real ficou condicionada à alimentação do protótipo.

No cenário de teste com alimentação exclusivamente via USB, a ativação do rádio
Wi-Fi levou o protótipo a condição de `brownout`. Por outro lado, a fonte
externa utilizada no projeto é suficiente para sustentar o funcionamento do
sistema, mas o arranjo de bancada disponível durante a sessão não permitia
alimentação externa e observabilidade serial simultâneas.

Essa limitação não invalida a estratégia de testes adotada, mas delimita o
escopo da evidência automatizada obtida nesta fase. Em termos metodológicos, a
implementação e o upload da suíte de integração embarcada real foram validados;
o fechamento experimental completo dessa etapa depende de instrumentação
adicional da bancada, como UART dedicada, meio alternativo de observação ou
registro visual do comportamento do painel.

## Síntese da validação

Com base nos resultados obtidos, conclui-se que o projeto atingiu um nível
consistente de verificação automatizada para a lógica central do firmware e para
seu comportamento básico em hardware real. A automação host-side cobre a maior
parte dos riscos de regressão de software, enquanto o smoke test embarcado
confirma a viabilidade da execução no ESP32.

Assim, a etapa seguinte mais adequada não é ampliar indiscriminadamente a
quantidade de testes unitários, mas consolidar a validação experimental do
protótipo em condição final de uso, com telemetria proveniente do jogo, fonte
de alimentação definitiva e evidência observável do comportamento do painel.

## Referências internas do projeto

Os detalhes operacionais e as evidências completas desta etapa encontram-se nos
seguintes documentos do repositório:

- `docs/validacao/testes/estrategia-de-testes.md`
- `docs/validacao/testes/casos-de-teste-automatizados.md`
- `docs/validacao/testes/rastreabilidade-de-testes.md`
- `docs/validacao/testes/execucao-e-evidencias-dos-testes.md`
- `docs/validacao/testes/resultados-da-execucao-automatizada.md`
- `docs/validacao/testes/quadro-geral-dos-testes.md`
