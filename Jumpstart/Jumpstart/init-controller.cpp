#include "init-controller.h"
#include "http_response.h"

Response* InitController::handleGet(const Request& req)
{
    Response* resp = new Response();

    std::string body = "";

    body += u8R"(<!DOCTYPE html>
        <html lang="en">
        <head>
          <meta charset="UTF-8" />
          <title>Professional CV</title>
          <meta name="viewport" content="width=device-width, initial-scale=1" />
          <style>
            body {
              font-family: Arial, sans-serif;
              background: #f9f9f9;
              margin: 0;
              padding: 2rem;
              color: #333;
            }
            .container {
              max-width: 960px;
              margin: auto;
              background: #fff;
              padding: 2rem;
              border-radius: 8px;
              box-shadow: 0 0 10px rgba(0,0,0,0.05);
            }
            h1, h2, h3 {
              margin-bottom: 0.5rem;
            }
            h1 {
              font-size: 2rem;
              border-bottom: 2px solid #ddd;
              padding-bottom: 0.5rem;
            }
            h2 {
              font-size: 1.4rem;
              margin-top: 2rem;
              color: #444;
            }
            h3 {
              font-size: 1.1rem;
              margin-top: 1rem;
            }
            .role {
              margin-bottom: 1rem;
            }
            .meta {
              font-size: 0.9rem;
              color: #666;
            }
            ul {
              padding-left: 1.2rem;
            }
            .skills {
              margin-top: 0.5rem;
              font-size: 0.9rem;
              color: #555;
            }
          </style>
        </head>
        <body>
          <div class="container">
            <h1>Experience</h1>

            <div class="role">
              <h2>Yucca</h2>
              <h3>Full Stack Developer - Angular / .NET 8 / SQL Server</h3>
              <p class="meta">Jan 2024 - Present  Romania Remote</p>
              <ul>
                <li>Angular 17, SQL, C#, .NET 8, EF Core, SQL Server</li>
                <li>NServiceBus, Sagas, Commands architecture</li>
              </ul>
              <p class="skills">Skills: .NET Core, T-SQL, C#, JavaScript/TypeScript, SQL Server, Angular, ASP.NET Web API</p>
            </div>

            <div class="role">
              <h2>Softlab360</h2>
              <p class="meta">11 yr 11 mos · Romania Remote</p>

              <h3>Angular / Typescript Developer</h3>
              <p class="meta">Oct 2015 - Dec 2023</p>
              <ul>
                <li>UI rebuild from AngularJS to Angular 19+</li>
                <li>Technologies: Angular Material, Signals/i18n, Capacitor, Copilot, Jest, Playwright</li>
              </ul>
              <p class="skills">Skills: JavaScript/TypeScript</p>

              <h3>.NET Full Stack Engineer</h3>
              <p class="meta">Feb 2022 - Sep 2023</p>
              <ul>
                <li>Fintech Wealth Management Portal</li>
                <li>Migration from .NET WebForms to REST APIs and Angular</li>
                <li>Kafka integrations, workflows, multitenancy refactor</li>
                <li>Exposure to ELK, Dynatrace, Azure Portal</li>
              </ul>
              <p class="skills">Skills: .NET Core, VB.NET, T-SQL, PnPjs, WebForms, C#, JavaScript/TypeScript, SQL Server, Angular</p>
            </div>

            <div class="role">
              <h2>Programming Pool</h2>
              <p class="meta">4 yr 5 mos · Romania Remote</p>

              <h3>Angular / Typescript Developer</h3>
              <p class="meta">Jun 2023 - Aug 2023</p>
              <ul>
                <li>Internal project setup and proof of concept</li>
                <li>Technologies: Angular 17, NX workspace</li>
              </ul>
              <p class="skills">Skills: Angular Material, JavaScript/TypeScript, Angular</p>

              <h3>Full Stack Engineer</h3>
              <p class="meta">Apr 2020 - May 2024</p>
              <ul>
                <li>Monolith to microservices transformation</li>
                <li>MassTransit, RabbitMQ, xUnit, FluentAssertions</li>
                <li>Angular 17 UI rebuild, MVP delivery</li>
              </ul>
              <p class="skills">Skills: .NET Core, T-SQL, WebForms, .NET Framework, C#, JavaScript/TypeScript, SQL Server, Angular</p>
            </div>
          </div>
        </body>
        </html>
        )";

    resp->raw = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" +
        body;

    return resp;
}