#include <TinyGPS++.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define GPS_BAUDRATE 9600  // The default baudrate of NEO-6M is 9600

// Replace with your network credentials
const char* ssid = "";
const char* password = "";

TinyGPSPlus gps;  // the TinyGPS++ object
AsyncWebServer server(80);

String getGPSData() {
  String data = "";
  if (gps.location.isValid()) {
    data += "Latitude: " + String(gps.location.lat(), 6) + "\n";
    data += "Longitude: " + String(gps.location.lng(), 6) + "\n";
    data += "Altitude: " + String(gps.altitude.isValid() ? String(gps.altitude.meters()) : "INVALID") + "\n";
    data += "Speed: " + String(gps.speed.isValid() ? String(gps.speed.kmph()) + " km/h" : "INVALID") + "\n";
    if (gps.date.isValid() && gps.time.isValid()) {
      data += "Date & Time: " + String(gps.date.year()) + "-" + String(gps.date.month()) + "-" + String(gps.date.day()) + " ";
      data += String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()) + "\n";
    } else {
      data += "Date & Time: INVALID\n";
    }
  } else {
    data += "Location: INVALID\n";
  }
  return data;
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(GPS_BAUDRATE);

  Serial.println(F("ESP32 - GPS module"));

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getGPSData());
  });

  server.begin();
}

void loop() {
  if (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      // Print GPS data to Serial Monitor
      if (gps.location.isValid()) {
        Serial.print(F("- latitude: "));
        Serial.println(gps.location.lat());

        Serial.print(F("- longitude: "));
        Serial.println(gps.location.lng());

        Serial.print(F("- altitude: "));
        if (gps.altitude.isValid())
          Serial.println(gps.altitude.meters());
        else
          Serial.println(F("INVALID"));
      } else {
        Serial.println(F("- location: INVALID"));
      }

      Serial.print(F("- speed: "));
      if (gps.speed.isValid()) {
        Serial.print(gps.speed.kmph());
        Serial.println(F(" km/h"));
      } else {
        Serial.println(F("INVALID"));
      }

      Serial.print(F("- GPS date&time: "));
      if (gps.date.isValid() && gps.time.isValid()) {
        Serial.print(gps.date.year());
        Serial.print(F("-"));
        Serial.print(gps.date.month());
        Serial.print(F("-"));
        Serial.print(gps.date.day());
        Serial.print(F(" "));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        Serial.println(gps.time.second());
      } else {
        Serial.println(F("INVALID"));
      }

      Serial.println();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}
