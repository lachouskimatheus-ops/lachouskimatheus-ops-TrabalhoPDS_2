#pragma once

#include "crow_all.h"
#include "json.hpp"
#include <string>

using json = nlohmann::json;

/* Padroniza todas as respostas da API */
class Responses {
public:
    static crow::response sucesso(const std::string& mensagem, const json& dados = {});

    static crow::response erro(const std::string& mensagem, int codigo = 400);
};