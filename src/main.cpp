#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PZEM004Tv30.h>

// Configuración WiFi
char ssid[] = "F401";
char password[] = "MSICU2025";

// URL del servidor donde enviarás el POST
const char* serverUrl = "https://powertrack-bend.up.railway.app/sensor/measurements";

// Variables para los valores del sensor
float voltaje = 110.2;
float corriente = 2.1;
float potencia = 462.0;
float factor_potencia = 0.95;
float energia = 10.5;
float frecuencia = 60.0;

// Objeto para el sensor PZEM004T
#if defined(ESP32)
PZEM004Tv30 pzem(Serial2, 16, 17);  // RX, TX
#else
PZEM004Tv30 pzem(Serial2);
#endif

// Función para obtener la dirección MAC del ESP32
String getMacAddress() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[18] = { 0 };
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}

void setup() {
  Serial.begin(115200);

  // Conectar a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Dirección MAC: ");
  Serial.println(getMacAddress());
}

void loop() {
  // Leer los datos del sensor
  //Serial.print("Custom Address:");
  //Serial.println(pzem.readAddress(), HEX);

  voltaje = pzem.voltage();
  corriente = pzem.current();
  potencia = pzem.power();
  energia = pzem.energy();
  frecuencia = pzem.frequency();
  factor_potencia = pzem.pf();

  if (isnan(voltaje)) {
    Serial.println("Error reading voltage");
  } else if (isnan(corriente)) {
    Serial.println("Error reading current");
  } else if (isnan(potencia)) {
    Serial.println("Error reading power");
  } else if (isnan(energia)) {
    Serial.println("Error reading energy");
  } else if (isnan(frecuencia)) {
    Serial.println("Error reading frequency");
  } else if (isnan(factor_potencia)) {
    Serial.println("Error reading power factor");
  } 
  // else {
    // Serial.print("Voltage: "); Serial.print(voltaje); Serial.println("V");
    // Serial.print("Current: "); Serial.print(corriente); Serial.println("A");
    // Serial.print("Power: "); Serial.print(potencia); Serial.println("W");
    // Serial.print("Energy: "); Serial.print(energia,3); Serial.println("kWh");
    // Serial.print("Frequency: "); Serial.print(frecuencia,1); Serial.println("Hz");
    // Serial.print("PF: "); Serial.println(factor_potencia);
  // }

  Serial.println();

  // Comprobar si WiFi está conectado
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Crear el JSON con los datos del sensor
    StaticJsonDocument<256> doc;
    doc["mac_address"] = getMacAddress();
    doc["voltaje"] = voltaje;
    doc["corriente"] = corriente;
    doc["potencia"] = potencia;
    doc["factor_potencia"] = factor_potencia;
    doc["energia"] = energia;
    doc["frecuencia"] = frecuencia;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    //Serial.println(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Código HTTP: " + String(httpResponseCode));
      Serial.println("Respuesta:");
      Serial.println(response);
    } else {
      Serial.print("Error en petición HTTP: ");
      Serial.println(httpResponseCode);
    }

    http.end();

    delay(5000); // Enviar datos cada 5 segundos
  } else {
    Serial.println("Error de conexión WiFi");
  }

  delay(10);
}
