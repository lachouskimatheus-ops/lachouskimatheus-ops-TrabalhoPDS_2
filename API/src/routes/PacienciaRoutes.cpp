#include "routes/PacienciaRoutes.hpp"

#include "multiplayer/Paciencia/PacienciaWebSocket.hpp"

void PacienciaRoutes::registrar(crow::SimpleApp& app) {
    PacienciaWebSocket::registrar(app);
}