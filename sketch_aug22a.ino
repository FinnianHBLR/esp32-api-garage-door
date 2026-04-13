#include <WiFi.h>
#include <aWOT.h>
// Wemos lolin32
WiFiServer server(80);
Application app;

IPAddress local_IP(192, 168, 1, 144);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Track uptime for reboot
unsigned long startMillis;

void removeQuotes(char* str) {
  int i = 0, j = 0;
  while (str[i] != '\0') {
    if (str[i] != '"') {
      str[j++] = str[i];
    }
    i++;
  }
  str[j] = '\0';
}

void toggleDoor(Request &req, Response &res) {
  char name[32];
  char value[32];

  while (req.left()) {
    if (!req.form(name, 32, value, 32)) {
      return res.sendStatus(400);
    }

    removeQuotes(name);
    // ADD: Password check
    if (strcmp(name, "") == 0) {
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
      res.print("Āe. Kia ora! ");
      res.sendStatus(200);
    } else {
      res.print("Kāo. ");
      res.sendStatus(403);
    }
  }
}

void setup() {
  pinMode(13, OUTPUT);

  startMillis = millis();

  WiFi.config(local_IP, gateway, subnet);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);

  // ADD: SSID + PASS
  WiFi.begin("", "");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  app.post("/open-door", &toggleDoor);

  server.begin();
}

void loop() {
  // Periodic reboot (every 3 days)
  if (millis() - startMillis > 1000UL * 60 * 60 * 24 * 3) {
    ESP.restart();
  }

  // WiFi self-healing
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
  // ADD: SSID + PASS
    WiFi.begin("", "");
    delay(5000);  // wait 5 seconds before retrying
    return; // Skip loop until reconnected
  }

  // Request handling
  WiFiClient client = server.available();

  if (client) {
    unsigned long timeout = millis();
    
    // Prevent hanging on slow or zombie clients by adding a timeout
    while (client.connected() && !client.available()) {
      if (millis() - timeout > 2000) {
        client.stop();
        return;
      }
      delay(1);
    }

    // Process the request (only if data exists)
    if (client.available()) {
      app.process(&client);
    }

    client.stop();
  }
}