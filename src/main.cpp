#include "crow.h"

int main2();

int main()
{
  main2();
    crow::SimpleApp app;

 CROW_ROUTE(app, "/")
    ([](){
        return R"(
            <html>
                <head><title>Hello</title></head>
                <body><h1>Hello, Crow!</h1></body>
            </html>
        )";
    });

    app.port(8080).multithreaded().run();
}
