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
                document.getElementById('APSSID').innerText = data.ssid;
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
                document.getElementById('mqttTemp').innerText = `tcc/sensores/temperatura/${serial}`;
                document.getElementById('mqttHumidity').innerText = `tcc/sensores/umidade_do_ar/${serial}`;
    
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
    
        async function fetchLimits() {
            try {
                const response = await fetch('/getLimits');
                const data = await response.json();
                
                document.getElementById("tempMin").value = data.tempMin;
                document.getElementById("tempMax").value = data.tempMax;
                document.getElementById("humMin").value = data.humMin;
                document.getElementById("humMax").value = data.humMax;
                
                console.log("✅ Limites carregados:", data);
            } catch (error) {
                console.error("❌ Erro ao buscar os limites:", error);
            }
        }
    
        // Carregar os limites salvos no Local Storage
        function carregarLimites() {
            document.getElementById("tempMin").value = localStorage.getItem("tempMin") || 15.0;
            document.getElementById("tempMax").value = localStorage.getItem("tempMax") || 35.0;
            document.getElementById("humMin").value = localStorage.getItem("humMin") || 30.0;
            document.getElementById("humMax").value = localStorage.getItem("humMax") || 80.0;
        }
    
        // Salvar limites no Local Storage e enviar ao ESP32
        async function salvarLimites(event) {
            event.preventDefault(); // Impede que a página seja recarregada
    
            const tempMin = document.getElementById("tempMin").value;
            const tempMax = document.getElementById("tempMax").value;
            const humMin = document.getElementById("humMin").value;
            const humMax = document.getElementById("humMax").value;
    
            console.log(`📡 Enviando limites: TempMin=${tempMin}, TempMax=${tempMax}, HumMin=${humMin}, HumMax=${humMax}`);
    
            // Salvar no Local Storage
            localStorage.setItem("tempMin", tempMin);
            localStorage.setItem("tempMax", tempMax);
            localStorage.setItem("humMin", humMin);
            localStorage.setItem("humMax", humMax);
    
            try {
                const response = await fetch("/setLimits", {
                    method: "POST",
                    headers: { "Content-Type": "application/x-www-form-urlencoded" },
                    body: `tempMin=${tempMin}&tempMax=${tempMax}&humMin=${humMin}&humMax=${humMax}`
                });
    
                if (!response.ok) {
                    throw new Error("Erro ao salvar configurações no ESP32.");
                }
    
                const data = await response.json();
                console.log("✅ Resposta do ESP32:", data);
                alert("✅ Limites salvos com sucesso!");
    
                await fetchLimits(); // Atualiza os valores exibidos na interface
    
            } catch (error) {
                console.error("❌ Erro ao salvar limites:", error);
                alert("Erro ao salvar os limites!");
            }
        }
    
        document.addEventListener("DOMContentLoaded", function () {
            console.log("🚀 Página carregada. Inicializando eventos...");
    
            fetchRTC();
            fetchNetworkInfo();
            fetchSerialNumber();
            fetchLimits();
            carregarLimites();
            showSection('identificacao');
    
            setInterval(fetchRTC, 5000);
            setInterval(fetchNetworkInfo, 10000);
    
            // Garante que o botão "Salvar" está funcionando
            const btnSalvar = document.getElementById("btnSalvar");
            if (btnSalvar) {
                console.log("✅ Botão 'Salvar' encontrado.");
                btnSalvar.addEventListener("click", salvarLimites);
            } else {
                console.error("❌ Botão 'Salvar' não encontrado! Verifique o ID no HTML.");
            }
    
            // Garante que o formulário não recarrega ao ser enviado
            const form = document.getElementById("formSensores");
            if (form) {
                form.addEventListener("submit", salvarLimites);
            } else {
                console.error("❌ Formulário não encontrado!");
            }
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
                <p><strong>AP SSID:</strong> <span id="APSSID">Carregando...</span></p>
            </div>
            <div class="card">
                <h3>Data e Hora Atual</h3>
                <p><strong>RTC:</strong> <span id="currentRTC">Carregando...</span></p>
            </div>
            <div class="card">
                <h3>Tópicos MQTT</h3>
                <p><strong>Temperatura:</strong> <span id="mqttTemp">Carregando...</span></p>
                <p><strong>Umidade:</strong> <span id="mqttHumidity">Carregando...</span></p>
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

        <div id="configSensores" class="section hidden">
            <h1>Configurar Sensores</h1>
            <div class="card">
                <form id="formSensores">
                    <label>Temperatura Mínima:</label>
                    <input type="number" id="tempMin" step="0.1">
                    
                    <label>Temperatura Máxima:</label>
                    <input type="number" id="tempMax" step="0.1">
                    
                    <label>Umidade Mínima:</label>
                    <input type="number" id="humMin" step="0.1">
                    
                    <label>Umidade Máxima:</label>
                    <input type="number" id="humMax" step="0.1">
                    
                    <button id="btnSalvar">Salvar</button>
                </form>                
            </div>
        </div>

        <div id="alertas" class="section hidden">
            <h1>Alertas</h1>
            <div id="alertBox" class="card">
                <h3>Notificações dos Sensores</h3>
                <ul id="alertList"></ul>
            </div>
        </div>

        <div id="logs" class="section hidden">
            <h1>Logs</h1>
            <div class="card">
                <p>Visualização dos logs do sistema.</p>
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