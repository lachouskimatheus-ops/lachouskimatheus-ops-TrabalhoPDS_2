/**
 * @file Responses.hpp
 * @brief Padronização e encapsulamento de respostas HTTP da API Crow.
 */

#pragma once

#include "crow_all.h"
#include "json.hpp"
#include <string>

using json = nlohmann::json;

/**
 * @class Responses
 * @brief Classe utilitária estática para fabricar objetos crow::response padronizados.
 *
 * Garante que todas as rotas REST da aplicação respondam utilizando a mesma estrutura
 * de JSON, facilitando o tratamento de dados e capturas de erro no frontend.
 */
class Responses {
public:
    /**
     * @brief Cria uma resposta HTTP de sucesso (Status 200 OK) com payload JSON.
     * @param mensagem Texto descritivo da operação bem-sucedida.
     * @param dados Objeto JSON opcional contendo estruturas, estados de mesas ou cartas.
     * @return Objeto crow::response configurado com `application/json`.
     */
    static crow::response sucesso(const std::string& mensagem, const json& dados = {});

    /**
     * @brief Cria uma resposta HTTP de erro/falha com código de status customizável.
     * @param mensagem Texto explicativo detalhando o motivo da rejeição ou falha.
     * @param codigo Código de status HTTP (padrão: 400 Bad Request).
     * @return Objeto crow::response contendo o erro estruturado em JSON.
     */
    static crow::response erro(const std::string& mensagem, int codigo = 400);
};