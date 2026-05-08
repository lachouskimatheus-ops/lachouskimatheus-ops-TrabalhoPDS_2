# 🃏 Truco (Paulista / Mineiro)

---

## 📌 Descrição

O módulo Truco é responsável por implementar a lógica do clássico jogo de cartas, utilizando as classes base `Mesa`, `Baralho`, `Carta` e a especialização `Jogador_Truco` definidas no projeto.

O foco deste módulo é gerenciar a dinâmica de rodadas e turnos, aplicando as regras de força de cartas e manilhas através de um sistema de arbitragem, permitindo alternar entre as variantes Paulista e Mineira.

---

## 🎯 Requisitos (E5)

### 🔹 Requisitos Funcionais

- O sistema deve permitir a criação de partidas de Truco com 4 jogadores divididos em duas equipes.
- O sistema deve distribuir exatamente 3 cartas para cada jogador no início de cada rodada.
- O sistema deve gerenciar a lógica do "Vira" para determinar as manilhas na variação Paulista.
- O sistema deve permitir que o jogador escolha uma carta da mão para jogar na mesa através de um índice.
- O sistema deve validar a força das cartas jogadas, considerando o peso do naipe em caso de manilhas.
- O sistema deve controlar a pontuação das equipes até que uma atinja o limite de pontos da partida.
- O sistema deve garantir a limpeza das mãos dos jogadores e da mesa ao fim de cada queda.

---

### 🔹 Requisitos Não Funcionais

- O módulo deve reutilizar as classes `Baralho`, `Carta` e a estrutura do `Menu`.
- O código deve ser modular, separado em arquivos `.hpp` e `.cpp`.
- A lógica de decisão de vencedores deve ser desacoplada da Mesa através da classe abstrata `Juiz`.
- O sistema deve utilizar alocação dinâmica com gerenciamento seguro via destrutores virtuais.
- A lógica do jogo deve ser independente de interface gráfica, pronta para futura integração em rede.

---

## 🧠 Modelagem

### 🧩 Classe Mesa

#### 🔹 Responsabilidades

- Gerenciar o ciclo de vida da partida (fases) de cada rodada.
- Comandar o `BaralhoSujo` para embaralhar e distribuir as cartas.
- Armazenar os ponteiros dos jogadores e a carta "Vira" da rodada.
- Avaliar as cartas jogadas na mesa para determinar o vencedor de cada turno.
- Alimentar o placar com os pontos das equipes.

#### 🔗 Colabora com:

- `Jogador_Truco`
- `BaralhoSujo`
- `Carta`
- `Juiz`

---

### 🧩 Classe Jogador_Truco

#### 🔹 Responsabilidades

- Armazenar o nome e o conjunto de cartas (`mao`) do participante.
- Gerenciar o recebimento de novas cartas a cada rodada.
- Executar a ação de jogar uma carta da mão para a mesa via índice.

#### 🔗 Colabora com:

- `Carta`

---

### 🧩 Classes JuizPaulista / JuizMineiro

#### 🔹 Responsabilidades

- Implementar a lógica de comparação de força de cartas conforme a variante.
- Identificar manilhas baseadas no "Vira" (Paulista) ou fixas (Mineira).

---

### 🔠 Enumeração Naipe

Define a hierarquia de desempate para as manilhas:

- `paus`
- `copas`
- `espadas`
- `ouros`

---

## ⚙️ Estratégia de Implementação

### 🧱 Estruturas de dados:

- `vector<Jogador_Truco*>` → Lista de jogadores sentados à mesa
- `vector<Carta*>` → Pilha de cartas jogadas no centro da mesa (para resolver a vaza)
- `Baralho*` → Ponteiro para a implementação específica de cartas em jogo

### 🎮 Fluxo do jogo:

- Inicialização da Mesa via injeção de dependência (Juiz e Baralho)
- Distribuição de 3 cartas por jogador e definição do Vira
- Jogo de cartas respeitando turnos e avaliação de força pelo Juiz
- Atualização do placar geral da partida

### 🧠 Regras:

- O Juiz realiza a verificação de "Manilha" e "Força Bruta"
- Manilhas possuem peso de naipe; cartas comuns seguem o valor de face
- A Mesa processa e define os pontos das equipes (os jogadores não se auto-pontuam)

---

## 🧩 Observações

- O módulo foi projetado para escalabilidade de Polimorfismo, permitindo instanciar as partidas genéricas através de ponteiros para a classe abstrata `Juiz`.
- O uso de alocação dinâmica e ponteiros crus garante alta performance e controle de memória.
- O sistema possui limpezas de memória controladas via destrutores virtuais implementados no projeto.
