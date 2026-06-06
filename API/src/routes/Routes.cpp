#include "routes/Routes.hpp"
#include "routes/FrontendRoutes.hpp"
#include "routes/PacienciaRoutes.hpp"
#include "routes/PifeRoutes.hpp"
#include "routes/FDPRoutes.hpp"

void Routes::registrarRotas(crow::SimpleApp& app) {

    CROW_ROUTE(app, "/")
    ([]() {
        crow::response res;
        res.set_static_file_info("frontend/pages/menu.html");
        return res;
    });

    FrontendRoutes::registrar(app);
    PacienciaRoutes::registrar(app);
    PifeRoutes::registrar(app);
    FDPRoutes::registrar(app);
}