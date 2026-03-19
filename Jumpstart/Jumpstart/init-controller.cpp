#include <string>

#include "resume.h"
#include "init-controller.h"
#include "http_response.h"


Response* InitController::handleGet(const Request& req)
{
    Response* resp = new Response();
    
    static const std::string htmlContent = initViewHtmlContent();

    resp->raw = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: " + std::to_string(htmlContent.size()) + "\r\n"
        "\r\n" +
        htmlContent;

    return resp;
}