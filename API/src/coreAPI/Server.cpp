#include "coreAPI/Server.hpp"
#include "routes/Routes.hpp"


void Server::iniciar(int porta){
    Routes::registrarRotas(app_); //passa o app_ para o sistema de rotas para registras as rotas

    app_.port(porta) //defina a porta que vai usar
        .multithreaded() //libera múltiplas threadeds 
        .run(); //incial o loop do server
}