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

    auto now = std::chrono::system_clock::now();
    auto y =
        std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(now))
            .year();
    crow::mustache::context ctx({{"name", get_username()},
                                 {"ip", get_ip_address()},
                                 {"mac", get_mac_address()}});
    ctx["year"] = std::to_string(int(y));

    return page.render(ctx);
  });

  app.port(8080).multithreaded().run();
}
