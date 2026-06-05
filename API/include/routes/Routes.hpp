#pragma once

#include "dependencias/crow_all.h"

//vai 'armazenar todas as routes' em um app

class Routes {
public:
    static void registrarRotas(crow::SimpleApp& app);
};