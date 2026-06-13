# Pife

## Arquivos principais

- jogadorPife.h / jogadorPife.cpp: mão do jogador.
- pife.h / pife.cpp: lógica principal do jogo.
- regrasPife.h / regrasPife.cpp: regras e combinações do Pife.

## Requisitos Funcionais

- Criar uma mão de jogador.
- Adicionar cartas à mão.
- Remover cartas da mão.
- Comprar cartas do baralho.
- Descartar cartas.
- Consultar a quantidade de cartas na mão.
- Verificar combinações básicas, como trincas e sequências.
- Permitir integração com a Central de Jogos.

## Requisitos Não Funcionais

- Reutilizar as classes Carta, Valor, Naipe e Baralho do Core.
- Código modular em arquivos `.h` e `.cpp`.
- Lógica independente de interface gráfica.
- Código de fácil manutenção e extensão.

## User Stories

- Como jogador, quero receber cartas para formar minha mão.
- Como jogador, quero visualizar minha mão.
- Como jogador, quero comprar cartas do baralho.
- Como jogador, quero descartar cartas.
- Como jogador, quero saber quantas cartas tenho.
- Como sistema, quero verificar combinações válidas.
- Como desenvolvedor, quero separar a lógica em classes diferentes.

## Cartões CRC

### Classe JogadorPife

Responsabilidades:
- Armazenar a mão.
- Receber cartas.
- Descartar cartas.
- Informar a quantidade de cartas.

Colabora com:
- Carta

### Classe Pife

Responsabilidades:
- Controlar a lógica do jogo.
- Gerenciar o baralho.
- Controlar compra e descarte.
- Distribuir as cartas

Colabora com:
- JogadorPife
- Baralho
- Carta

### Módulo RegrasPife

Responsabilidades:
- Verificar combinações.
- Identificar trincas e sequências.

Colabora com:
- Carta
- Valor
- Naipe

## Integração com a API

O Pife possui integração multiplayer com o frontend por meio de rotas HTTP e WebSocket. A lógica do jogo permanece no núcleo, enquanto a API identifica os jogadores, encaminha ações e envia o estado atualizado.

### Arquivos principais da API

- `SalaPife.hpp` / `SalaPife.cpp`: mantém a partida, os jogadores registrados e as conexões.
- `PifeWebSocket.hpp` / `PifeWebSocket.cpp`: comunica o frontend com o backend por mensagens JSON.
- `GerenciadorSalasPife.hpp` / `GerenciadorSalasPife.cpp`: cria, armazena e busca salas.
- `PifeRoutes.hpp` / `PifeRoutes.cpp`: registra as rotas HTTP para criação e acesso às salas.
- `ExcecoesPife.hpp`: define erros específicos das regras do jogo.

### Fluxo da comunicação

```text
Frontend → PifeWebSocket → SalaPife → Pife
Pife → Estado em JSON → Frontend
```

O frontend pode enviar ações como comprar do baralho, comprar da mesa, descartar, organizar a mão e bater. O `PifeWebSocket` identifica a sessão, chama o método correspondente no núcleo e atualiza os jogadores conectados.

As exceções são lançadas pelo núcleo e capturadas na camada WebSocket, que as transforma em mensagens de erro para a interface.

## Requisitos Funcionais da API

- Criar salas multiplayer com quantidade configurável de jogadores.
- Permitir entrada por código, nome e token.
- Permitir reconexão à partida.
- Processar compras, descartes, organização e tentativa de vitória.
- Atualizar todos os participantes depois de uma ação válida.
- Enviar apenas a própria mão para cada jogador.
- Informar erros de regra ao frontend.

## Requisitos Não Funcionais da API

- Utilizar JSON e WebSocket.
- Manter as validações no backend.
- Separar núcleo, sala, rotas e comunicação.
- Não expor as cartas dos adversários.
- Permitir conexão de jogadores em redes diferentes.

## User Stories da API

- Como jogador, quero entrar em uma sala por um código.
- Como jogador, quero me reconectar sem perder minha posição.
- Como jogador, quero receber o estado atualizado após cada jogada.
- Como jogador, quero ser informado quando uma ação for inválida.
- Como sistema, quero identificar o jogador pela conexão.
- Como sistema, quero ocultar as mãos dos adversários.

## Cartões CRC da API

### Classe SalaPife

Responsabilidades:
- Manter a instância do jogo.
- Registrar jogadores e conexões.
- Controlar início e reconexão da partida.

Colabora com:
- Pife
- PifeWebSocket
- GerenciadorSalasPife

### Classe PifeWebSocket

Responsabilidades:
- Receber mensagens JSON.
- Identificar sala e jogador.
- Executar ações no núcleo.
- Capturar exceções e enviar estados ou erros.

Colabora com:
- SalaPife
- Pife
- GerenciadorSalasPife

### Classe GerenciadorSalasPife

Responsabilidades:
- Criar, armazenar e localizar salas.

Colabora com:
- SalaPife
- PifeRoutes
- PifeWebSocket

