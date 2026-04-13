# 📄 Documento de Sprints  
**Projeto:** Sistema Háptico Modular para Sim Racing  
**Autor:** Cairon Ferreira Prado  
**Curso:** Análise e Desenvolvimento de Sistemas  
**Instituição:** IFPI  

---

## Metodologia de Desenvolvimento

O desenvolvimento do sistema segue uma abordagem **iterativa e incremental**, inspirada em metodologias ágeis (Scrum adaptado ao contexto acadêmico).  
O projeto foi dividido em **sprints**, cada uma contendo objetivos claros, backlog definido, validação funcional e registro de resultados.

As validações realizadas até o momento são majoritariamente **manuais**, com planejamento explícito para evolução futura para **testes automatizados**, conforme a maturidade do sistema.

---

## 📊 Visão Geral das Sprints

| Sprint | Objetivo Principal | Status |
|------|-------------------|--------|
| Sprint 1 | Comunicação UDP com o simulador | ✅ Concluída |
| Sprint 2 | Decodificação e filtragem da telemetria | ✅ Concluída |
| Sprint 3 | Exibição de dados no display | ✅ Concluída |
| Sprint 4 | Controle físico dos instrumentos do painel | 🔄 Em andamento |
| Sprint 5 | Integração, testes e validação final | ⏳ Planejada |

---

## Sprint 1 – Comunicação UDP com o Simulador

**Período:** 03/11/2025 – 24/11/2025  

### Objetivo
Estabelecer comunicação via protocolo UDP entre o simulador de corrida e o microcontrolador ESP32.

### Backlog Planejado
- Configuração da rede Wi-Fi no ESP32  
- Implementação do receptor de pacotes UDP  
- Recepção contínua de dados brutos  

### Atividades Executadas
- Implementação do módulo `UdpReceiver`
- Configuração da porta UDP do simulador
- Validação da recepção por meio de logs seriais

### Estratégia de Testes
- Testes manuais por observação dos logs seriais
- Envio de pacotes reais a partir do simulador
- Verificação de estabilidade da comunicação

### Critérios de Aceitação
- O ESP32 deve receber pacotes UDP continuamente
- Não deve ocorrer travamento do sistema durante a recepção
- Logs devem indicar recepção ativa de dados

### Resultados Obtidos
- Comunicação UDP funcional
- Recepção contínua e estável de pacotes

### Dificuldades Encontradas
- Configuração de rede local
- Ajustes de IP e porta do simulador

### Evidências
- Logs seriais
- Commits no repositório do projeto

---

## Sprint 2 – Decodificação e Filtragem da Telemetria

**Período:** 25/11/2025 – 19/12/2025  

### Objetivo
Decodificar os pacotes UDP recebidos e extrair apenas os dados relevantes para o sistema.

### Backlog Planejado
- Análise da estrutura dos pacotes do simulador
- Implementação do decoder de telemetria
- Filtragem dos campos necessários

### Atividades Executadas
- Implementação do `Forza7Decoder`
- Uso de offsets para leitura correta dos dados
- Separação de variáveis como velocidade, RPM e estado do veículo

### Estratégia de Testes
- Testes manuais com dados reais do simulador
- Comparação visual dos valores exibidos com o comportamento esperado
- Verificação de consistência dos dados ao longo do tempo

### Critérios de Aceitação
- Os campos de velocidade e RPM devem ser decodificados corretamente
- Valores inválidos devem ser descartados
- O sistema não deve falhar com pacotes inesperados

### Resultados Obtidos
- Decodificação correta dos dados de telemetria
- Dados considerados semanticamente válidos

### Dificuldades Encontradas
- Diferença entre os modos DASH e SLED do simulador
- Ajustes finos nos offsets dos pacotes

### Evidências
- Código-fonte do decoder
- Logs e testes em sessões reais de corrida

---

## Sprint 3 – Exibição de Dados no Display

**Período:** 19/12/2025 – 26/12/2025  

### Objetivo
Exibir informações essenciais de telemetria em um display embarcado de forma clara e em tempo real.

### Backlog Planejado
- Integração do display TFT
- Definição das informações exibidas
- Atualização dinâmica dos dados

### Atividades Executadas
- Implementação do `DisplayService`
- Exibição de velocidade, status da conexão e dados básicos
- Sincronização entre recepção UDP e atualização visual

### Estratégia de Testes
- Testes manuais com observação visual do display
- Verificação de atualização contínua sem travamentos
- Ajuste de taxa de atualização para evitar flickering

### Critérios de Aceitação
- O display deve atualizar os dados em tempo real
- As informações exibidas devem ser coerentes com a telemetria recebida
- O sistema não deve apresentar atrasos perceptíveis

### Resultados Obtidos
- Display funcional e responsivo
- Feedback visual imediato ao usuário

### Dificuldades Encontradas
- Gerenciamento do tempo de atualização do display
- Evitar bloqueio do loop principal

### Evidências
- Fotos do display em funcionamento
- Código do serviço de exibição

---

## Sprint 4 – Controle Físico dos Instrumentos do Painel (EM ANDAMENTO)

**Período:** 26/12/2025 – 23/01/2026  

### Objetivo
Controlar fisicamente os instrumentos do painel de acordo com os dados de telemetria recebidos.

### Backlog Planejado
- Mapeamento de dados de telemetria para movimentos físicos
- Implementação do controle de atuadores
- Definição de limites e calibração dos instrumentos

### Atividades Planejadas
- Controle de motores (ex.: ponteiros analógicos)
- Sincronização entre dados recebidos e resposta física
- Implementação de lógica de segurança

### Estratégia de Testes (Planejada)
- Testes manuais de movimento dos instrumentos
- Validação visual da correspondência dado → movimento
- Medição empírica de latência percebida

### Riscos Identificados
- Imprecisão mecânica
- Latência entre dado e resposta física
- Consumo energético dos atuadores

---

## Sprint 5 – Integração, Testes e Validação Final (PLANEJADA)

### Objetivo
Realizar a integração completa do sistema e validar seu funcionamento em ambiente real de simulação.

### Backlog Planejado
- Integração final hardware + software
- Testes prolongados em sessões de corrida
- Avaliação de estabilidade e desempenho
- Documentação dos resultados

### Estratégia de Testes Planejada
- Testes manuais em ambiente real
- Planejamento de testes automatizados para módulos críticos (decoder)
- Simulação controlada de pacotes UDP para validação futura

---

## Considerações Finais

A organização do desenvolvimento em sprints permitiu:
- Evolução incremental e controlada do sistema
- Validação contínua das funcionalidades implementadas
- Identificação clara de limitações e melhorias futuras
