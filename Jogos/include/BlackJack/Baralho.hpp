#pragma once
#ifndef BARALHO_HPP
#define BARALHO_HPP

#include <iostream>
#include <vector>
#include <random>
#include "Carta.hpp"

/**
 * @brief Representa um baralho de cartas reutilizável em múltiplos jogos.
 *
 * Classe base do sistema de cartas da coletânea. Gerencia um conjunto de
 * ponteiros para objetos `Carta`, oferecendo operações de embaralhamento,
 * compra e inserção. É herdada por baralhos especializados (ex: BaralhoSujo
 * do FDP) e utilizada diretamente pelo Paciência e pelo Pife.
 *
 * @note O vetor interno usa ponteiros (`Carta*`) para facilitar a herança
 *       polimórfica. A memória é gerenciada pelo destrutor virtual.
 */
class Baralho {
protected:
    std::vector<Carta*> cartas_; ///< Conjunto de cartas do baralho (gerenciado por ponteiros)

public:

    /**
     * @brief Construtor principal.
     *
     * Se `quantidadeBaralhos` for maior que zero, inicializa o baralho com
     * 52 × `quantidadeBaralhos` cartas padrão (sem coringas).
     * Se for zero, cria um baralho vazio (usado pelo Paciência para montagem manual).
     *
     * @param quantidadeBaralhos Número de baralhos padrão a incluir (padrão: 1).
     */
    Baralho(int quantidadeBaralhos = 1);

    /**
     * @brief Destrutor virtual. Libera a memória de todos os ponteiros em `cartas_`.
     */
    virtual ~Baralho();

    // =========================================================
    /// @name Operações Principais
    // =========================================================
    ///@{

    /**
     * @brief Inicializa (ou reinicializa) o baralho com o conjunto padrão de cartas.
     *
     * Limpa as cartas existentes e recria o baralho completo.
     * Virtual para permitir que subclasses definam baralhos especializados.
     */
    virtual void inicializar();

    /**
     * @brief Embaralha as cartas aleatoriamente usando `std::mt19937`.
     */
    void embaralhar();

    /**
     * @brief Retira e retorna um ponteiro para a carta do topo do baralho.
     *
     * @return Ponteiro para a carta retirada, ou nullptr se o baralho estiver vazio.
     * @warning O chamador passa a ser responsável pela memória da carta retornada.
     */
    Carta* puxarCarta();

    /**
     * @brief Retira e retorna a carta do topo do baralho por valor (cópia).
     *
     * Alternativa a `puxarCarta()` que retorna por valor em vez de ponteiro.
     * Libera a memória do ponteiro interno após a cópia.
     *
     * @return Cópia da carta retirada do topo.
     */
    Carta retirarCarta();

    /**
     * @brief Insere uma carta (por cópia) no fundo do baralho.
     * @param c Carta a ser inserida.
     */
    void inserirCarta(const Carta& c);

    /**
     * @brief Adiciona uma carta (por cópia) no topo do baralho.
     * @param carta Carta a ser adicionada.
     */
    void adicionarCarta(const Carta& carta);

    ///@}

    // =========================================================
    /// @name Consultas de Estado
    // =========================================================
    ///@{

    /** @return Número de cartas atualmente no baralho. */
    int getQtdCartas() const;

    /** @return Número de cartas atualmente no baralho (alias de getQtdCartas). */
    int tamanho() const;

    /** @return true se o baralho não contém cartas. */
    bool estaVazio() const;

    ///@}

    // =========================================================
    /// @name Utilitários
    // =========================================================
    ///@{

    /** @brief Remove todas as cartas do baralho e libera a memória. */
    void limpar();

    ///@}

    // =========================================================
    /// @name Construtores Especiais
    // =========================================================
    ///@{

    /**
     * @brief Construtor de cópia. Realiza cópia profunda de todos os ponteiros.
     * @param outro Baralho a ser copiado.
     */
    Baralho(const Baralho& outro);

    /**
     * @brief Operador de atribuição. Realiza cópia profunda de todos os ponteiros.
     * @param outro Baralho a ser copiado.
     * @return Referência para este objeto.
     */
    Baralho& operator=(const Baralho& outro);

    ///@}
};

#endif