#ifndef WEBPAGE_H
#define WEBPAGE_H

const char webpageHTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Configurar RTC e Wi-Fi</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin-top: 50px;
        }
        input, button {
            margin: 10px;
            padding: 10px;
            width: 300px;
        }
    </style>
    <script>
        // Função para buscar a data e hora do RTC
        async function fetchRTC() {
            try {
                const response = await fetch('/getRTC');
                const data = await response.text();
                document.getElementById('currentRTC').innerText = data;
            } catch (error) {
                console.error('Erro ao buscar RTC:', error);
                document.getElementById('currentRTC').innerText = "Erro ao obter dados.";
            }
        }

        // Atualiza o RTC automaticamente a cada 5 segundos
        setInterval(fetchRTC, 5000);

        // Atualiza ao carregar a página
        document.addEventListener('DOMContentLoaded', fetchRTC);
    </script>
</head>
<body>
    <h1>Configurar RTC e Wi-Fi</h1>
    
    <h3>Data e Hora Atual: <span id="currentRTC">Carregando...</span></h3>
    
    <h2>Configurar Data e Hora</h2>
    <form action="/setRTC" method="post">
        <input type="number" name="year" placeholder="Ano (YYYY)" required><br>
        <input type="number" name="month" placeholder="Mes (1-12)" required><br>
        <input type="number" name="day" placeholder="Dia (1-31)" required><br>
        <input type="number" name="hour" placeholder="Hora (0-23)" required><br>
        <input type="number" name="minute" placeholder="Minuto (0-59)" required><br>
        <input type="number" name="second" placeholder="Segundo (0-59)" required><br>
        <button type="submit">Ajustar</button>
    </form>

    <h2>Configurar Wi-Fi</h2>
    <form action="/setWiFi" method="post">
        <input type="text" name="ssid" placeholder="SSID (nome da rede)" required><br>
        <input type="password" name="password" placeholder="Senha" required><br>
        <button type="submit">Conectar</button>
    </form>
</body>
</html>
)rawliteral";

#endif