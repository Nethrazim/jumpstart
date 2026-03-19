#include <string>
#include <functional>

using std::string;

std::function<string()> initViewHtmlContent = []() {
    std::string body;
    body.reserve(15000);  // Pre-allocate to avoid reallocations

    body += R"(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Vlad Branzei - Full Stack Developer</title>
  <style>
    @import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800&display=swap');

    * { margin: 0; padding: 0; box-sizing: border-box; }

    @keyframes gradientShift {
      0% { background-position: 0% 50%; }
      50% { background-position: 100% 50%; }
      100% { background-position: 0% 50%; }
    }

    body {
      font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
      line-height: 1.7;
      color: #1a202c;
      background: linear-gradient(-45deg, #667eea, #764ba2, #f093fb, #4facfe);
      background-size: 400% 400%;
      animation: gradientShift 15s ease infinite;
      padding: 3rem 1.5rem;
      min-height: 100vh;
    }

    .resume {
      max-width: 1100px;
      margin: 0 auto;
      background: rgba(255, 255, 255, 0.95);
      backdrop-filter: blur(20px);
      box-shadow: 0 30px 90px rgba(0,0,0,0.25), 0 10px 30px rgba(0,0,0,0.15);
      border-radius: 24px;
      overflow: hidden;
      border: 1px solid rgba(255,255,255,0.3);
    }

    .header {
      position: relative;
      background: linear-gradient(135deg, #1a202c 0%, #2d3748 50%, #4a5568 100%);
      color: white;
      padding: 4rem 2.5rem 3rem;
      text-align: center;
      overflow: hidden;
    }

    .header::before {
      content: '';
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background: linear-gradient(180deg, rgba(102,126,234,0.1) 0%, transparent 100%);
      opacity: 0.3;
    }

    .header-content { position: relative; z-index: 1; }

    .name { 
      font-size: 3.5rem; 
      font-weight: 800; 
      margin-bottom: 0.5rem;
      letter-spacing: -0.03em;
      background: linear-gradient(135deg, #fff 0%, #e2e8f0 100%);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      background-clip: text;
    }

    .role { 
      font-size: 1.5rem; 
      color: #cbd5e0;
      font-weight: 300;
      margin-bottom: 0.75rem;
      letter-spacing: 0.05em;
    }

    .tagline {
      font-size: 1.1rem;
      color: #a0aec0;
      margin-bottom: 1.5rem;
      font-weight: 400;
    }

    .experience-badge {
      display: inline-flex;
      align-items: center;
      gap: 0.5rem;
      background: rgba(102,126,234,0.2);
      border: 2px solid rgba(102,126,234,0.3);
      padding: 0.75rem 1.75rem;
      border-radius: 50px;
      font-weight: 600;
      font-size: 0.95rem;
      backdrop-filter: blur(10px);
      transition: all 0.3s ease;
    }

    .experience-badge:hover {
      background: rgba(102,126,234,0.3);
      border-color: rgba(102,126,234,0.5);
      transform: translateY(-2px);
    }

    .experience-badge::before {
      content: '⚡';
      font-size: 1.2rem;
    }

    .content { padding: 3rem 2.5rem; }

    .section { margin-bottom: 3.5rem; }

    .section-title {
      font-size: 2rem;
      font-weight: 700;
      color: #1a202c;
      margin-bottom: 2rem;
      padding-bottom: 1rem;
      border-bottom: 3px solid transparent;
      border-image: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
      border-image-slice: 1;
      display: flex;
      align-items: center;
      gap: 1rem;
    }

    .section-title::before {
      content: '';
      width: 8px;
      height: 40px;
      background: linear-gradient(180deg, #667eea 0%, #764ba2 100%);
      border-radius: 10px;
      box-shadow: 0 4px 12px rgba(102,126,234,0.4);
    }

    .skills-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
      gap: 1.25rem;
    }

    .skill-category {
      background: linear-gradient(135deg, #f7fafc 0%, #edf2f7 100%);
      padding: 1.5rem;
      border-radius: 16px;
      border-left: 5px solid #667eea;
      box-shadow: 0 4px 15px rgba(0,0,0,0.05);
      transition: all 0.3s ease;
    }

    .skill-category:hover {
      transform: translateY(-5px);
      box-shadow: 0 8px 25px rgba(102,126,234,0.15);
      border-left-color: #764ba2;
    }

    .skill-category h3 { 
      color: #2d3748;
      font-size: 1.15rem;
      font-weight: 700;
      margin-bottom: 1rem;
      display: flex;
      align-items: center;
      gap: 0.5rem;
    }

    .skill-category h3::before {
      content: '▸';
      color: #667eea;
      font-weight: 800;
    }

    .skill-tag {
      display: inline-block;
      background: white;
      color: #4a5568;
      padding: 0.5rem 1rem;
      margin: 0.3rem;
      border-radius: 20px;
      font-size: 0.9rem;
      font-weight: 500;
      border: 1.5px solid #e2e8f0;
      transition: all 0.2s ease;
    }

    .skill-tag:hover {
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      color: white;
      border-color: transparent;
      transform: translateY(-2px);
      box-shadow: 0 4px 12px rgba(102,126,234,0.3);
    }

    .job {
      margin-bottom: 2rem;
      padding: 2rem;
      background: linear-gradient(135deg, #ffffff 0%, #f7fafc 100%);
      border-radius: 16px;
      border: 1px solid #e2e8f0;
      box-shadow: 0 4px 20px rgba(0,0,0,0.06);
      transition: all 0.3s ease;
      position: relative;
      overflow: hidden;
    }

    .job::before {
      content: '';
      position: absolute;
      left: 0;
      top: 0;
      bottom: 0;
      width: 5px;
      background: linear-gradient(180deg, #667eea 0%, #764ba2 100%);
      transition: width 0.3s ease;
    }

    .job:hover {
      transform: translateX(5px);
      box-shadow: 0 8px 30px rgba(102,126,234,0.15);
    }

    .job:hover::before { width: 8px; }

    .job-header {
      display: flex;
      justify-content: space-between;
      align-items: baseline;
      margin-bottom: 1rem;
      flex-wrap: wrap;
      gap: 0.5rem;
    }

    .company { 
      font-size: 1.5rem;
      color: #1a202c;
      font-weight: 800;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      background-clip: text;
    }

    .duration { 
      color: #718096;
      font-size: 0.95rem;
      font-weight: 600;
      background: #edf2f7;
      padding: 0.25rem 0.75rem;
      border-radius: 12px;
    }

    .position { 
      font-size: 1.15rem;
      color: #2d3748;
      margin-bottom: 0.5rem;
      font-weight: 700;
    }

    .location { 
      color: #a0aec0;
      font-size: 0.9rem;
      margin-bottom: 1rem;
      display: inline-flex;
      align-items: center;
      gap: 0.25rem;
    }

    .location::before { content: '📍'; }

    .achievements { margin: 1.25rem 0; }

    .achievements li {
      margin-left: 1.5rem;
      margin-bottom: 0.75rem;
      color: #4a5568;
      padding-left: 0.5rem;
    }

    .achievements li::marker { color: #667eea; }

    .tech-stack {
      margin-top: 1.25rem;
      padding-top: 1.25rem;
      border-top: 2px solid #edf2f7;
    }

    .tech-label {
      font-weight: 700;
      color: #667eea;
      font-size: 0.9rem;
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    .project {
      background: white;
      padding: 2rem;
      margin-bottom: 1.25rem;
      border-radius: 16px;
      border: 2px solid #e2e8f0;
      box-shadow: 0 4px 20px rgba(0,0,0,0.06);
      transition: all 0.3s ease;
      position: relative;
    }

    .project::after {
      content: '→';
      position: absolute;
      right: 2rem;
      top: 50%;
      transform: translateY(-50%);
      font-size: 2rem;
      color: #667eea;
      opacity: 0;
      transition: all 0.3s ease;
    }

    .project:hover {
      border-color: #667eea;
      transform: translateY(-5px);
      box-shadow: 0 12px 40px rgba(102,126,234,0.2);
    }

    .project:hover::after {
      opacity: 1;
      right: 1.5rem;
    }

    .project h3 {
      color: #1a202c;
      margin-bottom: 0.75rem;
      font-size: 1.3rem;
      font-weight: 700;
    }

    .project a {
      color: #667eea;
      text-decoration: none;
      font-weight: 600;
      font-size: 0.95rem;
      transition: color 0.2s ease;
    }

    .project a:hover {
      color: #764ba2;
      text-decoration: underline;
    }

    .project-desc {
      color: #4a5568;
      margin-top: 1rem;
      line-height: 1.7;
    }

    @media print {
      body { background: white; padding: 0; animation: none; }
      .resume { box-shadow: none; }
    }

    @media (max-width: 768px) {
      body { padding: 1.5rem 1rem; }
      .header { padding: 3rem 1.5rem 2rem; }
      .name { font-size: 2.5rem; }
      .role { font-size: 1.25rem; }
      .content { padding: 2rem 1.5rem; }
      .section-title { font-size: 1.6rem; }
      .skills-grid { grid-template-columns: 1fr; }
    }
  </style>
</head>
<body>
  <div class="resume">
    <div class="header">
      <div class="header-content">
        <div class="name">Vlad Branzei</div>
        <div class="role">Full Stack Developer</div>
        <div class="tagline">Enterprise Applications & Data Centric Applications</div>
        <div class="experience-badge">13+ Years of Professional Experience</div>
      </div>
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
};