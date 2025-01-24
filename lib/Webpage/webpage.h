#ifndef WEBPAGE_H
#define WEBPAGE_H

const char webpageHTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Configuração Wi-Fi</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
        input { margin: 10px; padding: 10px; width: 300px; }
        button { padding: 10px 20px; }
    </style>
</head>
<body>
    <h1>Configurar Wi-Fi</h1>
    <form action="/setWiFi" method="post">
        <input type="text" name="ssid" placeholder="SSID da Rede" required><br>
        <input type="password" name="password" placeholder="Senha" required><br>
        <button type="submit">Conectar</button>
    </form>
</body>
</html>
)rawliteral";

#endif
