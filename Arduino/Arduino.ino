#include <DHT.h>
#include <ArduinoJson.h>

// Defina os pinos para os sensores de umidade, DHT, e relé
const int pinoSensorUmidade1 = A0;
const int pinoSensorUmidade2 = A1;
const int pinoDHT = 2;
const int pinoRele = 3;

// Crie uma instância do sensor DHT
DHT dht(pinoDHT, DHT22);

// Variável para rastrear o estado do relé
bool releStatus;
bool regaStatus;

// Função para enviar dados
void enviarDados(int mediaUmidadeSolo, float umidadeAr, float temperatura, bool releStatus, bool regaStatus, int valorSensor1, int valorSensor2) {
  // Crie um objeto JSON dinâmico
  DynamicJsonDocument jsonDocument(200);

  // Adicione os dados ao JSON
  jsonDocument["MS"] = mediaUmidadeSolo;
  jsonDocument["UA"] = umidadeAr;
  jsonDocument["TP"] = temperatura;
  jsonDocument["RL"] = releStatus;
  jsonDocument["S1"] = valorSensor1;
  jsonDocument["S2"] = valorSensor2;
  jsonDocument["RG"] = regaStatus;

  // Serialize o JSON para uma string
  String jsonString;
  serializeJson(jsonDocument, jsonString);

  // Envie os dados via Serial
  Serial.println(jsonString);
}

void setup() {
  Serial.begin(9600);

  // Inicialize o sensor DHT
  dht.begin();

  // Configurar o pino do relé como saída
  pinMode(pinoRele, OUTPUT);
}

void loop() {
  // Leia os valores dos sensores de umidade
  int valorSensor1 = analogRead(pinoSensorUmidade1);
  int valorSensor2 = analogRead(pinoSensorUmidade2);

  // Calcule a média da umidade do solo
  int mediaUmidadeSolo = (valorSensor1 + valorSensor2) / 2;

  // Leia a umidade do ar e a temperatura usando o sensor DHT
  float umidadeAr = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // Verifique se a média de umidade do solo é maior que o limiar
  if (mediaUmidadeSolo >= 650) {
    digitalWrite(pinoRele, HIGH);
    releStatus = true;

    // Chame a função para enviar os dados
    enviarDados(mediaUmidadeSolo, umidadeAr, temperatura, releStatus, regaStatus, valorSensor1, valorSensor2);

    // Loop enquanto a média de umidade do solo for maior ou igual a 650
    while (mediaUmidadeSolo >= 650) {
      // Leia os valores dos sensores de umidade
      valorSensor1 = analogRead(pinoSensorUmidade1);
      valorSensor2 = analogRead(pinoSensorUmidade2);

      // Calcule a média da umidade do solo
      mediaUmidadeSolo = (valorSensor1 + valorSensor2) / 2;

      // Aguarde 1 segundo
      delay(1000);
    }

    // Desligue o relé
    digitalWrite(pinoRele, LOW);
    regaStatus = true;
    releStatus = false;
  } else {
    releStatus = false;
    regaStatus = false;
    digitalWrite(pinoRele, LOW);
  }

  // Chame a função para enviar os dados
  enviarDados(mediaUmidadeSolo, umidadeAr, temperatura, releStatus, regaStatus, valorSensor1, valorSensor2);

  // Aguarde um intervalo antes de enviar os próximos dados
  delay(300000);
}
