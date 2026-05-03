# Casos de Teste Automatizados

## Convenção de identificação

- `UT`: teste unitário.
- `IT`: teste de integração.
- `CT`: teste de componente.
- `ST`: smoke test.

## Suíte `test_forza7_decoder`

Arquivo: `test/test_forza7_decoder/test_main.cpp`

### `UT-DEC-01` - rejeitar pacote curto ou nulo

Objetivo:
- garantir que o decoder invalida a saída quando o pacote não tem o tamanho
  mínimo esperado.

Validações:
- `decode()` retorna `false`;
- `TelemetryFrame` fica inválido;
- campos previamente preenchidos são resetados.

### `UT-DEC-02` - mapear todos os campos principais do pacote

Objetivo:
- validar offsets, conversões e preenchimento do `TelemetryFrame`.

Validações:
- RPM, velocidade, marcha, volta, posição e combustível;
- temperaturas individuais;
- média de temperatura dos pneus.

### `UT-DEC-03` - converter marcha zero em ré

Objetivo:
- validar a regra específica de interpretação de marcha no protocolo atual.

Validação:
- `gear == -1` quando o byte recebido é `0`.

## Suíte `test_telemetry_service`

Arquivo: `test/test_telemetry_service/test_main.cpp`

### `UT-TEL-01` - resetar estado no `begin()`

Objetivo:
- garantir que a inicialização limpa telemetria, contadores e status de entrada.

Validações:
- `hasValidTelemetry() == false`;
- `hasAnySignal() == false`;
- `inputStatus() == Idle`;
- contadores zerados;
- porta UDP encaminhada para o receiver.

### `UT-TEL-02` - atualizar frame válido e getters derivados

Objetivo:
- validar o caminho principal de recepção e interpretação da telemetria.

Validações:
- atualização do último frame;
- timestamp preenchido a partir de `IClock`;
- getters de velocidade, RPM, marcha, volta e posição;
- clamp de combustível em `0..1`;
- conversão de Fahrenheit para Celsius.

### `UT-TEL-03` - contabilizar pacote descartado e erro de recepção

Objetivo:
- validar robustez de entrada contra falhas de comunicação.

Validações:
- incremento de `discardedPacketCount()`;
- incremento de `receiveErrorCount()`;
- transição de `inputStatus()` para `PacketDiscarded` e `ReceiveError`.

### `UT-TEL-04` - invalidar sinal após timeout

Objetivo:
- garantir que o sistema não use telemetria obsoleta.

Validações:
- perda de `hasValidTelemetry()` após o timeout;
- perda de `hasAnySignal()` após o timeout;
- `inputStatus()` passa para `NoPacket`.

### `UT-TEL-05` - preservar último frame válido quando um pacote falha na decodificação

Objetivo:
- garantir estabilidade da saída quando um pacote isolado está corrompido.

Validações:
- o último frame válido permanece disponível;
- o timestamp do frame válido anterior é preservado.

## Suíte `test_speed_gauge`

Arquivo: `test/test_speed_gauge/test_main.cpp`

### `UT-SPD-01` - calibrar e zerar o ponteiro na inicialização

Objetivo:
- validar o fluxo básico de `begin()` e calibração.

Validações:
- chamadas ao motor;
- posição alvo final em zero.

### `UT-SPD-02` - aplicar suavização em mudanças pequenas

Objetivo:
- validar a lógica de amortecimento do velocímetro.

Validações:
- alvo em steps menor que o alvo bruto;
- uso da velocidade normal do motor.

### `UT-SPD-03` - usar velocidade rápida em subida brusca

Objetivo:
- validar a resposta a aceleração intensa.

Validações:
- seleção de `fastStepsPerSec`;
- manutenção da suavização configurada.

### `UT-SPD-04` - usar alvo direto em queda brusca

Objetivo:
- validar a resposta em desaceleração forte.

Validações:
- bypass da suavização;
- alvo final direto no novo valor;
- uso da velocidade rápida.

### `UT-SPD-05` - aceitar apenas sinal de velocidade

Objetivo:
- garantir que o instrumento ignore sinais que não pertencem ao velocímetro.

Validações:
- `supports()`;
- `apply()` ignora `Rpm` e aplica `SpeedKmh`.

## Suíte `test_rpm_gauge`

Arquivo: `test/test_rpm_gauge/test_main.cpp`

### `UT-RPM-01` - calibrar e zerar o conta-giros na inicialização

Objetivo:
- validar o fluxo básico de `begin()` e calibração do instrumento de RPM.

Validações:
- chamadas esperadas ao motor;
- posição alvo final em zero.

### `UT-RPM-02` - aplicar suavização em mudanças pequenas de RPM

Objetivo:
- validar a lógica de amortecimento do conta-giros.

Validações:
- alvo em steps suavizado;
- uso da velocidade normal do motor.

### `UT-RPM-03` - usar velocidade rápida em subida brusca de RPM

Objetivo:
- validar a resposta a aceleração forte do motor.

Validações:
- seleção de `fastStepsPerSec`;
- atualização direta do alvo quando a variação é grande.

### `UT-RPM-04` - usar alvo direto em queda brusca de RPM

Objetivo:
- validar a resposta a desaceleração intensa.

Validações:
- bypass da suavização;
- alvo final direto no novo valor.

### `UT-RPM-05` - aceitar apenas sinal de RPM

Objetivo:
- garantir que o instrumento ignore sinais incompatíveis.

Validações:
- `supports()` para `Rpm`;
- `apply()` ignora outros tipos de sinal.

## Suíte `test_fuel_gauge`

Arquivo: `test/test_fuel_gauge/test_main.cpp`

### `UT-FUEL-01` - calibrar e zerar o indicador de combustível

Objetivo:
- validar o fluxo de `begin()` e calibração do medidor de combustível.

Validações:
- chamadas esperadas ao motor;
- posição alvo final em zero.

### `UT-FUEL-02` - aplicar primeira leitura sem suavização

Objetivo:
- garantir que a primeira leitura útil posicione o ponteiro diretamente.

Validações:
- alvo final igual ao valor de entrada convertido;
- uso da velocidade rápida.

### `UT-FUEL-03` - aplicar suavização em pequenas variações

Objetivo:
- validar o amortecimento de leituras consecutivas próximas.

Validações:
- alvo suavizado;
- uso da velocidade normal.

### `UT-FUEL-04` - tratar queda brusca com alvo direto

Objetivo:
- validar a reação a queda forte do nível de combustível.

Validações:
- alvo direto no novo valor;
- uso da velocidade rápida.

### `UT-FUEL-05` - limitar valores inválidos ou fora do intervalo

Objetivo:
- garantir robustez contra leituras fora de `0..1`.

Validações:
- clamp superior e inferior;
- posição final coerente com os limites configurados.

## Suíte `test_tire_temp_gauge`

Arquivo: `test/test_tire_temp_gauge/test_main.cpp`

### `UT-TEMP-01` - calibrar com recuo de estabilização

Objetivo:
- validar a calibração específica do indicador de temperatura dos pneus.

Validações:
- recuo inicial até o batente;
- backoff configurado;
- posição alvo final em zero.

### `UT-TEMP-02` - aplicar primeira temperatura sem suavização

Objetivo:
- garantir posicionamento inicial direto do ponteiro.

Validações:
- conversão direta de temperatura em steps;
- parâmetros de movimento enviados ao motor.

### `UT-TEMP-03` - suavizar leituras subsequentes

Objetivo:
- validar o filtro aplicado nas leituras seguintes de temperatura.

Validações:
- alvo intermediário em vez do alvo bruto.

### `UT-TEMP-04` - inverter direção de indicação quando configurado

Objetivo:
- validar a configuração de orientação do mostrador.

Validações:
- steps negativos ou invertidos quando `invertIndicationDirection` está ativo.

### `UT-TEMP-05` - aceitar apenas sinal de temperatura e limitar valores

Objetivo:
- garantir que o instrumento ignore sinais incorretos e respeite os limites.

Validações:
- `supports()` para `TireTemperatureC`;
- `apply()` ignora outros sinais;
- clamp no limite máximo.

## Suíte `test_udp_receiver`

Arquivo: `test/test_udp_receiver/test_main.cpp`

### `UT-UDP-01` - encaminhar porta e resultado do `begin()`

Objetivo:
- validar o contrato básico de inicialização do adaptador UDP.

Validações:
- porta recebida pelo `WiFiUDP`;
- retorno do `begin()` respeita o resultado do backend.

### `UT-UDP-02` - retornar `NoPacket` quando não há pacote disponível

Objetivo:
- garantir que o adaptador represente corretamente ausência de dados.

Validações:
- `PacketReceiveStatus::NoPacket`;
- ausência de tentativa de leitura do payload.

### `UT-UDP-03` - copiar pacote válido para o buffer de destino

Objetivo:
- validar o caminho nominal de leitura UDP.

Validações:
- bytes copiados corretamente;
- `bytesRead` coerente com o payload recebido.

### `UT-UDP-04` - descartar pacote maior que o buffer

Objetivo:
- validar proteção contra estouro do buffer de recepção.

Validações:
- retorno `PacketTooLarge`;
- descarte completo do payload do pacote.

### `UT-UDP-05` - reportar erro de leitura do backend

Objetivo:
- validar robustez diante de falha ao ler o payload UDP.

Validações:
- retorno `ReceiveError`;
- chamada ao método de leitura do backend.

## Suíte `test_gpio_button_input`

Arquivo: `test/test_gpio_button_input/test_main.cpp`

### `UT-BTN-01` - configurar pull-up e estado inicial do botão

Objetivo:
- validar inicialização do botão em modo ativo em nível baixo.

Validações:
- `pinMode()` com `INPUT_PULLUP`;
- ausência de evento espúrio na inicialização.

### `UT-BTN-02` - emitir evento de pressão somente após debounce

Objetivo:
- garantir que o evento só ocorra após estabilização do sinal.

Validações:
- ausência de evento antes da janela de debounce;
- ocorrência de um único evento após a estabilização.

### `UT-BTN-03` - ignorar bouncing antes da estabilização

Objetivo:
- validar supressão de ruído mecânico do botão.

Validações:
- ausência de evento durante oscilações rápidas;
- evento somente após o último nível estável respeitar o debounce.

### `UT-BTN-04` - suportar botão ativo em nível alto com pull-down

Objetivo:
- validar a configuração alternativa de polaridade elétrica.

Validações:
- `pinMode()` com `INPUT_PULLDOWN`;
- evento de pressão detectado em nível alto estável.

### `UT-BTN-05` - ignorar configuração com pino inválido

Objetivo:
- garantir comportamento seguro quando o botão não está configurado.

Validações:
- ausência de acesso indevido a GPIO;
- ausência de evento de pressão.

## Suíte `test_telemetry_pipeline`

Arquivo: `test/test_telemetry_pipeline/test_main.cpp`

### `IT-PIPE-01` - decodificar pacote real do Forza pelo serviço de telemetria

Objetivo:
- validar o pipeline real `receiver fake -> decoder real -> service`.

Validações:
- atualização do frame final;
- conversão de velocidade;
- leitura de marcha, volta, posição, combustível e temperaturas.

### `IT-PIPE-02` - preservar último frame válido após pacote inválido

Objetivo:
- garantir estabilidade do pipeline diante de um pacote curto/corrompido.

Validações:
- manutenção do último frame válido;
- preservação do timestamp anterior.

### `IT-PIPE-03` - recuperar telemetria válida após timeout

Objetivo:
- validar recuperação do pipeline após perda temporária de sinal.

Validações:
- invalidação correta após timeout;
- retomada correta após novo pacote válido.

## Suíte `test_instrument_cluster`

Arquivo: `test/test_instrument_cluster/test_main.cpp`

### `UT-CLS-01` - propagar `begin()` e `tick()`

Objetivo:
- validar a orquestração básica do cluster.

Validações:
- todos os instrumentos registrados recebem `begin()`;
- todos os instrumentos registrados recebem `tick()`.

### `UT-CLS-02` - despachar sinal somente para instrumentos compatíveis

Objetivo:
- validar o roteamento de sinais por tipo.

Validações:
- velocidade vai apenas ao velocímetro;
- RPM vai apenas ao tacômetro;
- combustível vai apenas ao medidor compatível.

## Suíte `test_app`

Arquivo: `test/test_app/test_main.cpp`

### `CT-APP-01` - exibir tela de portal quando o portal está ativo

Objetivo:
- validar o comportamento de inicialização em modo de configuração Wi-Fi.

Validações:
- mensagem correta no display;
- ausência de inicialização da telemetria e dos instrumentos.

### `CT-APP-02` - iniciar runtime quando o Wi-Fi já está disponível

Objetivo:
- validar o caminho feliz de inicialização da aplicação.

Validações:
- inicialização do cluster e da telemetria;
- primeira atualização de status da UI;
- cópia de SSID e IP para `UiStatus`.

### `CT-APP-03` - iniciar runtime quando o Wi-Fi conecta depois

Objetivo:
- validar a transição da tela de espera para o modo operacional.

Validações:
- segunda chamada de `begin()` no display;
- inicialização da telemetria e do cluster após a conexão.

### `CT-APP-04` - atualizar instrumentos e UI durante o loop

Objetivo:
- validar o ciclo principal da aplicação.

Validações:
- chamada de `tick()` da telemetria;
- encaminhamento de velocidade, RPM, combustível e temperatura ao cluster;
- atualização de `UiStatus` com sinal, descartes, erros, marcha e volta.

### `CT-APP-05` - trocar layout ao pressionar botão

Objetivo:
- validar a navegação de telas durante a execução.

Validações:
- leitura do botão;
- chamada de `nextLayout()`;
- atualização imediata da UI.

## Suíte `test_smoke_esp32_runtime`

Arquivo: `test/test_smoke_esp32_runtime/test_main.cpp`

### `ST-ESP-01` - subir runtime e processar um tick no ESP32

Objetivo:
- garantir que o fluxo principal executa no microcontrolador sem falha grave.

Validações:
- inicialização da telemetria;
- inicialização do cluster;
- processamento de um ciclo do loop.

### `ST-ESP-02` - exibir tela de portal no ESP32

Objetivo:
- garantir que o fluxo alternativo de configuração também sobe corretamente no
  microcontrolador.

Validações:
- renderização da mensagem principal de portal;
- ausência de inicialização da telemetria nesse modo.

## Suíte `test_ui_status`

Arquivo: `test/test_ui_status/test_main.cpp`

### `UT-UI-01` - truncar textos longos sem perder terminador nulo

Objetivo:
- validar segurança de cópia de SSID e IP para buffers fixos.

Validações:
- truncamento no limite do buffer;
- preservação do `'\0'` final.

### `UT-UI-02` - aceitar ponteiro nulo e limpar destino

Objetivo:
- validar robustez da rotina de cópia textual.

Validações:
- `ssid` e `ip` ficam vazios ao receber `nullptr`.

### `UT-UI-03` - ignorar alvo nulo ou capacidade zero

Objetivo:
- garantir que a rotina privada não corrompe memória em chamadas inválidas.

Validação:
- buffer previamente preenchido é preservado.

## Suíte `test_protocol_regression`

Arquivo: `test/test_protocol_regression/test_main.cpp`

### `IT-REG-01` - manter compatibilidade com fixture nominal

Objetivo:
- garantir que offsets e conversões do protocolo não regrediram.

Validações:
- velocidade, marcha, volta, posição, combustível e temperatura média.

### `IT-REG-02` - preservar mapeamento de ré e clamp de combustível

Objetivo:
- validar regra específica de marcha zero e clamp de combustível acima de `1`.

Validações:
- `gear == -1`;
- `fuelLevel() == 1.0f`.

### `IT-REG-03` - preservar leituras de hotlap em fixture de alta velocidade

Objetivo:
- manter estabilidade da interpretação em cenário extremo do protocolo.

Validações:
- velocidade alta;
- marcha alta;
- temperatura média elevada.

## Suíte `test_telemetry_robustness`

Arquivo: `test/test_telemetry_robustness/test_main.cpp`

### `IT-ROB-01` - usar o último frame válido em burst curto

Objetivo:
- validar que o serviço consome múltiplos pacotes no mesmo `tick()`.

Validações:
- número de decodificações;
- permanência do frame mais recente.

### `IT-ROB-02` - respeitar `PACKET_DRAIN_LIMIT` e continuar no `tick()` seguinte

Objetivo:
- garantir previsibilidade do loop mesmo sob burst acima do limite.

Validações:
- processamento de `64` pacotes no primeiro ciclo;
- processamento do restante no segundo ciclo.

### `IT-ROB-03` - sobreviver a ciclos repetidos de timeout e recuperação

Objetivo:
- validar robustez temporal em execução prolongada.

Validações:
- invalidação após timeout;
- recuperação após novo pacote válido;
- contadores permanecem consistentes ao longo de vários ciclos.

## Suíte `test_udp_pipeline_esp32`

Arquivo: `test/test_udp_pipeline_esp32/test_main.cpp`

### `IT-ESPUDP-01` - receber pacote válido por `WiFiUDP` real

Objetivo:
- validar integração real entre `WiFiUDP`, `UdpReceiver`, `Forza7Decoder` e
  `TelemetryService` no ESP32.

Validações:
- recepção do pacote;
- atualização de velocidade, marcha e volta.

### `IT-ESPUDP-02` - descartar pacote oversized e recuperar no pacote seguinte

Objetivo:
- validar robustez da recepção real no hardware.

Validações:
- incremento de descartes;
- recuperação com novo pacote válido.

### `IT-ESPUDP-03` - invalidar por timeout e recuperar com novo pacote

Objetivo:
- validar perda e recuperação de sinal com rede real.

Validações:
- timeout de sinal;
- nova recepção válida após a perda.
