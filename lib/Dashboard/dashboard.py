from flask import Flask, jsonify, render_template
import random

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/sensor-data')
def sensor_data():
    # Simulação de dados de sensores
    try:
        data = {
            'temperature': round(random.uniform(20.0, 30.0), 2),
            'humidity': round(random.uniform(40.0, 60.0), 2)
        }
        return jsonify(data)
    except Exception as e:
        app.logger.error(f"Erro ao gerar dados do sensor: {e}")
        return jsonify({"error": "Erro ao obter dados dos sensores"}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
