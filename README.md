# 🃏 Cartaralho

Coletânea de jogos de cartas desenvolvida como projeto da disciplina de **Programação e Desenvolvimento de Software II** da UFMG. O sistema oferece seis jogos clássicos com interface web moderna, backend em C++ e comunicação em tempo real via WebSocket.

---

## 🎮 Jogos Disponíveis

| Jogo | Modo | Descrição |
|------|------|-----------|
| **Paciência** | Single-player | Klondike Solitaire com solver automático e sistema de recordes |
| **FDP (Fodinha de Paus)** | Multiplayer | Jogo de apostas em vazas com regras paulistas |
| **Truco** | Multiplayer | Truco mineiro e paulista |
| **Pife** | Multiplayer | Jogo de combinações de cartas com coringa |
| **Poker** | Single/Multiplayer | 5 Card Draw contra computador ou outros jogadores |
| **Blackjack** | Single/Multiplayer | 21 clássico contra a banca ou outros jogadores|

---

## 🏗️ Arquitetura

O projeto é dividido em três camadas principais:

```
Cartaralho/
│
├── Jogos/                        # Lógica dos jogos em C++ puro
│   ├── include/                  # Headers (.hpp) organizados por jogo
│   │   ├── Core/                 # Carta e Baralho (base compartilhada)
│   │   ├── Paciencia/
│   │   ├── FDP/
│   │   ├── Truco/
│   │   ├── Pife/
│   │   └── Poker/
│   ├── src/                      # Implementações (.cpp), mesma estrutura
│   └── obj/                      # Arquivos objeto (.o), mesma estrutura
│
├── API/                          # Servidor HTTP/WebSocket
│   ├── dependencias/             # Crow, ASIO e nlohmann/json (header-only)
│   ├── include/                  # Headers da API organizados por módulo
│   │   ├── coreAPI/              # Servidor, sessões e utilitários base
│   │   ├── multiplayer/          # Salas e lógica dos jogos multiplayer
│   │   └── routes/               # Rotas HTTP e WebSocket de cada jogo
│   ├── src/                      # Implementações (.cpp), mesma estrutura do include
│   └── obj/                      # Arquivos objeto (.o), mesma estrutura do include
│
├── frontend/                     # Interface web
│   ├── assets/                   # Imagens, sons, fontes e baralhos
│   ├── css/                      # Estilos de cada jogo
│   ├── js/                       # Lógica de cada jogo no frontend
│   └── pages/                    # Páginas HTML
│
└── docs/                         # Documentação gerada pelo Doxygen
```

**Tecnologias utilizadas:**
- **Backend:** C++17, [Crow](https://crowcpp.org/) (framework HTTP/WebSocket), ASIO, nlohmann/json
- **Frontend:** HTML5, CSS3, JavaScript (Vanilla)
- **Documentação:** Doxygen
- **Build:** Make (MSYS2/MinGW64)

---

## ⚙️ Requisitos

- MSYS2 com MinGW64
- G++ com suporte a C++17
- Make

---

## 🚀 Como Executar

**Compilar:**
```bash
make
```

**Iniciar o servidor:**
```bash
make run
```

Após iniciar, acesse no navegador:
```
http://localhost:8080
```

---

## 👥 Equipe

Projeto desenvolvido em grupo para a disciplina de PDS II — UFMG.
 
- Lucas de Almeida Pereira
- João Luís Fernandes Mendes Gomes
- Matheus Lachouski
- Igor Santos Sassano
- Cassio Alexandre Rodrigues Leite Junior
- Emanuel Freitas Guimarães Costa

---

## 📄 Documentação

A documentação completa das classes e métodos foi gerada com Doxygen e está disponível em `docs/html/index.html`.

Para regenerar:
```bash
doxygen Doxyfile
```