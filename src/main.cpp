#include "crow.h"

#include "User.hpp"

int main2();

int main() {
  main2();
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")
  ([]() {
    auto username = get_username();
    auto ip = get_ip_address();
    auto mac = get_mac_address();

    auto page = crow::mustache::load("index.html");
    crow::mustache::context ctx({{"name", get_username()},
                                 {"ip", get_ip_address()},
                                 {"mac", get_mac_address()}});
    return page.render(ctx);
  });

  app.port(8080).multithreaded().run();
}
