# 🌐 Protocolo de Comunicação

## 1. Visão Geral

Este documento descreve o protocolo de comunicação utilizado para a transmissão
de dados de telemetria entre o simulador de corrida e o sistema embarcado.

O protocolo adotado é o **UDP (User Datagram Protocol)**, protocolo padrão de baixa latência e simplicidade em simuladores, características adequadas para aplicações em
tempo real.

---

## 2. Protocolo de Transporte

- **Protocolo:** UDP
- **Modelo:** Comunicação unidirecional (simulador → dispositivo)
- **Rede:** Local (LAN)
- **Porta:** configurável no simulador e no sistema embarcado

---

## 3. Estrutura Geral do Pacote

Os pacotes UDP são compostos por uma sequência fixa de bytes, organizados em
campos com tamanhos e posições predefinidas.

Cada campo representa uma variável específica do estado do veículo.

---

## 4. Organização dos Dados (Conceito de Offset)

Os dados são acessados com base em **offsets**, que representam a posição, em bytes,
de cada campo dentro do pacote.

Exemplo conceitual:

| Offset (bytes) | Campo            | Tipo  | Descrição                    |
|---------------|------------------|-------|------------------------------|
| 0             | Velocidade       | float | Velocidade do veículo        |
| 4             | RPM              | float | Rotação do motor             |
| 8             | Estado do veículo| int   | Flags de status              |

---

## 5. Processo de Decodificação

O processo de decodificação segue as etapas:

1. Recepção do pacote UDP completo.
2. Validação do tamanho mínimo esperado.
3. Leitura dos campos utilizando seus offsets.
4. Conversão dos dados binários para tipos nativos.
5. Validação semântica dos valores obtidos.

---

## 6. Validação dos Dados

Para garantir a confiabilidade das informações processadas, são aplicadas
validações como:
- Verificação de valores fora de faixa
- Descarte de pacotes inconsistentes
- Tratamento seguro de dados inesperados

---

## 7. Considerações

A utilização de um protocolo simples e eficiente permite:
- Baixa latência na transmissão
- Processamento rápido no microcontrolador
- Compatibilidade com diferentes simuladores no futuro
