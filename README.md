# TCC – Sistema Embarcado para Feedback Háptico em Sim Racing

Este repositório abrange toda a coleção de arquivos, códigos-fonte e documentação referente ao **Trabalho de Conclusão de Curso (TCC)**, desenvolvido na modalidade **Relatório Técnico de Software**.

O projeto propõe o desenvolvimento de um **dispositivo de sistema embarcado plug and play**, baseado em microcontrolador ESP32, destinado a aumentar a experiência imersiva de jogadores de simulação de corrida por meio de **feedback físico baseado em dados de telemetria**.

---

## Visão Geral do Projeto

O sistema recebe dados de telemetria disponibilizados por jogos de simulação de corrida (como velocidade, rotação do motor, marcha e eventos específicos), por meio de comunicação em rede utilizando o protocolo **UDP**, interpreta essas informações e aciona **atuadores físicos modulares**, como motores e displays, proporcionando uma experiência mais realista ao jogador.

O público-alvo do projeto é composto por **jogadores de simulação sem conhecimento técnico prévio** em programação, eletrônica ou sistemas embarcados, sendo a facilidade de uso um dos principais requisitos da solução.

---

## Objetivo Geral

Desenvolver um sistema embarcado capaz de receber e interpretar dados de telemetria de jogos de simulação de corrida e gerar feedback físico em tempo real, de forma automática e transparente ao usuário final.

---

## Escopo do Projeto (MVP)

Este repositório contempla a implementação de um **Produto Mínimo Viável (MVP)**, cujo objetivo é validar a viabilidade técnica da proposta apresentada no TCC.

### Funcionalidades contempladas no MVP

O MVP do projeto compromete-se a implementar **os requisitos funcionais até o RF10**, conforme definido na documentação de requisitos, incluindo:

- Conexão automática à rede local
- Comunicação com o jogo via protocolo UDP
- Recepção e interpretação de dados de telemetria
- Controle de indicadores físicos de:
  - velocidade
  - rotação do motor
  - marcha
- Indicação de status operacional do sistema
- Compatibilidade com a plataforma **Xbox Series**
- Inicialização automática do sistema
- Detecção automática de módulos conectados

Essas funcionalidades são consideradas **suficientes para demonstrar o funcionamento básico do sistema**, validar a arquitetura proposta e comprovar a viabilidade da solução.

---

## Funcionalidades Futuras

Os requisitos funcionais e não funcionais descritos na documentação que **excedem o RF10** são considerados **extensões futuras**, podendo ser implementados caso haja disponibilidade adicional de tempo e recursos.

Entre as possíveis evoluções do sistema, destacam-se:

- Compatibilidade com outras plataformas (PlayStation 4, PlayStation 5 e PC)
- Implementação de efeitos adicionais de vibração
- Suporte a outros simuladores de corrida
- Expansão do conjunto de atuadores modulares
- Aprimoramento da personalização dos efeitos físicos

Essas funcionalidades não fazem parte do escopo mínimo garantido do projeto.

---

## Arquitetura do Sistema

O sistema foi projetado seguindo uma **arquitetura modular**, composta por:

- Módulo de comunicação de rede (UDP)
- Módulo de interpretação de dados de telemetria
- Módulo de mapeamento telemetria → efeito físico
- Módulo de controle de atuadores
- Interface simples de indicação de status

Essa organização visa facilitar a manutenção, a expansão futura e a reutilização do sistema em outros contextos de simulação.

---

## Documentação

A documentação do projeto encontra-se organizada no diretório `docs/`, incluindo:

- Requisitos Funcionais
- Requisitos Não Funcionais
- Histórias de Usuário
- Casos de Uso

---

## Tecnologias Utilizadas

- ESP32
- C / C++ (firmware)
- Comunicação via UDP
- Motores de passo e servomotores
- Display OLED
- PlantUML para modelagem
- Markdown para documentação

---

## Observação Final

Este repositório representa um **projeto acadêmico**, cujo foco principal é a validação técnica e conceitual da solução proposta. A implementação foi deliberadamente delimitada a um MVP, conforme descrito na documentação do trabalho, não representando um produto final comercial.

---

## Autor

Trabalho de Conclusão de Curso desenvolvido como requisito parcial para obtenção do grau em **Análise e Desenvolvimento de Sistemas**.
