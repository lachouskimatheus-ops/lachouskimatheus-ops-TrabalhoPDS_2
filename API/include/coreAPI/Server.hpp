#pragma once
#include "crow_all.h"

class Server{
private:
    crow::SimpleApp app_;

public:
    void iniciar(int porta = 18080);

};