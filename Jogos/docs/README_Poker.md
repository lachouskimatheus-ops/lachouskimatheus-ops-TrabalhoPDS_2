# Poker

## Descrição

O módulo Poker é responsável por implementar a lógica básica do jogo de Poker, utilizando as classes de cartas e baralho definidas no Core do projeto.

O foco deste módulo é a avaliação da mão do jogador com base nas combinações clássicas do Poker.

---

## Requisitos (E5)

### Requisitos Funcionais

- O sistema deve permitir armazenar uma mão de até 5 cartas.
- O sistema deve permitir adicionar cartas à mão do jogador.
- O sistema deve identificar a força da mão com base nas regras do Poker.
- O sistema deve reconhecer combinações como:
  - Carta Alta
  - Par
  - Dois Pares
  - Trinca
  - Sequência
  - Flush
  - Full House
  - Quadra
  - Straight Flush
  - Royal Flush
- O sistema deve retornar o nome da jogada identificada.

---

### Requisitos Não Funcionais

- O módulo deve reutilizar as classes `Carta`, `Valor` e `Naipe` do Core.
- O código deve ser modular e separado em arquivos `.h` e `.cpp`.
- A lógica deve ser independente de interface gráfica.
- O código deve ser de fácil manutenção e extensão.

---

## Modelagem

### Classe Poker

#### Responsabilidades:
- Armazenar a mão do jogador (vetor de cartas)
- Avaliar a força da mão
- Identificar padrões de combinação (pares, sequência, flush, etc)
- Retornar o nome da jogada

#### Colabora com:
- `Carta`
- `Valor`
- `Naipe`

---

## Estratégia de Implementação

A avaliação da mão é feita utilizando:

- Conversão dos valores das cartas para números inteiros
- Contagem de frequência dos valores (para detectar pares, trincas, quadras)
- Ordenação dos valores (para detectar sequência)
- Verificação de igualdade de naipes (para detectar flush)

---

## Observações

- A implementação considera uma mão de exatamente 5 cartas.
- Casos especiais, como sequência com Ás baixo (A, 2, 3, 4, 5), são tratados.
- O módulo pode ser utilizado pelo menu principal e pela API do projeto.

---

## Integração com a Mesa e a API

Além da avaliação individual de mãos, o módulo utiliza `MesaPoker` para controlar a partida, os jogadores, as trocas e o resultado de cada rodada.

### Classe MesaPoker

#### Responsabilidades:

- Criar e distribuir as mãos dos jogadores.
- Controlar as fases da rodada.
- Registrar trocas de cartas.
- Executar trocas automáticas quando necessário.
- Comparar as mãos e definir o vencedor ou empate.
- Controlar rodadas e pontuação.

#### Colabora com:

- `Poker`
- `Baralho`
- `Carta`
- API multiplayer

### Arquivos principais da API

- `SalaPoker.hpp` / `SalaPoker.cpp`: mantém a mesa e associa jogadores às conexões.
- `PokerWebSocket.hpp` / `PokerWebSocket.cpp`: recebe ações do frontend e envia o estado da partida.
- `GerenciadorSalasPoker.hpp` / `GerenciadorSalasPoker.cpp`: cria e localiza salas.
- `PokerRoutes.hpp` / `PokerRoutes.cpp`: registra as rotas HTTP do Poker.

### Fluxo da comunicação

```text
Frontend → JSON/WebSocket → PokerWebSocket → SalaPoker → MesaPoker
MesaPoker → Estado atualizado em JSON → Frontend
```

A API recebe ações como entrada na sala, escolha das cartas para troca, confirmação da troca e solicitação de nova rodada. O backend controla a ordem das fases e impede que o frontend altere diretamente as regras ou as mãos.

## Requisitos funcionais adicionais

- Criar partidas para dois a quatro jogadores.
- Distribuir cinco cartas para cada participante.
- Permitir seleção, confirmação e cancelamento de trocas.
- Avaliar e comparar as mãos ao final da rodada.
- Informar vencedor ou empate.
- Permitir criação de salas e entrada de jogadores.
- Enviar a cada jogador apenas sua própria mão.

## Requisitos não funcionais adicionais

- Manter a avaliação das mãos no núcleo.
- Utilizar JSON e WebSocket na comunicação.
- Separar `Poker`, `MesaPoker`, sala, rotas e WebSocket.
- Validar no backend todas as ações recebidas.
- Ocultar as cartas dos adversários.

## Cartões CRC da API

### Classe SalaPoker

**Responsabilidades:**

- Manter a instância de `MesaPoker`.
- Registrar jogadores e conexões.
- Controlar entrada e reconexão.

**Colabora com:**

- `MesaPoker`
- `PokerWebSocket`
- `GerenciadorSalasPoker`

### Classe PokerWebSocket

**Responsabilidades:**

- Receber mensagens JSON do frontend.
- Encaminhar ações para a mesa.
- Enviar estados, resultados e erros.

**Colabora com:**

- `SalaPoker`
- `MesaPoker`
- `GerenciadorSalasPoker`

