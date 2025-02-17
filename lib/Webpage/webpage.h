#ifndef WEBPAGE_H
#define WEBPAGE_H

const char webpageHTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Configuração do Dispositivo</title>
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@400;500;700&display=swap" rel="stylesheet">
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body {
            font-family: 'Roboto', sans-serif;
            display: flex;
            height: 100vh;
            background: #e3e6f3;
            color: #333;
        }
        .sidebar {
            width: 250px;
            background: #1d3557;
            color: #fff;
            padding: 20px;
            display: flex;
            flex-direction: column;
            gap: 30px;
        }
        .sidebar h2 {
            text-align: center;
            margin-bottom: 40px;
            font-size: 1.5em;
        }
        .sidebar ul {
            list-style: none;
        }
        .sidebar ul li {
            margin: 20px 0;
            cursor: pointer;
            padding: 10px;
            border-radius: 5px;
            transition: background 0.3s;
        }
        .sidebar ul li:hover {
            background: #457b9d;
        }
        .content {
            flex: 1;
            padding: 40px;
            overflow-y: auto;
        }
        .content h1 {
            margin-bottom: 30px;
            color: #1d3557;
        }
        .card {
            background: #fff;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.2);
            padding: 20px;
            margin: 20px 0;
        }
        .card h3 {
            margin-bottom: 15px;
        }
        .network-info p {
            margin: 5px 0;
            font-weight: 500;
        }
        form {
            display: flex;
            flex-direction: column;
            gap: 15px;
            margin-top: 20px;
            max-width: 400px;
        }
        input {
            padding: 12px;
            border: 1px solid #ccc;
            border-radius: 5px;
            font-size: 1em;
        }
        button {
            padding: 12px;
            border: none;
            border-radius: 5px;
            background: #1d3557;
            color: #fff;
            font-size: 1em;
            cursor: pointer;
            transition: background 0.3s;
        }
        button:hover {
            background: #457b9d;
        }
        .hidden {
            display: none;
        }
    </style>
    <script>
        async function fetchRTC() {
            try {
                const response = await fetch('/getRTC');
                const data = await response.text();
                const rtcElement = document.getElementById('currentRTC');
                if (rtcElement) {
                    rtcElement.innerText = data;
                }
            } catch (error) {
                console.error('Erro ao buscar RTC:', error);
            }
        }

        async function fetchNetworkInfo() {
            try {
                const response = await fetch('/getNetworkInfo');
                const data = await response.json();
                document.getElementById('deviceIP').innerText = data.ip;
                document.getElementById('gatewayIP').innerText = data.gateway;
                document.getElementById('subnetMask').innerText = data.subnet;
                document.getElementById('dnsServer').innerText = data.dns;
                document.getElementById('macAddress').innerText = data.mac;
                document.getElementById('wifiSSID').innerText = data.ssid;
            } catch (error) {
                console.error('Erro ao buscar informações de rede:', error);
            }
        }

        async function fetchSerialNumber() {
            try {
                const response = await fetch('/getSerialNumber');
                const serial = await response.text();
                const snElement = document.getElementById('serialNumber');
                if (snElement) {
                    snElement.innerText = serial;
                }
                // Atualizar os tópicos MQTT no Node-RED com o serial number
                document.getElementById('mqttTemp').innerText = `tcc/sensores/temperatura/${serial}`;
                document.getElementById('mqttHumidity').innerText = `tcc/sensores/umidade_do_ar/${serial}`;
                document.getElementById('mqttLimits').innerText = `tcc/sensores/config/temperature_limits/${serial}`;
                
                // Enviar comandos para o Node-RED para alterar os tópicos dinamicamente
                await fetch(`/setMqttTopics?serial=${serial}`);

            } catch (error) {
                console.error('Erro ao buscar Serial Number:', error);
            }
        }

        function showSection(sectionId) {
            document.querySelectorAll('.section').forEach(el => el.classList.add('hidden'));
            const section = document.getElementById(sectionId);
            if (section) section.classList.remove('hidden');
        }

        document.addEventListener('DOMContentLoaded', () => {
            fetchRTC();
            fetchNetworkInfo();
            fetchSerialNumber();
            showSection('identificacao');
            setInterval(fetchRTC, 5000);
            setInterval(fetchNetworkInfo, 10000);
        });
    </script>    
</head>
<body>
    <div class="sidebar">
        <h2>Configuração</h2>
        <ul>
            <li onclick="showSection('identificacao')">Identificação</li>
            <li onclick="showSection('configRede')">Configuração de Rede</li>
            <li onclick="showSection('modoOperacao')">Modo de Operação</li>
            <li onclick="showSection('configSensores')">Configurar Sensores</li>
            <li onclick="showSection('logs')">Logs</li>
        </ul>
    </div>
    <div class="content">
        <div id="identificacao" class="section">
            <h1>Identificação</h1>
            <div class="card network-info">
                <h3>Informações de Identificação</h3>
                <p><strong>Serial Number:</strong> <span id="serialNumber">Carregando...</span></p>
                <p><strong>IP:</strong> <span id="deviceIP">Carregando...</span></p>
                <p><strong>Gateway:</strong> <span id="gatewayIP">Carregando...</span></p>
                <p><strong>Máscara de Sub-rede:</strong> <span id="subnetMask">Carregando...</span></p>
                <p><strong>Servidor DNS:</strong> <span id="dnsServer">Carregando...</span></p>
                <p><strong>MAC Address:</strong> <span id="macAddress">Carregando...</span></p>
                <p><strong>Wi-Fi SSID:</strong> <span id="wifiSSID">Carregando...</span></p>
            </div>
            <div class="card">
                <h3>Data e Hora Atual</h3>
                <p><strong>RTC:</strong> <span id="currentRTC">Carregando...</span></p>
            </div>
            <div class="card">
                <h3>Tópicos MQTT</h3>
                <p><strong>Temperatura:</strong> <span id="mqttTemp">Carregando...</span></p>
                <p><strong>Umidade:</strong> <span id="mqttHumidity">Carregando...</span></p>
                <p><strong>Limites de Temperatura:</strong> <span id="mqttLimits">Carregando...</span></p>
            </div>
        </div>

        <div id="configRede" class="section hidden">
            <h1>Configuração de Rede</h1>
            <div class="card">
                <h3>Configurar Wi-Fi</h3>
                <form action="/setWiFi" method="post">
                    <input type="text" name="ssid" placeholder="SSID (nome da rede)" required>
                    <input type="password" name="password" placeholder="Senha" required>
                    <button type="submit">Conectar</button>
                </form>
            </div>
        </div>

        <div id="modoOperacao" class="section hidden">
            <h1>Modo de Operação</h1>
            <div class="card">
                <p>Configuração do modo de operação do dispositivo.</p>
            </div>
        </div>

        <div id="configSensores" class="section hidden">
            <h1>Configurar Sensores</h1>
            <div class="card">
                <p>Configuração dos sensores conectados ao dispositivo.</p>
            </div>
        </div>

        <div id="logs" class="section hidden">
            <h1>Logs</h1>
            <div class="card">
                <p>Visualização dos logs do sistema.</p>
            </div>
        </div>
    </div>
</body>
</html>

)rawliteral";    

#endif