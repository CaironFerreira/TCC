# Casos de Uso

---

## UC01 – Exibir velocidade no painel físico
**Ator:** Jogador de simulação  
**Objetivo:** Visualizar fisicamente a velocidade do veículo.  
**Resumo:** Sistema recebe telemetria → interpreta velocidade → aciona motor do painel → jogador visualiza velocidade.  
**RF Relacionados:** RF04  
**RNF Relacionados:** RNF01, RNF02  
**Histórias Relacionadas:** Story 1 (Jogador)

---

## UC02 – Exibir rotação do motor no painel físico
**Ator:** Jogador de simulação  
**Objetivo:** Visualizar a rotação do motor em painel físico.  
**Resumo:** Sistema recebe RPM → interpreta valor → aciona motor do painel → jogador acompanha rotação.  
**RF Relacionados:** RF05  
**RNF Relacionados:** RNF01, RNF02  
**Histórias Relacionadas:** Story 2 (Jogador)

---

## UC03 – Exibir marcha no display OLED
**Ator:** Jogador de simulação  
**Objetivo:** Visualizar a marcha engatada fisicamente.  
**Resumo:** Sistema recebe marcha → interpreta dado → atualiza display OLED.  
**RF Relacionados:** RF06  
**RNF Relacionados:** RNF01  
**Histórias Relacionadas:** Story 3 (Jogador)

---

## UC04 – Consultar status operacional do sistema
**Ator:** Jogador de simulação  
**Objetivo:** Verificar se o sistema está funcionando corretamente.  
**Resumo:** Sistema inicializa → verifica conexão → atualiza indicadores → jogador consulta status.  
**RF Relacionados:** RF09  
**RNF Relacionados:** RNF01, RNF06  
**Histórias Relacionadas:** Story 4 (Jogador)

---

## UC05 – Operar sistema na plataforma Xbox Series
**Ator:** Jogador de simulação  
**Objetivo:** Utilizar o sistema no Xbox Series.  
**Resumo:** Jogador conecta dispositivo → sistema conecta à rede → recebe telemetria → aciona atuadores.  
**RF Relacionados:** RF10  
**RNF Relacionados:** RNF05  
**Histórias Relacionadas:** Story 5 (Jogador)

---

## UC06 – Operar sistema na plataforma PlayStation 4
**Ator:** Jogador de simulação  
**Objetivo:** Utilizar o sistema no PlayStation 4.  
**Resumo:** Jogador conecta dispositivo → sistema opera sem software adicional → efeitos físicos são gerados.  
**RF Relacionados:** RF11  
**RNF Relacionados:** RNF11  
**Histórias Relacionadas:** Story 6 (Jogador)

---

## UC07 – Operar sistema na plataforma PlayStation 5
**Ator:** Jogador de simulação  
**Objetivo:** Utilizar o sistema no PlayStation 5.  
**Resumo:** Sistema conecta à rede → recebe telemetria → controla painel e atuadores.  
**RF Relacionados:** RF12  
**RNF Relacionados:** RNF12  
**Histórias Relacionadas:** Story 7 (Jogador)

---

## UC08 – Operar sistema na plataforma PC
**Ator:** Jogador de simulação  
**Objetivo:** Utilizar o sistema no PC.  
**Resumo:** Sistema conecta à rede local → recebe telemetria via UDP → controla painel e vibrações.  
**RF Relacionados:** RF13  
**RNF Relacionados:** RNF13  
**Histórias Relacionadas:** Story 8 (Jogador)

---

## UC09 – Simular vibração ao sair da faixa principal
**Ator:** Jogador de simulação  
**Objetivo:** Sentir vibração ao passar sobre a faixa zebrada.  
**Resumo:** Sistema detecta condição de pista → aciona motores → jogador sente vibração.  
**RF Relacionados:** RF14  
**RNF Relacionados:** RNF02, RNF10  
**Histórias Relacionadas:** Story 9 (Jogador)

---

## UC10 – Simular vibração ao dar a partida
**Ator:** Jogador de simulação  
**Objetivo:** Sentir vibração inicial ao dar partida no veículo.  
**Resumo:** Sistema detecta evento de partida → aciona motores por curto período.  
**RF Relacionados:** RF15  
**RNF Relacionados:** RNF02, RNF10  
**Histórias Relacionadas:** Story 10 (Jogador)

---

## UC11 – Conectar automaticamente à rede local
**Ator:** Sistema  
**Objetivo:** Conectar-se automaticamente à rede local.  
**Resumo:** Sistema inicia → conecta à rede → sinaliza status.  
**RF Relacionados:** RF01  
**RNF Relacionados:** RNF01, RNF06  
**Histórias Relacionadas:** Story 1 (Sistema)

---

## UC12 – Receber telemetria via UDP
**Ator:** Sistema  
**Objetivo:** Receber dados do jogo continuamente.  
**Resumo:** Sistema abre porta UDP → recebe pacotes → encaminha para processamento.  
**RF Relacionados:** RF02  
**RNF Relacionados:** RNF02, RNF06  
**Histórias Relacionadas:** Story 2 (Sistema)

---

## UC13 – Interpretar dados de telemetria
**Ator:** Sistema  
**Objetivo:** Converter dados recebidos em comandos para atuadores.  
**Resumo:** Sistema processa pacotes → extrai dados → gera comandos.  
**RF Relacionados:** RF03  
**RNF Relacionados:** RNF02, RNF09  
**Histórias Relacionadas:** Story 3 (Sistema)

---

## UC14 – Detectar módulos conectados
**Ator:** Sistema  
**Objetivo:** Identificar conexão e desconexão de módulos.  
**Resumo:** Sistema monitora conexões → habilita/desabilita módulos automaticamente.  
**RF Relacionados:** RF07  
**RNF Relacionados:** RNF04  
**Histórias Relacionadas:** Story 4 (Sistema)

---

## UC15 – Inicialização automática do sistema
**Ator:** Sistema  
**Objetivo:** Iniciar automaticamente após receber energia.  
**Resumo:** Sistema inicializa → conecta à rede → entra em modo operacional.  
**RF Relacionados:** RF08  
**RNF Relacionados:** RNF01, RNF06  
**Histórias Relacionadas:** Story 5 (Sistema)
