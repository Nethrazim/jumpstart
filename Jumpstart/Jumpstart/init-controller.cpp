#include "init-controller.h"
#include "http_response.h"

Response* InitController::handleGet(const Request& req)
{
    Response* resp = new Response();

    std::string body = "Hello from GET " + req.path + "\n";

    body += u8R"(<!DOCTYPE html>
        <body>
            <span style="font-size:24px; color: red;">mama mia</span>
        </body>
        </html>)";

    resp->raw = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" +
        body;

    return resp;
}