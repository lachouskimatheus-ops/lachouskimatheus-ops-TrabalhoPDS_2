#pragma once

#include "dependencias/crow_all.h"
#include "MesaFDP.hpp"

class FDPRoutes {
public:
    static void registrar(crow::SimpleApp& app);
};