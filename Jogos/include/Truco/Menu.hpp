/**
 * @file Menu.hpp
 * @brief Definição das estruturas do menu de controle e seleção textual no console.
 */

#pragma once
#include <string>
#include <functional>
#include <vector>

/**
 * @struct OpcaoDoMenu
 * @brief Estrutura que mapeia o nome de uma opção de texto e seu respectivo callback de execução.
 */
struct OpcaoDoMenu {
    std::string nome_opcao;         ///< Rótulo textual que descreve a opção no menu.
    std::function<void()> acao;     ///< Função ou método que será engatado e disparado quando a opção for escolhida.
};

/**
 * @class Menu
 * @brief Responsável por renderizar a interface CLI básica, capturar entradas e redirecionar o fluxo para os jogos.
 */
class Menu {
private:
    std::vector<OpcaoDoMenu> opcoes; ///< Coleção de opções cadastradas no menu.

public:
    /**
     * @brief Cadastra uma nova opção mapeada dentro do contêiner do Menu.
     * @param nome Rótulo textual da ação.
     * @param acao Instância funcional `std::function` contendo o comportamento a ser ativado.
     */
    void addOpcao(std::string nome, std::function<void()> acao);

    /**
     * @brief Imprime o menu estruturado na tela do console e aguarda a entrada do usuário para executar o callback.
     */
    void exibir();
};