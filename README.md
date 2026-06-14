# 🃏 Cartaralho

Coletânea de jogos de cartas desenvolvida como projeto prático para a disciplina de **Programação e Desenvolvimento de Software II (PDS II)** da Universidade Federal de Minas Gerais (UFMG). 

O sistema consolida um ecossistema de seis jogos clássicos integrados sob uma interface web responsiva e moderna, alimentada por um motor robusto de backend em C++17 e comunicação bidirecional em tempo real via WebSockets.

---

## 📽️ Demonstração Prática
> **Atenção:** O vídeo abaixo possui menos de 5 minutos e demonstra o processo exato de compilação, inicialização e execução das principais funcionalidades do sistema.
- [Clique aqui para assistir ao vídeo de demonstração](https://drive.google.com/file/d/16A752Q8NEaFzzyjgjf7nNgYaBz6dyagq/view?usp=drive_link)

---

## 🎮 Funcionalidades Entregues e Escopo

### 💻 Itens Entregues (No Escopo)
* **Paciência (Single-player):** Klondike Solitaire completo com renderização gráfica dinâmica, histórico de movimentos, sistema de recordes locais e *solver* com resolução automatizada.
* **FDP - Fodinha de Paus (Multiplayer):** Sistema dinâmico de apostas e vazas baseado nas regras paulistas, controle de pontuação e eliminação por vidas.
* **Truco (Multiplayer):** Suporte total a partidas simultâneas com alternância de regras regionais (Modo Paulista com manilhas baseadas no vira e Modo Mineiro com manilhas fixas).
* **Pife (Multiplayer):** Gerenciador de rodadas focado em combinações de cartas (trincas/sequências), descarte interativo e suporte a cartas coringas configuradas dinamicamente.
* **Poker (Single/Multiplayer):** Modalidade de *5 Card Draw* com inteligência artificial para o modo single-player e gerenciamento de rodadas de apostas síncronas para múltiplos jogadores.
* **Blackjack (Single/Multiplayer):** Jogo de 21 clássico com inteligência computacional para controle das ações da Banca (Dealer) e avaliação individualizada das jogadas.
* **Mecanismos Core compartilhados:** Baralho dinâmico unificado, tratamento polimórfico de regras e estados, e persistência de sessões para reconexão em partidas multiplayer.

### 🚫 Itens Fora do Escopo
* Persistência de dados em Bancos de Dados Relacionais (SQL) ou Não-Relacionais (NoSQL) fora do ecossistema local ou cache de memória (`localStorage`).
* Sistemas de chat por voz ou servidores de matchmaking globais dedicados na nuvem.

---

## 🏗️ Arquitetura e Organização do Projeto

A organização de pastas segue uma divisão estrita de responsabilidades entre as regras de negócio puras (Motores), a camada de transporte de dados (API) e a interface do usuário (Frontend).

```text
Cartaralho/
│
├── Jogos/                        # Motores e Regras de Negócio em C++ puro
│   ├── include/                  # Headers (.hpp) organizados por jogo
│   │   ├── Core/                 # Classe Carta e Baralho (base compartilhada)
│   │   ├── Paciencia/
│   │   ├── FDP/
│   │   ├── Truco/
│   │   ├── Pife/
│   │   └── Poker/
│   ├── src/                      # Implementações (.cpp), mesma estrutura
│   └── obj/                      # Arquivos objeto (.o) gerados na compilação
│
├── API/                          # Camada do Servidor HTTP/WebSocket (Micro-serviço)
│   ├── dependencias/             # Framework Crow, ASIO e nlohmann/json (Header-only)
│   ├── include/                  # Headers da API distribuídos por módulos
│   │   ├── coreAPI/              # Gerenciador de conexões, sessões e utilitários
│   │   ├── multiplayer/          # Controle de salas e sincronização de turnos
│   │   └── routes/               # Mapeamento de endpoints HTTP e conexões WS
│   ├── src/                      # Implementações (.cpp), mesma estrutura de módulos
│   └── obj/                      # Arquivos objeto (.o) gerados na compilação
│
├── frontend/                     # Interface Homem-Máquina (IHM)
│   ├── assets/                   # Mídias estáticas: sprites de cartas, áudios e fontes
│   ├── css/                      # Arquivos de estilização modular para cada jogo
│   ├── js/                       # Controladores JavaScript de eventos e WebSockets
│   └── pages/                    # Estruturas HTML estáticas e modais de salas
│
└── docs/                         # Documentação técnica gerada automaticamente pelo Doxygen
```

---

## ⚙️ Compilação e Execução

### Requisitos Prévios
* Ambiente configurado via **MSYS2** com a Toolchain **MinGW64**.
* Compilador G++ com suporte nativo à especificação **C++17**.
* Utilitário **GNU Make**.

### Comandos Exatos de Terminal

1. **Limpeza de resíduos de compilação anterior (Opcional):**
```bash
make clean
```

2. **Compilação do projeto:**
```bash
make
```

3. **Inicialização do servidor HTTP/WebSocket:**
```bash
make run
```

4. **Acesso:**
Abra o seu navegador de preferência e acesse o endereço: `http://localhost:8080`

---

## 🕹️ Instruções de Uso e Exemplos práticos

O fluxo operacional do sistema concentra-se inteiramente no frontend, fornecendo uma experiência visual intuitiva guiada por formulários HTML e manipulação direta via cliques e arrastes nas cartas.

### Fluxo de Uso Padrão (Exemplo: Blackjack / 21)

1. **Menu Inicial:** O usuário acessa a página central e escolhe o jogo Blackjack. Ele é redirecionado para a tela de configurações (`blackjack_config.html`).
2. **Entrada de Dados (Lobby):** 
   * *Ação do Usuário:* Clicar em um dos botões numéricos (`1`, `2`, `3` ou `4`) para definir a quantidade máxima de jogadores suportados na sala.
   * *Ação do Usuário:* Clicar no botão `CRIAR SALA`.
3. **Mesa de Espera:** 
   * *Saída do Sistema:* O backend gera um identificador único alfanumérico para a sala (ex: `R6B9L2`), exibe na tela o link completo de convite e ativa um modal bloqueante aguardando que os oponentes acessem o mesmo link.
4. **Partida em Andamento:** 
   * Assim que a capacidade máxima é preenchida, o WebSocket transmite o payload JSON contendo o estado inicial do jogo, renderizando as cartas recebidas.
   * *Entrada do Usuário:* Clicar nos botões operacionais da mesa, como `PEDIR CARTA` ou `PARAR`.
   * *Saída do Sistema:* O servidor processa a pontuação em tempo real, atualiza o placar e exibe modais com o veredito final da rodada contra a Banca (Dealer).

---

## 🛡️ Programação Defensiva e Tratamento de Exceções

O projeto foi arquitetado sob os princípios de tolerância a falhas tanto no tráfego de rede quanto na manipulação de memória em C++. Os principais tratamentos estão divididos em:

* **Gerenciamento de Erros de WebSocket (`API/src/routes/`):** Captura de exceções de parsing via blocos `try-catch` ao processar strings JSON recebidas pelo frontend. Se um payload malformado for enviado para travar o servidor, a API o descarta, protege a execução contínua e envia uma resposta estruturada de erro (`tipo: "erro"`) ao remetente.
* **Validação de Limites de Estruturas de Dados (`Jogos/src/`):** Métodos como `Baralho::puxarCarta()` e `Jogador::jogarCarta(int posicao)` validam defensivamente os tamanhos dos vetores. Tentar puxar uma carta de um deck vazio dispara um `std::runtime_error`, tratado na camada superior para evitar travamentos abruptos do software.
* **Prevenção de Referências Nulas (*Null-Pointer Dereferencing*):** Funções e lógicas de rodada criam objetos locais temporários na memória (*stack*) para atuar como coringas de validação quando referências a ponteiros de cartas (como o Vira do Truco) forem avaliados como `nullptr`, blindando a execução contra falhas de segmentação (*Segmentation Faults*).
* **Proteção Contra Vazamentos de Memória (`API/src/multiplayer/` e `Jogos/src/Core/`):** Desalocação rigorosa via destrutores de ponteiros gerenciados de forma dinâmica (`delete`) ao redefinir mesas ou limpar instâncias de rodadas anteriores, mantendo o consumo de RAM do servidor linear e estável.

---

## 📄 Documentação (Doxygen)

Toda a infraestrutura do ecossistema foi comentada seguindo o padrão Doxygen. Os arquivos de visualização técnica ficam disponíveis na pasta local em `docs/html/index.html`.

Para gerar ou atualizar a documentação técnico-estrutural através do código-fonte, utilize o terminal na raiz do projeto:
```bash
doxygen Doxyfile
```

---

## ⏱️ Ciclo de Desenvolvimento e Gestão de Sprints

O ciclo produtivo do sistema foi segmentado em Sprints estruturadas em modelo ágil. Vale destacar que o grupo obteve **100% de aproveitamento do escopo planejado**, resultando em nenhum item negligenciado ou postergado.

* **Sprint 1: Arquitetura Core e Motores (Início do projeto até 01 de Maio)**
  * *Objetivo:* Construção da base polimórfica estrutural de Cartas e Baralhos, e implementação inicial dos motores lógicos locais em C++.
  * *Planejado vs. Realizado:* Desenvolvimento integral de todas as classes básicas de dados e os algoritmos internos dos jogos sem atrasos.
* **Sprint 2: Infraestrutura de Rede e Interface (01 de Maio a 20 de Maio)**
  * *Objetivo:* Configuração do micro-serviço HTTP/WebSocket utilizando o framework Crow e estruturação das páginas estáticas em HTML/CSS.
  * *Planejado vs. Realizado:* Integração dos canais assíncronos e tratamento de parsing JSON concluídos com estabilidade de tráfego.
* **Sprint 3: Interface Gráfica Avançada e Integração Geral (20 de Maio a 11 de Junho)**
  * *Objetivo:* Polimento estético, inserção de sprites texturizados avançados de cartas, efeitos sonoros dinâmicos e testes exaustivos de concorrência.
  * *Planejado vs. Realizado:* Implementação de mecanismos automáticos de reconexão de pacotes e refatoração completa do CSS de todos os jogos para um padrão premium minimalista unificado.

---

## 👥 Equipe e Detalhamento de Contribuições

> **Nota Metodológica:** O volume de linhas detalhado abaixo reflete **exclusivamente o esforço técnico de codificação** do repositório. Dependências diretas de terceiros, arquivos de mídia, artefatos de compilação e documentações geradas automaticamente (Doxygen) foram rigorosamente isolados da contagem para apresentar uma métrica fidedigna do código-fonte.
>
> **⚠️ Observação Importante:** Os dados brutos apresentados nativamente na aba "Insights/Contributors" do GitHub **não refletem as mudanças reais** da equipe. As métricas do GitHub foram drasticamente infladas pela inclusão inicial de bibliotecas externas com dezenas de milhares de linhas (como os cabeçalhos do Crow e JSON), bem como por mudanças estruturais e reestruturações de pastas no repositório realizadas ao longo das Sprints. Os valores abaixo representam a extração limpa, filtrada e auditada do código-fonte proprietário digitado.

* **Matheus Lachouski (85 commits | +25.684 linhas / -17.644 linhas)**
  * Estruturação da arquitetura base do servidor HTTP e rotas WebSockets na pasta `API/`.
  * Desenvolvimento lógico e de interface ponta a ponta da modalidade **Pife**.
  * Construção e automação do processo de compilação com `Makefile`.
  * *(**Observação:** O volume de linhas gerado é significativamente maior em relação aos demais membros pois ele foi o responsável ativo pela reestruturação das pastas e organização do repositório no GitHub, o que resultou na movimentação e no redirecionamento sistemático de arquivos ao longo do ciclo de desenvolvimento).*

* **Igor Santos Sassano (85 commits | +6.575 linhas / -1.564 linhas)**
  * Desenvolvimento do motor de turnos e mecânica do jogo **Poker**.
  * Construção integral do ecossistema do Poker, interligando a inteligência artificial (Computador) e o multiplayer de rede.

* **Cassio Alexandre Rodrigues Leite Junior (76 commits | +4.984 linhas / -649 linhas)**
  * Implementação da lógica estrutural e mesa gráfica do jogo **Blackjack / 21**.
  * Codificação das estatísticas da Banca (Dealer) e o sistema de pontuação dinâmica do Ás.

* **Lucas de Almeida Pereira (73 commits | +9.047 linhas / -2.390 linhas)**
  * Criação das classes base compartilhadas dos motores de jogo (`Carta` e `Baralho`).
  * Implementação da lógica, *solver* autônomo e interface de renderização gráfica do **Paciência**.
  * Estruturação do Menu Principal de seleção e documentação técnica unificada via Doxygen.

* **João Luís Fernandes Mendes Gomes (41 commits | +8.383 linhas / -6.524 linhas)**
  * Construção das rotas, validações de vazas e controle de pontuação do multiplayer de **FDP**.
  * Incorporação de assets, sprites gráficos texturizados e trilha sonora da aplicação.

* **Emanuel Freitas Guimarães Costa (22 commits | +3.011 linhas / -561 linhas)**
  * Implementação das regras regionais, cálculo de manilhas fixas e fluxos lógicos do **Truco Mineiro**.
  * Integração das variáveis do motor central do Truco com a interface web de jogo.
 
📚 Cartões CRC e User Stories

A documentação da engenharia de requisitos e a modelagem orientada a objetos desenvolvida para o sistema, contemplando os **Cartões CRC** e as **User Stories**, encontra-se fragmentada e organizada nos diretórios de documentação individuais de cada jogo.

Acesse os documentos de cada motor através dos links diretos abaixo:

* [🃏 Paciência](./Jogos/docs/README_Paciencia.md)
* [🃏 FDP](./Jogos/docs/README_FDP.md)
* [🃏 Truco](./Jogos/docs/README_Truco.md)
* [🃏 Pife](./Jogos/docs/README_Pife.md)
* [🃏 Poker](./Jogos/docs/README_Poker.md)
* [🃏 Blackjack](./Jogos/docs/README_Blackjack.md)
