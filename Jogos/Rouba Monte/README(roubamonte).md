_Jogo Rouba-Monte_

📖 Regras do Jogo
O Rouba-Monte é um jogo de paciência e atenção, cujo objetivo final é terminar a partida com o maior monte de cartas.

_O Início_
O baralho (geralmente de 52 cartas) é embaralhado.

Oito cartas são viradas na mesa (o Descarte).

Cada jogador recebe uma mão inicial (opcional, dependendo da variação) ou as cartas permanecem no Monte de Compra.

_Dinâmica da Rodada_
Em seu turno, o jogador compra uma carta do monte e verifica as seguintes possibilidades:

Pescar da Mesa: Se a carta comprada for igual (mesmo valor/número) a alguma carta da mesa, o jogador as junta e inicia seu próprio Monte. A carta comprada deve ficar sempre no topo.

Roubar o Monte: Se a carta comprada for igual à carta do topo do monte de um adversário, o jogador "rouba" o monte inteiro para si, colocando a nova carta no topo.

Acumular no Próprio Monte: Se a carta comprada for igual à carta do topo do seu próprio monte, ele apenas a adiciona ali.

Descarte: Se a carta comprada não servir para nenhuma das ações acima, ela deve ser colocada aberta no centro da mesa (área de descarte).

Fim de jogo: A partida termina quando não houver mais cartas no monte de compra. Ganha quem tiver o monte com o maior número de cartas.

🛠 Funcionalidades Implementadas
Para que o jogo funcione corretamente no terminal, as seguintes funcionalidades técnicas foram desenvolvidas:

1. Gerenciamento do Baralho (Deck Management)
Geração: Criação de um array ou vetor contendo as 52 cartas.

Embaralhamento: Utilização de algoritmos (como std::shuffle) para garantir a aleatoriedade das partidas.

2. Estrutura de Dados dos Montes (Stack Logic)
Cada monte de jogador é tratado como uma Pilha (Stack).

Funcionalidade: Apenas o elemento do topo é visível e comparável para fins de "roubo".

3. Lógica de Comparação (Matching System)
O sistema verifica automaticamente se o valor da carta atual coincide com:

Alguma carta da mesa (Vetor de Descarte).

O topo de qualquer monte adversário.

O topo do próprio monte.

4. Fluxo de Turnos (Game Loop)
Controle de qual jogador está ativo no momento.

Atualização do estado da tela a cada jogada.

5. Interface via Terminal
Exibição organizada das cartas na mesa.

Exibição do tamanho atual do monte de cada jogador e qual carta está no topo.
