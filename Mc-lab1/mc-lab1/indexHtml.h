#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP8266 LED Control</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            padding: 20px;
        }
        .container {
            max-width: 400px;
            background: white;
            padding: 20px;
            margin: auto;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        h2 {
            color: #333;
        }
        .button {
            display: inline-block;
            background: #007BFF;
            color: white;
            border: none;
            padding: 12px 20px;
            font-size: 18px;
            border-radius: 5px;
            cursor: pointer;
            transition: background 0.3s;
        }
        .button:hover {
            background: #0056b3;
        }
        .status {
            margin-top: 20px;
            font-size: 18px;
            color: #555;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>ESP8266 LED Control</h2>
        <button class="button" onclick="sendCommand('next')">Toggle LED</button>
        <p class="status">Status: <span id="led-status">Waiting...</span></p>
    </div>

    <script>
        function sendCommand(command) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/toggle?led=" + command, true);
            xhr.send();
            
            xhr.onreadystatechange = function () {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    document.getElementById("led-status").innerText = "LED Toggled!";
                }
            };
        }
    </script>
</body>
</html>
)=====";

#endif
