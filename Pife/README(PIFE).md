# Pife
  
---

## Arquivos principais

- jogadorPife.h / jogadorPife.cpp: mão do jogador.
- pife.h / pife.cpp: lógica principal do jogo.
- distribuirPife.h / distribuirPife.cpp: distribuição de cartas.
- regrasPife.h / regrasPife.cpp: regras e combinações do Pife.

---

## Requisitos Funcionais

- Criar uma mão de jogador.
- Adicionar cartas à mão.
- Remover cartas da mão.
- Comprar cartas do baralho.
- Descartar cartas.
- Consultar a quantidade de cartas na mão.
- Verificar combinações básicas, como trincas e sequências.
- Permitir futura integração com a Central de Jogos.

---

## Requisitos Não Funcionais

- Reutilizar as classes Carta, Valor, Naipe e Baralho do Core.
- Código modular em arquivos `.h` e `.cpp`.
- Lógica independente de interface gráfica.
- Código de fácil manutenção e extensão.

---

## User Stories

- Como jogador, quero receber cartas para formar minha mão.
- Como jogador, quero visualizar minha mão.
- Como jogador, quero comprar cartas do baralho.
- Como jogador, quero descartar cartas.
- Como jogador, quero saber quantas cartas tenho.
- Como sistema, quero verificar combinações válidas.
- Como desenvolvedor, quero separar a lógica em classes diferentes.

---

## Cartões CRC

### Classe JogadorPife

Responsabilidades:
- Armazenar a mão.
- Receber cartas.
- Descartar cartas.
- Informar a quantidade de cartas.

Colabora com:
- Carta

---

### Classe Pife

Responsabilidades:
- Controlar a lógica do jogo.
- Gerenciar o baralho.
- Controlar compra e descarte.

Colabora com:
- JogadorPife
- Baralho
- Carta

---

### Classe DistribuirPife

Responsabilidades:
- Distribuir cartas.
- Retirar cartas do baralho.

Colabora com:
- Baralho
- JogadorPife

---

### Classe RegrasPife

Responsabilidades:
- Verificar combinações.
- Identificar trincas e sequências.

Colabora com:
- Carta
- Valor
- Naipe