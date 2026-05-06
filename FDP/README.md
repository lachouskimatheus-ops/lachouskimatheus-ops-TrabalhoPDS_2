# 🃏 FDP (Fodinha / Oh Hell)

### 📌 Descrição
O módulo FDP é responsável por implementar a lógica do jogo de vazas e sobrevivência "FDP" (também conhecido como Fodinha), utilizando as classes base `Mesa`, `Jogador`, `BaralhoSujo` e `Carta` definidas no Core do projeto.

O foco deste módulo é atuar como a autoridade (servidor) da partida, gerenciando a máquina de estados das rodadas, validando o sistema de apostas restritas, resolvendo os combates de cartas com trunfo e aplicando os danos de sobrevivência aos jogadores.

---

### 🎯 Requisitos (E5)

#### 🔹 Requisitos Funcionais
* O sistema deve iniciar o jogo com um número fixo de vidas para todos os participantes ativos.
* O sistema deve distribuir uma quantidade variável de cartas por rodada (crescente e depois decrescente) baseada no número da rodada atual.
* O sistema deve revelar a carta do topo do baralho após a distribuição para definir o naipe do Trunfo.
* O sistema deve permitir que os jogadores registrem uma aposta informando quantas vazas garantem vencer na rodada.
* O sistema deve validar as apostas de acordo com a Regra FDP:
  * bloquear e invalidar a aposta do último jogador caso a soma de todos os palpites seja igual ao total de cartas em jogo.
* O sistema deve resolver cada vaza avaliando a carta mais alta do naipe puxado, respeitando a supremacia do naipe do Trunfo.
* O sistema deve aplicar a regra de dano (Regra da Casa) de exatamente 1 vida aos jogadores que não cumprirem suas apostas.
* O sistema deve registrar o histórico de danos sofridos em um placar geral da partida.
* O sistema deve eliminar jogadores que zerarem suas vidas e declarar vitória ao último sobrevivente.

#### 🔹 Requisitos Não Funcionais
* O módulo deve reutilizar as classes `Mesa`, `Jogador`, `BaralhoSujo` e `Carta` do Core.
* O código deve ser modular, separado em arquivos `.hpp` e `.cpp`.
* A validação das regras de negócio deve ocorrer de forma centralizada (Server-Side), impedindo manipulações por parte do jogador.
* O gerenciamento do histórico de danos deve utilizar containers da STL (como `std::map`).
* A lógica do jogo deve ser independente de interface gráfica, pronta para futura integração em rede.

---

### 🧠 Modelagem

#### 🧩 Classe MesaFDP
🔹 **Responsabilidades**
* Gerenciar a máquina de estados (fases) de cada rodada.
* Comandar o `BaralhoSujo` para embaralhar e distribuir as cartas.
* Calcular e validar a aposta proibida do último jogador.
* Avaliar as cartas jogadas na mesa para determinar o vencedor de cada vaza.
* Aplicar a mecânica de dano e atualizar as vidas dos jogadores no final da rodada.
* Alimentar o objeto `Placar` com os resultados.

🔗 **Colabora com:**
* `JogadorFDP`
* `BaralhoSujo`
* `Carta`
* `Placar`

#### 🧩 Classe JogadorFDP
🔹 **Responsabilidades**
* Armazenar e fornecer seus atributos exclusivos de estado: vidas atuais, aposta da rodada e vazas ganhas na rodada.
* Zerar seus contadores temporários através de uma preparação para nova rodada.
* Receber o dano calculado pela mesa.

🔗 **Colabora com:**
* `Carta` (manipulação dos ponteiros na mão)

#### 🧾 Estrutura Placar
🔹 **Responsabilidades**
* Armazenar o histórico cumulativo de desempenho de cada jogador na partida.
* Calcular o total de danos sofridos usando referências otimizadas de memória.

#### 🔠 Enumeração FaseRodada
Define a máquina de estados que rege o ciclo do jogo:
* `Distribuicao`
* `Apostas`
* `Combate`
* `Apuracao`

---

### ⚙️ Estratégia de Implementação
A lógica arquitetural do sistema é baseada em:

🧱 **Estruturas de dados:**
* `vector<JogadorFDP*>` → Lista de jogadores sentados à mesa
* `vector<Carta*>` → Pilha de cartas jogadas no centro da mesa (para resolver a vaza)
* `map<string, vector<int>>` → Estrutura de chaves-valores do Placar

🎮 **Fluxo do jogo:**
* Atualização do número de cartas da rodada atual
* Identificação de quem é o dealer e o primeiro a falar
* Recebimento de apostas com validação em laço de repetição (`while`) comandado pela Mesa
* Jogo de cartas (vazas) respeitando turnos

🧠 **Regras:**
* A Mesa realiza a verificação de "naipe puxado"
* Trunfo corta naipe, e outros naipes são descartes diretos
* A Mesa processa e define os setters dos jogadores (os jogadores não se auto-pontuam)

### 🧩 Observações
* O módulo foi projetado para escalabilidade de Polimorfismo, permitindo instanciar as partidas genéricas através de ponteiros para a classe abstrata `Mesa`.
* O uso de alocação dinâmica e ponteiros crus garante alta performance.
* O sistema possui limpezas de memória controladas via destrutores virtuais implementados.