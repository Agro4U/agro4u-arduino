#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Bortolozzo";
const char* password = "03011997";
const char* userId = "paulo.junqueira@agro4u.life";
const char* accessToken = "4KtL7xHg3sF8pAqW2oPvQ6cB1iU9rY5";
const char* serverUrl = "http://195.181.247.232:8000/api/v1/realtime/data-receive";  // Substitua pela sua URL

// Função para enviar dados via POST
void enviarDadosHTTP(int mediaUmidadeSolo, float umidadeAr, float temperatura, bool releStatus, bool regaStatus, int valorSensor1, int valorSensor2) {
  // Faz a solicitação HTTP POST com os dados recebidos
  HTTPClient http;
  WiFiClient client;

  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");

  // Cria um novo objeto JSON e adiciona os dados recebidos
  DynamicJsonDocument postJsonDocument(200);
  postJsonDocument["userId"] = userId;
  postJsonDocument["accessToken"] = accessToken;
  postJsonDocument["MS"] = mediaUmidadeSolo;
  postJsonDocument["UA"] = umidadeAr;
  postJsonDocument["TP"] = temperatura;
  postJsonDocument["RL"] = releStatus ? "true" : "false";  // Converte bool para string
  postJsonDocument["RG"] = regaStatus ? "true" : "false";  // Converte bool para string
  postJsonDocument["S1"] = valorSensor1;
  postJsonDocument["S2"] = valorSensor2;

  // Serializa o JSON para uma string
  String postJsonString;
  serializeJson(postJsonDocument, postJsonString);

  // Faz a solicitação POST
  int httpCode = http.POST(postJsonString);

  // Verifica se a solicitação foi bem-sucedida
  if (httpCode > 0) {
    Serial.println("POST request successful");
  } else {
    Serial.println("Error in POST request");
  }

  // Libera os recursos
  http.end();
}

void setup() {
  Serial.begin(9600);

  // Conecta-se à rede Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void loop() {
  // Aguarda a chegada de dados pela porta serial
  if (Serial.available() > 0) {
    // Lê os dados da porta serial
    String jsonString = Serial.readStringUntil('\n');

    // Converte a string JSON para um objeto JSON
    DynamicJsonDocument jsonDocument(200);
    deserializeJson(jsonDocument, jsonString);

    // Extrai os dados do JSON
    int mediaUmidadeSolo = jsonDocument["MS"];
    float umidadeAr = jsonDocument["UA"];
    float temperatura = jsonDocument["TP"];
    bool releStatus = jsonDocument["RL"];
    bool regaStatus = jsonDocument["RG"];
    int valorSensor1 = jsonDocument["S1"];
    int valorSensor2 = jsonDocument["S2"];

    // Chama a função para enviar os dados via POST
    enviarDadosHTTP(mediaUmidadeSolo, umidadeAr, temperatura, releStatus, regaStatus, valorSensor1, valorSensor2);
  }
}
