

#include <ArduinoOTA.h>
#ifdef ESP32
#include <WiFi.h>
#endif
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "AsyncJson.h"
#include "ArduinoJson.h"

// Replace with your network credentials
const char *ssid = "SUPERONLINE_WiFi_3921";
const char *password = "FCMKWC7LAUPT";

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (digitalRead(ledPin))
    {
      ledState = "ON";
    }
    else
    {
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  SPIFFS.begin(true);

  pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    response->addHeader("Server", "ESP Async Web Server");
    JsonObject root = response->getRoot();
    int i = 0;
    if (digitalRead(ledPin))
    {
      i = 1;
    }
    root["status"] = i;
    response->setLength();
    request->send(response);
  });

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/set", [](AsyncWebServerRequest *request, JsonVariant &json) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, "{\"status\":\"value\")");
    JsonObject jsonObj = json.as<JsonObject>();

    AsyncJsonResponse *response = new AsyncJsonResponse();
    response->addHeader("Server", "ESP Async Web Server");
    JsonObject root = response->getRoot();
    root["status"] = jsonObj["status"];
    if (jsonObj["status"] == 0)
    {
      digitalWrite(ledPin, LOW);
    }
    else
    {
      digitalWrite(ledPin, HIGH);
    }
    response->setLength();
    request->send(response);
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    if (request->method() == HTTP_OPTIONS)
    {
      AsyncWebServerResponse *response = request->beginResponse(200);
      response->addHeader("Access-Control-Allow-Methods", "PUT, POST, PATCH, DELETE, GET");
      request->send(response);
    }
    else
    {
      request->send(404);
    }
  });

  // Start server
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Orign, X-Requested-With, Content-Type, Accept, Authorization");
  server.addHandler(handler);
  server.begin();
}

void loop()
{
}
