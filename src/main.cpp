#include "crow.h"

int main2();

int main()
{
  main2();
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    app.port(18080).run();
}
