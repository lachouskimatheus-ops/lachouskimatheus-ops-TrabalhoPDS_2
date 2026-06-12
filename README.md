# 🃏 Cartaralho - PDS II

Este projeto consiste em uma plataforma de 6 jogos multiplayer e singleplayer de cartas baseada em uma arquitetura cliente-servidor robusta. O sistema integra um backend focado em performance escrito em C++ com uma interface web reativa e dinâmica.

---

##  Funcionalidades e Escopo do Sistema

A plataforma conta com um ecossistema completo para gerenciamento e execução de jogos de cartas em tempo real:
* **Servidor Online:** Infraestrutura estável utilizando o microframework **Crow** para gerenciamento de rotas HTTP e WebSockets.
* **Motores dos Jogos:** Implementação completa das regras de negócio, manipulação de baralhos e fluxos de turnos assíncronos para jogos como **Blackjack**, **Paciência** e **Pife**.
* **Comunicação Bidirecional:** Troca de mensagens em tempo real e de baixa latência entre o cliente e o servidor através de payloads JSON.
* **Resiliência:** Tratamento estruturado de erros e validações por meio de exceções customizadas universais (`ExcecoesPife`, etc.).
* **Interface Responsiva:** Telas de menus, lobbies e tabuleiros construídas de forma semântica em HTML5 e totalmente estilizadas via CSS3.

---

## 🛠️ Tecnologias Utilizadas

* **Backend Core:** C++17
* **Framework de Rede:** Crow C++ (HTTP & WebSockets)
* **Serialização de Dados:** Biblioteca JSON para C++
* **Frontend:** HTML5, CSS3 e JavaScript (Vanilla)
* **Documentação Técnica:** Doxygen

---

## 📦 Como Executar o Projeto

### Pré-requisitos
* Compilador GCC instalado com suporte a C++17.
* Biblioteca do framework Crow e dependências configuradas.

### Passos para Compilação e Execução
1. Clone o repositório para a sua máquina local:
   ```bash
   git clone [https://github.com/lachousimatheus-ops/lachousimatheus-ops-TrabalhoPDS_2.git](https://github.com/lachousimatheus-ops/lachousimatheus-ops-TrabalhoPDS_2.git)
