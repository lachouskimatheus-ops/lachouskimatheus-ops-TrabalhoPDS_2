#include "routes/FrontendRoutes.hpp"

void FrontendRoutes::registrar(crow::SimpleApp& app) {

    CROW_ROUTE(app, "/pages/<path>")
    ([](const std::string& path){

        crow::response res;
        res.set_static_file_info("frontend/pages/" + path);

        return res;
    });

    CROW_ROUTE(app, "/assets/<path>")
    ([](const std::string& path){

        crow::response res;
        res.set_static_file_info("frontend/assets/" + path);

        return res;
    });

    CROW_ROUTE(app, "/css/<path>")
    ([](const std::string& path){

        crow::response res;
        res.set_static_file_info("frontend/css/" + path);

        return res;
    });

    CROW_ROUTE(app, "/js/<path>")
    ([](const std::string& path){

        crow::response res;
        res.set_static_file_info("frontend/js/" + path);

        return res;
    });
}