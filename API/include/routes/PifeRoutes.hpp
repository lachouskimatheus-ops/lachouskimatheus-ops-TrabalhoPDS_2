#pragma once

#include "dependencias/crow_all.h"

class PifeRoutes {
public:
    static void registrar(crow::SimpleApp& app);
};