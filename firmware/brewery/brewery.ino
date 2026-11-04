/*
 * Пивоварня на ESP32 — стартовая прошивка
 * Датчик температуры DS18B20 на GPIO 4
 * Реле ТЭН/насоса на GPIO 26
 * Веб-сервер показывает температуру и управляет реле
 *
 * Библиотеки: OneWire и DallasTemperature
 * (Arduino IDE: Скетч -> Подключить библиотеку -> Управлять библиотеками)
 */

#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* WIFI_SSID = "ВАШ_WIFI";
const char* WIFI_PASS = "ВАШ_ПАРОЛЬ";

#define ONE_WIRE_PIN 4   // линия данных DS18B20
#define RELAY_PIN    26  // управление реле

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
WebServer server(80);

bool heaterOn = false;
float currentTemp = 0.0;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Пивоварня на ESP32</title></head><body style='font-family:sans-serif;text-align:center'>";
  html += "<h1>Пивоварня на ESP32</h1>";
  html += "<p>Температура: <b>" + String(currentTemp, 1) + " °C</b></p>";
  html += "<p>Нагрев: <b>" + String(heaterOn ? "ВКЛ" : "ВЫКЛ") + "</b></p>";
  html += "<p><a href='/on'><button>ВКЛЮЧИТЬ</button></a> ";
  html += "<a href='/off'><button>ВЫКЛЮЧИТЬ</button></a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  sensors.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println();
  Serial.print("IP адрес: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", []() { heaterOn = true; digitalWrite(RELAY_PIN, HIGH); handleRoot(); });
  server.on("/off", []() { heaterOn = false; digitalWrite(RELAY_PIN, LOW); handleRoot(); });
  server.begin();
}

void loop() {
  server.handleClient();
  sensors.requestTemperatures();
  currentTemp = sensors.getTempCByIndex(0);
  delay(2000);
}
