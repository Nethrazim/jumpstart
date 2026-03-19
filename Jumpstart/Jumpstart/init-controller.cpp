#include "init-controller.h"
#include "http_response.h"

Response* InitController::handleGet(const Request& req)
{
    Response* resp = new Response();

    // Static - initialized only ONCE, reused for all requests
    static const std::string htmlContent = []() {
        std::string body;
        body.reserve(15000);  // Pre-allocate to avoid reallocations

        
        body += R"(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Professional Resume - Full Stack Developer</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      line-height: 1.6;
      color: #333;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      padding: 2rem 1rem;
    }
    .resume {
      max-width: 1000px;
      margin: 0 auto;
      background: white;
      box-shadow: 0 20px 60px rgba(0,0,0,0.3);
      border-radius: 10px;
      overflow: hidden;
    }
    .header {
      background: linear-gradient(135deg, #2d3748 0%, #1a202c 100%);
      color: white;
      padding: 3rem 2rem;
      text-align: center;
    }
    .header h1 { font-size: 2.5rem; margin-bottom: 0.5rem; font-weight: 700; }
    .header .title { font-size: 1.3rem; color: #a0aec0; margin-bottom: 1rem; }
    .header .experience-badge {
      display: inline-block;
      background: #667eea;
      padding: 0.5rem 1.5rem;
      border-radius: 25px;
      font-weight: 600;
      margin-top: 1rem;
    }
    .content { padding: 2rem; }
    .section { margin-bottom: 2.5rem; }
    .section-title {
      font-size: 1.8rem;
      color: #2d3748;
      border-bottom: 3px solid #667eea;
      padding-bottom: 0.5rem;
      margin-bottom: 1.5rem;
      display: flex;
      align-items: center;
    }
    .section-title::before {
      content: '';
      width: 6px;
      height: 30px;
      background: #667eea;
      margin-right: 1rem;
      border-radius: 3px;
    }
    .skills-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      gap: 1rem;
    }
    .skill-category {
      background: #f7fafc;
      padding: 1rem;
      border-radius: 8px;
      border-left: 4px solid #667eea;
    }
    .skill-category h3 { color: #2d3748; font-size: 1.1rem; margin-bottom: 0.5rem; }
    .skill-tag {
      display: inline-block;
      background: #edf2f7;
      color: #4a5568;
      padding: 0.3rem 0.8rem;
      margin: 0.2rem;
      border-radius: 15px;
      font-size: 0.9rem;
      border: 1px solid #cbd5e0;
    }
    .job {
      margin-bottom: 2rem;
      padding: 1.5rem;
      background: #f8f9fa;
      border-radius: 8px;
      border-left: 4px solid #667eea;
      transition: transform 0.2s;
    }
    .job:hover { transform: translateY(-2px); box-shadow: 0 4px 12px rgba(102,126,234,0.2); }
    .job-header {
      display: flex;
      justify-content: space-between;
      align-items: baseline;
      margin-bottom: 0.5rem;
      flex-wrap: wrap;
    }
    .company { font-size: 1.4rem; color: #2d3748; font-weight: 700; }
    .duration { color: #718096; font-size: 0.95rem; font-weight: 600; }
    .position { font-size: 1.1rem; color: #4a5568; margin-bottom: 0.5rem; font-weight: 600; }
    .location { color: #a0aec0; font-size: 0.9rem; margin-bottom: 1rem; }
    .achievements { margin: 1rem 0; }
    .achievements li { margin-left: 1.5rem; margin-bottom: 0.5rem; color: #4a5568; }
    .tech-stack { margin-top: 1rem; padding-top: 1rem; border-top: 1px solid #e2e8f0; }
    .tech-label { font-weight: 600; color: #2d3748; font-size: 0.9rem; }
    .project {
      background: white;
      padding: 1.5rem;
      margin-bottom: 1rem;
      border-radius: 8px;
      border: 2px solid #e2e8f0;
      transition: border-color 0.2s;
    }
    .project:hover { border-color: #667eea; }
    .project h3 { color: #2d3748; margin-bottom: 0.5rem; font-size: 1.2rem; }
    .project a { color: #667eea; text-decoration: none; font-weight: 600; }
    .project a:hover { text-decoration: underline; }
    .project-desc { color: #4a5568; margin-top: 0.5rem; }
    @media print {
      body { background: white; padding: 0; }
      .resume { box-shadow: none; }
    }
    @media (max-width: 768px) {
      .header h1 { font-size: 2rem; }
      .content { padding: 1rem; }
    }
  </style>
</head>
<body>
  <div class="resume">
    <div class="header">
      <h1>Full Stack Developer</h1>
      <h2>Vlad Branzei</h2>
      <div class="title">Enterprise Applications & Microservices Architect</div>
      <div class="experience-badge">13+ Years of Professional Experience</div>
    </div>
    <div class="content">
      <div class="section">
        <h2 class="section-title">Core Competencies</h2>
        <div class="skills-grid">
          <div class="skill-category">
            <h3>Backend & Architecture</h3>
            <div>
              <span class="skill-tag">C# / .NET 8</span>
              <span class="skill-tag">Microservices</span>
              <span class="skill-tag">REST APIs</span>
              <span class="skill-tag">NServiceBus</span>
              <span class="skill-tag">RabbitMQ</span>
              <span class="skill-tag">Kafka</span>
            </div>
          </div>
          <div class="skill-category">
            <h3>Frontend</h3>
            <div>
              <span class="skill-tag">Angular 19</span>
              <span class="skill-tag">TypeScript</span>
              <span class="skill-tag">RxJS</span>
              <span class="skill-tag">NgRx</span>
              <span class="skill-tag">Signals</span>
            </div>
          </div>
          <div class="skill-category">
            <h3>Database</h3>
            <div>
              <span class="skill-tag">SQL Server</span>
              <span class="skill-tag">T-SQL</span>
              <span class="skill-tag">EF Core</span>
              <span class="skill-tag">Performance Tuning</span>
            </div>
          </div>
          <div class="skill-category">
            <h3>Additional</h3>
            <div>
              <span class="skill-tag">C++</span>
              <span class="skill-tag">Azure</span>
              <span class="skill-tag">Multitenant Apps</span>
            </div>
          </div>
        </div>
      </div>)";

        // Experience section - Part 1
        body += R"(
      <div class="section">
        <h2 class="section-title">Professional Experience</h2>
        <div class="job">
          <div class="job-header">
            <div class="company">Yucca</div>
            <div class="duration">Jan 2026 - Present</div>
          </div>
          <div class="position">Full Stack Developer</div>
          <div class="location">Romania - Remote</div>
          <ul class="achievements">
            <li>Enterprise app development with .NET 8, Angular, and NServiceBus</li>
            <li>Implemented Sagas and Commands architecture</li>
          </ul>
          <div class="tech-stack">
            <span class="tech-label">Tech:</span> Angular, .NET 8, EF Core, SQL Server, NServiceBus
          </div>
        </div>
        <div class="job">
          <div class="job-header">
            <div class="company">Softlab360</div>
            <div class="duration">Feb 2024 - Dec 2025</div>
          </div>
          <div class="location">Romania - Remote</div>
          <div class="position">Angular Developer</div>
          <ul class="achievements">
            <li>UI modernization from AngularJS to Angular 19</li>
            <li>Tailwind, Angular Material, Signals, Jest, Playwright</li>
          </ul>
          <div class="position" style="margin-top:1rem;">.NET Full Stack Engineer</div>
          <ul class="achievements">
            <li>Fintech: WebForms to REST APIs migration</li>
            <li>Kafka integration, Windows Services, multitenancy</li>
            <li>ELK, Dynatrace, Azure monitoring</li>
          </ul>
          <div class="tech-stack">
            <span class="tech-label">Tech:</span> .NET 8, Angular 17, Kafka, T-SQL, Azure
          </div>
        </div>)";

        // Experience section - Part 2
        body += R"(
        <div class="job">
          <div class="job-header">
            <div class="company">Programming Pool</div>
            <div class="duration">Apr 2020 - Aug 2024</div>
          </div>
          <div class="location">Romania - Remote</div>
          <div class="position">Full Stack Engineer</div>
          <ul class="achievements">
            <li>Monolith to microservices with MassTransit & RabbitMQ</li>
            <li>xUnit, FluentAssertions testing</li>
            <li>Angular 17, NX workspace, NgRx</li>
          </ul>
          <div class="tech-stack">
            <span class="tech-label">Tech:</span> .NET 8, MassTransit, RabbitMQ, Angular, NX
          </div>
        </div>
        <div class="job">
          <div class="job-header">
            <div class="company">Micro Focus</div>
            <div class="duration">Oct 2019 - Mar 2020</div>
          </div>
          <div class="position">SQL Server Developer</div>
          <ul class="achievements">
            <li>Performance tuning, deadlock resolution</li>
            <li>T-SQL optimization</li>
          </ul>
        </div>
        <div class="job">
          <div class="job-header">
            <div class="company">Previous Roles</div>
            <div class="duration">2013 - 2020</div>
          </div>
          <ul class="achievements">
            <li>SQL optimization & .NET development</li>
            <li>SAP Fiori migration</li>
            <li>Gaming platform tools</li>
          </ul>
        </div>
      </div>)";

        // Projects section
        body += R"(
      <div class="section">
        <h2 class="section-title">Open Source Projects</h2>
        <div class="project">
          <h3>Jumpstart - TCP/IP HTTP Server</h3>
          <a href="https://github.com/Nethrazim/jumpstart">github.com/Nethrazim/jumpstart</a>
          <div class="project-desc">
            Cross-platform TCP/IP server with HTTP parser, MVC, routing.
            Non-blocking I/O, thread pool, queues.
          </div>
          <div class="tech-stack">
            <span class="tech-label">Tech:</span> C++17, BSD Sockets, CMake
          </div>
        </div>
        <div class="project">
          <h3>Legends of the Fey - OpenGL 3D Viewer</h3>
          <a href="https://github.com/Nethrazim/legends-of-the-fey">github.com/Nethrazim/legends-of-the-fey</a>
          <div class="project-desc">OpenGL 3D scene viewer with shaders</div>
        </div>
        <div class="project">
          <h3>C++ Practice Repository</h3>
          <a href="https://github.com/Nethrazim/cpptutorial2025">github.com/Nethrazim/cpptutorial2025</a>
          <div class="project-desc">Modern C++ learning</div>
        </div>
      </div>
    </div>
  </div>
</body>
</html>)";
        return body;
    }();  // ← Immediately invoked lambda (executes once)

    resp->raw = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: " + std::to_string(htmlContent.size()) + "\r\n"
        "\r\n" +
        htmlContent;

    return resp;
}