# 🃏 Paciência (Solitaire)

## 📌 Descrição

O módulo Paciência é responsável por implementar a lógica do jogo clássico de cartas *Solitaire*, utilizando as classes Carta e Baralho definidas no Core do projeto.

O foco deste módulo é gerenciar o estado completo do jogo e permitir a execução das regras do Paciência, incluindo movimentação de cartas, compra do estoque, organização das pilhas e verificação de vitória.

---

## 🎯 Requisitos (E5)

### 🔹 Requisitos Funcionais

- O sistema deve representar o estado completo do jogo de Paciência.
- O sistema deve permitir a distribuição inicial das cartas em 7 colunas.
- O sistema deve permitir comprar cartas do estoque para o descarte.
- O sistema deve permitir mover cartas entre:
  - colunas
  - descarte
  - fundações
- O sistema deve validar movimentos de acordo com as regras do jogo:
  - alternância de cores nas colunas
  - ordem decrescente nas colunas
  - ordem crescente nas fundações
  - mesmo naipe nas fundações
- O sistema deve permitir reutilizar o descarte quando o estoque estiver vazio.
- O sistema deve identificar quando o jogador venceu o jogo.
- O sistema deve permitir desfazer jogadas (undo).

---

### 🔹 Requisitos Não Funcionais

- O módulo deve reutilizar as classes `Carta`, `Valor` e `Naipe` do Core.
- O código deve ser modular, separado em arquivos `.h` e `.cpp`.
- A lógica do jogo deve ser independente de interface gráfica.
- O sistema deve ser de fácil manutenção e extensão.
- O código deve evitar duplicação de lógica (uso de função genérica `mover`).
- O histórico de jogadas deve permitir retorno ao estado anterior do jogo.

---

## 🧠 Modelagem

### 🧩 Classe Paciencia

#### 🔹 Responsabilidades

- Armazenar o estado completo do jogo:
  - colunas (7 pilhas)
  - fundações (4 pilhas)
  - descarte
  - estoque
- Controlar regras do jogo
- Executar movimentação de cartas
- Gerenciar compra de cartas
- Gerenciar histórico de jogadas (undo)
- Verificar condição de vitória

---

### 🧾 Estrutura EstadoJogo

Responsável por armazenar uma cópia completa do estado do jogo.

Contém:
- colunas
- fundações
- descarte
- estoque

👉 Utilizada para implementação do sistema de desfazer jogadas.

---

### 🔠 Enumeração TipoPilha

Define os tipos de origem e destino de movimentação:

- Coluna
- Descarte
- Fundação

---

### 🔗 Colabora com:

- `Carta`
- `Baralho`
- `Valor`
- `Naipe`

---

## ⚙️ Estratégia de Implementação

A lógica do jogo é baseada em:

### 🧱 Estruturas de dados:
- `vector<vector<Carta>>` → colunas
- `vector<vector<Carta>>` → fundações
- `vector<Carta>` → descarte
- `Baralho` → estoque

### 🎮 Fluxo do jogo:
- distribuição inicial das cartas
- compra de cartas do estoque
- movimentação entre pilhas

### 🧠 Regras:
- alternância de cores nas colunas
- ordem decrescente nas colunas
- ordem crescente nas fundações

### ⏪ Sistema de histórico:
- uso de `stack<EstadoJogo>`
- salvar estado antes de cada jogada
- restaurar estado no desfazer

---

## 🧩 Observações

- O jogo segue a versão clássica Klondike
- Apenas a última carta de uma coluna pode ser movida (regra dependente da classe `Carta`)
- O sistema de undo permite retorno completo do estado anterior
- O módulo pode ser utilizado pela interface gráfica da API ou por um terminal interativo

---

## 🌐 Integração com a API

A Paciência é um jogo individual, mas sua interface web utiliza a API do projeto para criar a sessão, receber comandos e devolver o estado atualizado.

### Arquivos principais da integração

- `PacienciaRoutes.hpp` / `PacienciaRoutes.cpp`: registram as rotas HTTP relacionadas ao jogo.
- `PacienciaWebSocket.hpp` / `PacienciaWebSocket.cpp`: recebem ações do frontend e enviam o estado da partida.
- Classes de sessão ou sala: mantêm a instância de `Paciencia` associada ao jogador.

### Fluxo da comunicação

```text
Frontend → JSON/WebSocket → PacienciaWebSocket → Paciencia
Paciencia → Estado atualizado em JSON → Frontend
```

A API traduz comandos da interface para operações do núcleo, como comprar do estoque, mover cartas e desfazer uma jogada. Depois da execução, o estado das colunas, fundações, estoque e descarte é serializado e enviado ao navegador.

### Tratamento de erros

As validações específicas continuam no núcleo. A camada da API captura exceções e transforma o erro em uma mensagem JSON para o frontend, sem misturar regras do jogo com elementos visuais.

### Requisitos funcionais adicionais

- Criar uma sessão individual de Paciência.
- Receber movimentos em formato JSON.
- Enviar colunas, fundações, estoque e descarte ao frontend.
- Informar movimentos inválidos.
- Permitir reinício e desfazer pela interface web.

### Requisitos não funcionais adicionais

- Utilizar JSON na comunicação.
- Utilizar WebSocket para atualização da partida.
- Manter o núcleo independente de HTML, CSS e JavaScript.
- Separar rotas, comunicação e lógica do jogo.

## 🧩 Cartões CRC da API

### Classe PacienciaWebSocket

**Responsabilidades:**

- Receber comandos do frontend.
- Executar operações na instância de `Paciencia`.
- Serializar e enviar o estado do jogo.
- Capturar e enviar mensagens de erro.

**Colabora com:**

- `Paciencia`
- Classes de sessão da API

### Módulo PacienciaRoutes

**Responsabilidades:**

- Registrar as rotas HTTP do jogo.
- Criar ou localizar a sessão de Paciência.
- Entregar a página e os dados necessários à inicialização.

**Colabora com:**

- `PacienciaWebSocket`
- `Paciencia`

