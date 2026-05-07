# Arquivos principais

- jogadorPife.h / jogadorPife.cpp: classe responsável pela mão do jogador.
- pife.h / pife.cpp: classe principal do jogo.
- distribuirPife.h / distribuirPife.cpp: funções relacionadas à distribuição de cartas.
- regrasPife.h / regrasPife.cpp: funções relacionadas às regras e combinações do Pife.

# Requisitos Funcionais

- O sistema deve permitir: 
    -criar uma mão de jogador.
    -adicionar cartas à mão.
    -remover cartas da mão.
    -comprar cartas do baralho.
    -descartar cartas.
    -consultar a quantidade de cartas na mão.
    -verificar combinações básicas do Pife, como trincas e sequências.
    -permitir futuramente integrar o módulo Pife ao menu principal da Central de Jogos.

# Requisitos Não Funcionais
- O módulo deve reutilizar as classes Carta, Valor, Naipe e Baralho do Core
- O código deve ser modular
- O código deve ser de fácil manutenção e extensão
- O módulo deve seguir boas práticas de orientação a objeto

# User Stories
- Como jogador, quero receber cartas para formar minha mão.
- Como jogador, quero visualizar minha mão
- Como jogador, quero comprar uma carta do baralho.
- Como jogador, quero descartar uma carta
- Como jogador, quero saber quantas cartas tenho
- Como sistema, quero ver se o par é valido
- Como desenvolvedor, quero separar a lógica do jogo em classes diferentes para facilitar manutenção.

# Cartões CRC
 Classe JogadorPife

Responsabilidades:
- Armazenar a mão do jogador.
- Receber cartas.
- Descartar cartas.
- Informar a quantidade de cartas na mão.
- Permitir acesso às cartas da mão.

Classe Pife

Responsabilidades:
- Controlar a lógica geral do jogo.
- Gerenciar o baralho.
- Gerenciar os jogadores.
- Controlar compra e descarte.
- Coordenar a execução de uma rodada.

Colabora com:
- JogadorPife
- Baralho
- Carta

Classe DistribuirPife

Responsabilidades:
- Distribuir cartas iniciais aos jogadores.
- Retirar cartas do baralho.
- Entregar cartas aos jogadores.

Colabora com:
- Baralho
- JogadorPife
- Carta

Classe RegrasPife

Responsabilidades:
- Verificar combinações válidas.
- Identificar trincas.
- Identificar sequências.
- Auxiliar na validação das jogadas.

Colabora com:
- Carta
- Valor
- Naipe