🃏 FDP (Fodinha / Oh Hell)

📌 Descrição
O módulo FDP é responsável por implementar a lógica do jogo de vazas e sobrevivência "FDP" (também conhecido como Fodinha), utilizando a base arquitetural estabelecida no Core do projeto (classes mãe Mesa e Jogador, e as utilitárias BaralhoSujo e Carta).
O foco deste módulo é atuar como a autoridade (servidor) da partida, gerenciando a máquina de estados das rodadas, validando o sistema de apostas restritas, resolvendo os combates de cartas com trunfo e aplicando os danos de sobrevivência aos jogadores.

🎯 Requisitos (E5)

🔹 Requisitos Funcionais (User Stories)

    Como sistema, quero iniciar o jogo com um número fixo de vidas para todos os participantes ativos.

    Como mesa, quero distribuir uma quantidade variável de cartas por rodada (crescente e depois decrescente) baseada no número da rodada atual.

    Como sistema, quero revelar a carta do topo do baralho após a distribuição para definir o naipe do Trunfo.

    Como jogador, quero poder registrar uma aposta informando quantas vazas garanto que vou vencer nesta rodada.

    Como mesa (A Regra FDP), quero bloquear e invalidar a aposta do último jogador da rodada caso a soma de todos os palpites seja igual ao total de cartas em jogo.

    Como mesa, quero resolver cada vaza verificando quem jogou a carta mais alta do naipe puxado, respeitando a supremacia das cartas do naipe do Trunfo.

    Como sistema (Regra da Casa), quero aplicar o dano fixo de exatamente 1 vida aos jogadores que não cumprirem suas apostas (aposta_atual != vazas_ganhas).

    Como sistema, quero registrar todo o histórico de danos sofridos em um placar geral da partida.

    Como mesa, quero eliminar e remover da partida os jogadores que zerarem suas vidas, declarando vitória ao último sobrevivente.

🔹 Requisitos Não Funcionais

    O módulo deve herdar das classes base do Core (MesaFDP herda de Mesa, JogadorFDP herda de Jogador).

    O código deve ser modular, separado em arquivos .hpp e .cpp, permitindo fácil compilação via Makefile.

    A validação das regras de negócio (como a aposta proibida) deve ocorrer estritamente do lado da Mesa, adotando uma arquitetura à prova de trapaças (Server-Side Validation).

    O gerenciamento do histórico de danos deve fazer uso otimizado de containers da STL, especificamente std::map com buscas diretas por chave.

    A lógica do jogo deve ser completamente desacoplada da interface, pronta para ser integrada a um servidor multiplayer de rede.

🧠 Modelagem (Cartões CRC)

🧩 Classe MesaFDP
🔹 Responsabilidades

    Gerenciar a máquina de estados (fases) de cada rodada.

    Comandar o BaralhoSujo para embaralhar e distribuir as cartas.

    Calcular e validar a aposta proibida do último jogador.

    Avaliar as cartas jogadas na mesa para determinar o vencedor de cada vaza.

    Aplicar a mecânica de dano e atualizar as vidas dos jogadores no final da rodada.

    Alimentar o objeto Placar com os resultados.

🔗 Colabora com:

    JogadorFDP

    BaralhoSujo

    Carta

    Placar

🧩 Classe JogadorFDP
🔹 Responsabilidades

    Armazenar e fornecer seus atributos exclusivos de estado: vidas atuais, aposta da rodada e vazas ganhas na rodada.

    Zerar seus contadores temporários através de uma preparação para nova rodada.

    Receber o dano calculado pela mesa.

🔗 Colabora com:

    Carta (manipulação dos ponteiros na mão)

🧾 Estrutura Placar
🔹 Responsabilidades

    Armazenar o histórico cumulativo de desempenho de cada jogador na partida.

    Calcular o total de danos sofridos usando referências otimizadas de memória.

🔠 Enumeração FaseRodada
Define a máquina de estados que rege o ciclo do jogo:

    Distribuicao

    Apostas

    Combate

    Apuracao

⚙️ Estratégia de Implementação
A lógica arquitetural do sistema é baseada em:

🧱 Estruturas de dados:

    std::vector<JogadorFDP*> → Lista de jogadores sentados à mesa.

    std::vector<Carta*> → Pilha de cartas jogadas no centro da mesa (para resolver a vaza).

    std::map<std::string, std::vector<int>> → Estrutura de chaves-valores do Placar.

🎮 Fluxo do jogo (Loop Principal):

    Atualização do número de cartas da rodada atual.

    Identificação de quem é o dealer (distribuidor) e o primeiro a falar.

    Recebimento de apostas com validação em laço de repetição (while) comandado pela Mesa.

    Jogo de cartas (vazas) respeitando turnos.

🧠 Regras Críticas Tratadas:

    A Mesa realiza a verificação de "naipe puxado", forçando a lógica de que "Trunfo corta naipe, e outros naipes são descartes diretos".

    Aplicação implacável da verificação de integridade antes da alocação de pontuação (A Mesa processa e define os setters dos jogadores, os jogadores não se auto-pontuam).

🧩 Observações

    O módulo foi projetado para escalabilidade de Polimorfismo, permitindo instanciar as partidas genéricas através de ponteiros para a classe abstrata Mesa.

    O uso de alocação dinâmica e ponteiros crus garante alta performance, com limpezas controladas via destrutores virtuais implementados.