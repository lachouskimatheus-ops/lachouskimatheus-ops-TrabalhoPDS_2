#include "Routes.hpp"
#include "coreAPI/Responses.hpp"

void Routes::registrarRotas(crow::SimpleApp& app){

    CROW_ROUTE(app, "/") //rotas de testes para validar API
    ([]() {
        return Responses::sucesso("API funcionando");
    });


    CROW_ROUTE(app, "/ping")
    ([]() {
        return Responses::sucesso("pong");

    });
}