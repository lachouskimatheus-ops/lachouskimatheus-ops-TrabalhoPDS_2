#ifndef CARTA_HPP
#define CARTA_HPP

#include <iostream>
#include <string>

/**
 * @brief Representa o naipe de uma carta do baralho.
 *
 * Utiliza aliases dentro do enum para garantir compatibilidade entre
 * os diferentes jogos da coletânea, que historicamente usavam convenções
 * de nomenclatura distintas (minúsculo vs. maiúsculo, singular vs. plural).
 *
 * @note Os valores inteiros correspondem aos índices usados pelo servidor
 *       WebSocket e pelo sistema de personalização do frontend
 *       (0=paus, 1=copas, 2=espadas, 3=ouros).
 */
enum class Naipe {
    paus    = 0, ///< Paus (♣) — alias minúsculo usado pelo Paciência e Pife
    Paus    = 0, ///< Paus (♣) — alias maiúsculo usado pelo FDP e Truco
    copas   = 1, ///< Copas (♥) — alias minúsculo
    Copa    = 1, ///< Copas (♥) — alias maiúsculo
    espadas = 2, ///< Espadas (♠) — alias minúsculo
    Espada  = 2, ///< Espadas (♠) — alias maiúsculo
    ouros   = 3, ///< Ouros (♦) — alias minúsculo
    Ouro    = 3, ///< Ouros (♦) — alias maiúsculo
    Nenhum  = 4  ///< Naipe indefinido ou inválido
};

/**
 * @brief Representa o valor nominal de uma carta do baralho.
 *
 * O valor inteiro subjacente corresponde ao número da carta (1–13),
 * permitindo comparações aritméticas diretas nas regras dos jogos.
 */
enum class Valor {
    Invalido = 0,  ///< Valor indefinido ou inválido
    As       = 1,  ///< Ás (1)
    Dois     = 2,  ///< Dois (2)
    Tres     = 3,  ///< Três (3)
    Quatro   = 4,  ///< Quatro (4)
    Cinco    = 5,  ///< Cinco (5)
    Seis     = 6,  ///< Seis (6)
    Sete     = 7,  ///< Sete (7)
    Oito     = 8,  ///< Oito (8)
    Nove     = 9,  ///< Nove (9)
    Dez      = 10, ///< Dez (10)
    Valete   = 11, ///< Valete / J (11)
    Dama     = 12, ///< Dama / Q (12)
    Rei      = 13  ///< Rei / K (13)
};

/**
 * @brief Representa uma carta de baralho com valor e naipe.
 *
 * Classe central da coletânea de jogos, compartilhada por todos os jogos
 * (Paciência, FDP, Pife, Truco, Poker). Unifica duas implementações
 * anteriores distintas, expondo métodos compatíveis com ambas as convenções
 * de nomenclatura originais.
 */
class Carta {
private:
    int   valor_; ///< Valor numérico da carta (1–13)
    Naipe naipe_; ///< Naipe da carta

public:

    // =========================================================
    /// @name Construtores
    // =========================================================
    ///@{

    /** @brief Construtor padrão. Cria uma carta inválida (valor 0, naipe Nenhum). */
    Carta();

    /**
     * @brief Construtor por valor inteiro e naipe.
     * @param valor Valor numérico da carta (1 = Ás, 13 = Rei).
     * @param naipe Naipe da carta.
     */
    Carta(int valor, Naipe naipe);

    /**
     * @brief Construtor por enum Valor e naipe.
     * @param valor Valor da carta como enum Valor.
     * @param naipe Naipe da carta.
     */
    Carta(Valor valor, Naipe naipe);

    ///@}

    // =========================================================
    /// @name Interface FDP / Paciência / Pife
    // =========================================================
    ///@{

    /**
     * @brief Retorna o valor numérico da carta.
     * @return Inteiro de 1 (Ás) a 13 (Rei).
     */
    int getValor() const;

    /**
     * @brief Retorna o naipe da carta.
     * @return Naipe da carta.
     */
    Naipe getNaipe() const;

    /**
     * @brief Retorna a força da carta para fins de comparação no FDP.
     * @return Valor de força da carta.
     */
    int forca() const;

    /**
     * @brief Retorna a força do naipe para desempate no FDP.
     * @return Valor numérico de força do naipe.
     */
    int getForcaNaipe() const;

    /**
     * @brief Converte a carta para string no formato "valor_naipe".
     * @return String representando a carta (ex: "As_Paus", "10_Copas").
     */
    std::string toString() const;

    ///@}

    // =========================================================
    /// @name Interface Truco
    // =========================================================
    ///@{

    /**
     * @brief Retorna o valor da carta como enum Valor.
     * @return Valor da carta (ex: Valor::As, Valor::Rei).
     */
    Valor mostraValor() const;

    /**
     * @brief Retorna o naipe da carta como enum Naipe.
     * @return Naipe da carta.
     */
    Naipe mostraNaipe() const;

    /**
     * @brief Verifica se a carta possui valor e naipe válidos.
     * @return true se a carta é válida (valor entre 1–13 e naipe entre 0–3).
     */
    bool validacaoCarta() const;

    /**
     * @brief Retorna o valor da carta como string legível.
     * @return String do valor (ex: "A", "J", "Q", "K", "2"–"10").
     */
    std::string valorString() const;

    /**
     * @brief Retorna o naipe da carta como string legível.
     * @return String do naipe (ex: "Paus", "Copas", "Espadas", "Ouros").
     */
    std::string naipeString() const;

    /**
     * @brief Retorna a representação completa da carta como string.
     * @return String no formato "valor de naipe" (ex: "A de Paus", "K de Ouros").
     */
    std::string cartaString() const;

    ///@}
};

#endif