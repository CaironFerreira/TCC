# Elicitação de Requisitos

## 1. Requisitos Funcionais (RF)

### RF01 – Conexão automática à rede local
O sistema deve conectar-se automaticamente à rede local onde o jogo está sendo executado, sem exigir configuração manual por parte do usuário.

### RF02 – Comunicação com o jogo via UDP
O sistema deve estabelecer comunicação com o jogo de simulação por meio do protocolo UDP, permitindo o recebimento contínuo de dados de telemetria.

### RF03 – Interpretação dos dados recebidos
O sistema deve interpretar os dados de telemetria recebidos, convertendo-os em informações utilizáveis pelo módulo de controle de atuadores.

### RF04 – Controle de indicador de velocidade
O sistema deve controlar o motor do painel de instrumentos que indica a velocidade do veículo.

### RF05 – Controle de indicador de rotação do motor
O sistema deve controlar o motor do painel de instrumentos que indica a rotação do motor.

### RF06 – Controle de indicador de marcha
O sistema deve controlar o Display OLED presente no painel de instrumentos que indica a marcha em que o veículo está engatado.

### RF07 – Detecção automática de módulos conectados
O sistema deve identificar automaticamente a conexão e desconexão de atuadores modulares.

### RF08 – Inicialização automática do sistema
O sistema deve iniciar seu funcionamento automaticamente após a conexão do dispositivo à energia e à rede.

### RF09 – Indicação de status operacional
O sistema deve informar ao usuário seu estado de funcionamento (conectado, desconectado, erro), por meio de indicadores simples.

### RF10 – Compatibilidade com a plataforma Xbox Series
O sistema deve funcionar na plataforma Xbox Series.

### RF11 – Compatibilidade com a plataforma PlayStation 4
O sistema deve funcionar na plataforma PlayStation 4.

### RF12 – Compatibilidade com a plataforma PlayStation 5
O sistema deve funcionar na plataforma PlayStation 5.

### RF13 – Compatibilidade com a plataforma PC
O sistema deve funcionar na plataforma PC (Personal Computer).

### RF14 – Simular vibração do veículo ao sair da faixa principal
O sistema deve simular a vibração do veículo ao passar sobre a faixa zebrada controlando motores.

### RF15 – Simular vibração do veículo ao dar a partida
O sistema deve simular a vibração do veículo ao dar partida controlando motores.

## 2. Requisitos Não Funcionais (RNF)

### RNF01 – Facilidade de uso
O sistema deve ser operável por usuários sem conhecimento técnico prévio em programação, eletrônica ou redes.

### RNF02 – Baixa latência
O sistema deve apresentar tempo de resposta menor que 30ms para que o feedback físico seja percebido como imediato pelo usuário.

### RNF04 – Modularidade e expansibilidade
A arquitetura do sistema deve permitir a adição de novos atuadores sem necessidade de alterações estruturais significativas.

### RNF05 – Compatibilidade com o ambiente do usuário
O sistema deve funcionar nas plataformas Xbox Series, sem exigir hardware ou software adicional.

### RNF06 – Robustez
O sistema deve continuar operando corretamente mesmo diante de pequenas falhas de comunicação ou variações nos dados de telemetria.

### RNF08 – Escalabilidade funcional
O sistema deve permitir expansão futura para suporte a outros simuladores de corrida com características semelhantes.

### RNF09 – Manutenibilidade
O sistema deve possuir uma organização interna que facilite correções, melhorias e evolução do firmware.

### RNF10 – Confiabilidade física
O sistema não deve causar comportamento inesperado ou perigoso nos atuadores, garantindo segurança ao usuário durante o uso.

### RNF11 – Compatibilidade com a plataforma PlayStation 4
O sistema deve funcionar na plataforma PlayStation 4, sem exigir hardware ou software adicional.

### RNF12 – Compatibilidade com a plataforma PlayStation 5
O sistema deve funcionar na plataforma PlayStation 5, sem exigir hardware ou software adicional.

### RNF13 – Compatibilidade com a plataforma PC
O sistema deve funcionar na plataforma PC (Personal Computer), sem exigir hardware ou software adicional.

---
*Última atualização: 10 de dezembro de 2025*