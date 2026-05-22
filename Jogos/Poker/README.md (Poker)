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
- O módulo pode ser integrado ao menu principal para execução do jogo futuramente.