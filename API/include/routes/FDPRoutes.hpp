#pragma once

#include "dependencias/crow_all.h"

class FDPRoutes {
public:
    static void registrar(crow::SimpleApp& app);
};