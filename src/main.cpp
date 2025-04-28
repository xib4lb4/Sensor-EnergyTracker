#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configuración WiFi - Reemplaza con tus credenciales
char ssid[] = "Motomami";        // Cambiado de const char* a char[]
char password[] = "Soporte01"; // Cambiado de const char* a char[]

// URL del servidor donde enviarás el POST
const char* serverUrl = "https://powertrack-bend.up.railway.app/sensor/measurements";

// Variables para los valores del sensor
float voltaje = 110.2;
float corriente = 2.1;
float potencia = 462.0;
float factor_potencia = 0.95;
float energia = 10.5;
float frecuencia = 60.0;

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
  // Comprobar si WiFi está conectado
  if(WiFi.status() == WL_CONNECTED) {
    // Crear objeto HTTP
    HTTPClient http;
    
    // Iniciar conexión
    http.begin(serverUrl);
    
    // Especificar encabezados de contenido
    http.addHeader("Content-Type", "application/json");
    
    // Crear el JSON con los datos del sensor usando ArduinoJson
    StaticJsonDocument<256> doc;
    doc["sensorId"] = 4;
    doc["voltaje"] = voltaje;
    doc["corriente"] = corriente;
    doc["potencia"] = potencia;
    doc["factor_potencia"] = factor_potencia;
    doc["energia"] = energia;
    doc["frecuencia"] = frecuencia;
    
    // Serializar el JSON a String
    String requestBody;
    serializeJson(doc, requestBody);
    
    // Enviar la petición POST
    int httpResponseCode = http.POST(requestBody);
    
    // Comprobar respuesta
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Código HTTP: " + String(httpResponseCode));
      Serial.println("Respuesta:");
      Serial.println(response);
    } else {
      Serial.print("Error en petición HTTP: ");
      Serial.println(httpResponseCode);
    }
    
    // Cerrar conexión
    http.end();
    
    // Esperar antes de la siguiente petición
    delay(5000); // Enviar datos cada 5 segundos
  } else {
    Serial.println("Error de conexión WiFi");
  }
  
  // Simulación de lectura de sensores - En un caso real, aquí leerías los sensores
  // voltaje = leerVoltaje();
  // corriente = leerCorriente();
  // potencia = voltaje * corriente;
  // etc...
  
  delay(10);
}