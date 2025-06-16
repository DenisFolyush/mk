#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
extern bool reverseOrder;

void handleRoot() {
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="uk">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP8266 Управління</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                text-align: center;
                background-color: #f4f4f4;
                padding: 20px;
            }
            .container {
                max-width: 400px;
                margin: auto;
                background: white;
                padding: 20px;
                box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                border-radius: 8px;
            }
            h1 {
                color: #333;
            }
            p {
                font-size: 18px;
            }
            button {
                background: #007bff;
                color: white;
                border: none;
                padding: 15px;
                font-size: 18px;
                cursor: pointer;
                margin: 10px;
                width: 100%;
                border-radius: 5px;
            }
            button:hover {
                background: #0056b3;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>Управління напрямком світіння</h1>
            <button onclick="toggleDirection()">Змінити напрямок</button>
        </div>
    
        <script>
            function toggleDirection() {
                fetch("/toggle").then(response => console.log("Напрямок змінено"));
            }
        </script>
    </body>
    </html>
    )rawliteral";
    
    server.send(200, "text/html", html);
}

void handleToggle() {
    reverseOrder = !reverseOrder;
    Serial.println("Напрямок змінено через веб-інтерфейс.");
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

void setupWebServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/toggle", HTTP_GET, handleToggle);
    server.begin();
    Serial.println("Сервер запущено!");
}

#endif // WEB_SERVER_H
