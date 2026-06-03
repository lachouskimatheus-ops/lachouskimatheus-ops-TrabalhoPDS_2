#include "coreAPI/Responses.hpp"

crow::response sucesso(const std::string& mensagem, const json& dados = {}){
    json resposta;
    resposta["sucess"] = true;
    resposta["message"] = mensagem;
    resposta["data"] = dados;

    return crow::response(200, resposta.dump());
}

crow::response erro(const std::string& mensagem, int codigo){
    json resposta;

    resposta["sucess"] = false;
    resposta["message"] = mensagem;

    return crow::response(codigo, resposta.dump());
}