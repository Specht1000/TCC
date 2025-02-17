// Inicialização do gauge semicircular
const ctx = document.getElementById('temperatureGauge').getContext('2d');

const temperatureGauge = new Chart(ctx, {
    type: 'doughnut',
    data: {
        datasets: [{
            data: [0, 100],
            backgroundColor: ['#00bcd4', '#333333'],
            borderWidth: 2,
            circumference: 180, // Semicircular
            rotation: 270 // Começa da esquerda
        }]
    },
    options: {
        responsive: false, // Forçar o tamanho fixo
        maintainAspectRatio: true,
        cutout: '70%',
        plugins: {
            legend: { display: false },
            tooltip: { enabled: false }
        }
    }
});

// Função para atualizar os dados do dashboard
async function fetchSensorData() {
    try {
        const response = await fetch('/sensor-data');
        const data = await response.json();

        // Atualiza o valor do gauge
        const tempValue = Math.min(Math.max(data.temperature, 0), 100);
        temperatureGauge.data.datasets[0].data = [tempValue, 100 - tempValue];
        temperatureGauge.update();

        // Atualiza o valor no centro do gauge
        document.getElementById('temperature').textContent = `${data.temperature} °C`;
    } catch (error) {
        console.error('Erro ao buscar dados dos sensores:', error);
    }
}

// Atualiza os dados a cada 5 segundos
setInterval(fetchSensorData, 5000);
fetchSensorData();
