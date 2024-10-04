#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_SSID"; //Fill ssid password and gemini token
const char* password = "YOUR_PASSWORD";
const char* Gemini_Token = "YOUR_TOKEN";
const char* Gemini_Max_Tokens = "100";
String img ="iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAAAFUlEQVR42mP8z8BQz0AEYBxVSF+FABJADveWkH6oAAAAAElFTkSuQmCC"; //it is just an example. You can replace it with whatever you want
String res ="";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  while (!Serial)
    ;

    WiFi.begin(ssid, password);
    Serial.print("Connecting to");
    Serial.println(ssid);
    while (WiFi.status() !=WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() 
{
  Serial.println("");
  Serial.println("Ask your Question : ");
  while (!Serial.available())
    ;
  while (Serial.available()) {
    char add = Serial.read();
    res = res + add;
    delay(1);
  }
  int len = res.length();
  res = res.substring(0, (len - 1));
  res = "\"" + res + "\"";
  img = "\"" + img + "\"";
  Serial.println("");
  Serial.print("Asking Your Question : ");
  Serial.println(res);

  WiFiClientSecure client;
  client.setInsecure();  // Use for testing purposes only
  HTTPClient http;

  if (http.begin(client, "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + (String)Gemini_Token)) {  // HTTPS
    http.addHeader("Content-Type", "application/json");
    String payload = String("{\"contents\": [{\"parts\":[{\"text\":" + res + "},{\"inline_data\": {\"mime_type\": \"image/png\",\"data\":" + img + "}}]}],\"generationConfig\": {\"maxOutputTokens\":" + (String)Gemini_Max_Tokens + "}}");

    int httpCode = http.POST(payload);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = http.getString();

      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);
      String Answer = doc["candidates"][0]["content"]["parts"][0]["text"];

      // For Filtering out Special Characters, WhiteSpaces and NewLine Characters
      Answer.trim();
      String filteredAnswer = "";
      for (size_t i = 0; i < Answer.length(); i++) {
        char c = Answer[i];
        if (isalnum(c) || isspace(c)) {
          filteredAnswer += c;
        } else {
          filteredAnswer += ' ';
        }
      }
      Answer = filteredAnswer;

      Serial.println("");
      Serial.println("Here is your Answer: ");
      Serial.println("");
      Serial.println(Answer);
    } else {
      Serial.printf("[HTTPS] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
  res = "";
}