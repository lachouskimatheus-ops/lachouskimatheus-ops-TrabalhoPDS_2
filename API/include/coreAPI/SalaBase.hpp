/**
 * @file SalaBase.hpp
 * @brief Definição da classe base polimórfica para gestão de salas e conexões.
 */

#ifndef SALA_BASE_HPP
#define SALA_BASE_HPP

#include <string>
#include <vector>

#include "coreAPI/JogadorConectado.hpp"

/**
 * @class SalaBase
 * @brief Classe de controlo abstrata para gerir o agrupamento de jogadores e sessões de rede.
 *
 * Fornece todos os métodos necessários para adicionar utilizadores, controlar assentos,
 * validar chaves de segurança (tokens de reconexão) e supervisionar o estado de lotação
 * de uma partida multilink antes de despachar comandos para as engines lógicas.
 */
class SalaBase {
protected:
    std::string idSala_;             ///< Identificador único alfanumérico da sala (UUID ou Código).
    int maxJogadores_;               ///< Limite teto de assentos disponíveis configurados para a sala.
    int jogadoresConectados_;        ///< Contador em tempo real de ligações ativas em simultâneo.

    std::vector<JogadorConectado> jogadores_; ///< Contentor contendo a lista e o estado de rede de cada assento.

public:
    /**
     * @brief Construtor da SalaBase.
     * @param idSala String identificadora que batiza a sala.
     * @param maxJogadores Quantidade máxima de assentos permitidos.
     */
    SalaBase(const std::string& idSala, int maxJogadores);

    /**
     * @brief Destrutor virtual padrão. Assegura a desalocação correta em classes derivadas.
     */
    virtual ~SalaBase() = default;

    /**
     * @brief Regista um novo utilizador num assento vago associando-o a um token.
     * @param tokenReconexao Chave única gerada para o cliente.
     * @return ID do assento atribuído (0 a max-1), ou -1 se não houver vagas.
     */
    int adicionarJogador(const std::string& tokenReconexao);

    /**
     * @brief Trata o evento de um jogador que caiu e está a tentar restabelecer o canal WebSocket.
     * @param tokenReconexao Chave de segurança apresentada pelo cliente para validação.
     * @return ID do assento recuperado, ou -1 se o token for inválido ou rejeitado.
     */
    int reconectarJogador(const std::string& tokenReconexao);

    /**
     * @brief Sinaliza que a ligação WebSocket de um determinado assento caiu.
     * * Não remove o jogador da sala, apenas altera o seu estado interno para offline.
     * @param idJogador ID do assento a ser desconectado.
     * @return true se o estado foi atualizado com sucesso.
     */
    bool desconectarJogador(int idJogador);

    /**
     * @brief Procura nas sessões registadas se algum assento possui o token fornecido.
     * @param tokenReconexao Chave textual de busca.
     * @return ID do assento correspondente ou -1 se não for localizado.
     */
    int buscarJogadorPorToken(const std::string& tokenReconexao) const;

    /**
     * @brief Verifica a existência prévia de um token de reconexão no vetor de registos.
     * @param tokenReconexao Token a ser testado.
     * @return true se o token já estiver registado na sala.
     */
    bool tokenExiste(const std::string& tokenReconexao) const;

    /**
     * @brief Consulta se um determinado assento está com uma ligação ativa na rede.
     * @param idJogador ID do assento.
     * @return true se estiver online, false se caiu ou offline.
     */
    bool jogadorEstaConectado(int idJogador) const;

    /**
     * @brief Verifica se a sala ainda dispõe de assentos vazios para novos registos.
     * @return true se houver pelo menos um assento livre.
     */
    bool podeAdicionarNovoJogador() const;

    /**
     * @brief Retorna o identificador alfanumérico da sala.
     * @return ID da sala como string.
     */
    std::string idSala() const;

    /**
     * @brief Retorna o limite máximo de jogadores configurados para esta sessão.
     * @return Número inteiro de vagas.
     */
    int maxJogadores() const;

    /**
     * @brief Retorna a quantidade de jogadores que estão online no momento.
     * @return Contagem inteira de utilizadores conectados.
     */
    int jogadoresConectados() const;

    /**
     * @brief Retorna o número de assentos que já foram reservados por tokens (ativos + instáveis).
     * @return Total de utilizadores inscritos na sala.
     */
    int jogadoresRegistrados() const;

    /**
     * @brief Verifica se todos os assentos possíveis da sala já foram preenchidos.
     * @return true se a sala estiver completamente cheia.
     */
    bool estaLotada() const;

    /**
     * @brief Verifica se todos os utilizadores registados estão simultaneamente online.
     * @return true se o painel de conexões estiver 100% ativo.
     */
    bool todosConectados() const;

    /**
     * @brief Verifica se não existe nenhum jogador registado ou ligado na sala.
     * @return true se a sala estiver totalmente vazia.
     */
    bool estaVazia() const;

    /**
     * @brief Fornece acesso direto e constante à coleção de estruturas dos estados de conexão.
     * @return Referência constante para o vetor `std::vector<JogadorConectado>`.
     */
    const std::vector<JogadorConectado>& jogadores() const;
};

#endif