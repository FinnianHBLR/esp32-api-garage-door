#include <WiFi.h>
#include <aWOT.h>

WiFiServer server(80);
Application app;
IPAddress ip();    
IPAddress local_IP(192, 168, 1, 144);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void removeQuotes(char* str) {
  int i = 0, j = 0;
  while (str[i] != '\0') {
    if (str[i] != '"') {
      str[j++] = str[i];  // Copy characters that are not quotes
    }
    i++;
  }
  str[j] = '\0';  // Null-terminate the cleaned string
}

void toggleDoor(Request &req, Response &res) {
  char name[15];
  char value[15];

  while (req.left()) {
    if (!req.form(name, 15, value, 15)) {
      return res.sendStatus(400);
    }

    removeQuotes(name);

    if(strcmp(name, "") == 0){
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

  if (!WiFi.config(local_IP, gateway, subnet)) {
  }
  
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
  
  WiFi.begin("", "");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  app.post("/open-door", &toggleDoor);

  server.begin();
}

void loop() {  
  WiFiClient client = server.available();

  if (client.connected()) {
    app.process(&client);
    client.stop();
  }
}