/*
 *  Save money buying garage door remotes.
 *  Use your phone to open it instead.
 *  by Logan Moore <https://loganmoore.me>
 *
 *  https://loganmoore.me/garage-door-opener
 *
 */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "JellyFishNet";
const char* password = "muzazE7i";

ESP8266WebServer server(80);

void postButtonPress() {
  digitalWrite(D1, HIGH);
  delay(100);
  digitalWrite(D1, LOW);
  server.send(200, "text/json", "{\"status\": \"Success\"}");
}

void getRoot() {
  server.send(200, F("text/html"), "<!DOCTYPE html><html lang='en'> <head> <title>Circle Button</title> <style>*{/* Reset browser's padding and margin */ margin: 0; padding: 0; box-sizing: border-box;}.container{margin: 100px;}.my-button{width: 200px; height: 200px; outline-style:initial; border-radius: 100px; cursor: pointer;}</style> </head> <body> <div class='container'> <button class='my-button' onclick='pressButton()'>Garage Door</button> </div></body> <script>function pressButton(){var xhttp=new XMLHttpRequest(); xhttp.open('POST', '/press_button', true); xhttp.send();}</script></html>");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void restServerRouting() {
  server.on(F("/press_button"), HTTP_POST, postButtonPress);
  server.on("/", HTTP_GET, getRoot);
  server.onNotFound(handleNotFound);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(D1, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected");

  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  restServerRouting();
  server.begin();
  Serial.printf("Web server started, open http://%s in a web browser\n", WiFi.localIP().toString().c_str());
}

void loop() {
  server.handleClient();
}
