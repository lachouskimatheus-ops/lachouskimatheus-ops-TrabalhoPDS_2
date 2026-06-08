#include "routes/PifeRoutes.hpp"
#include "multiplayer/Pife/PifeWebSocket.hpp"

void PifeRoutes::registrar(crow::SimpleApp& app) {
    PifeWebSocket::registrar(app);
}