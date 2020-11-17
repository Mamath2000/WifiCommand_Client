/*
    This sketch sends a string to a TCP server, and prints a one-line response.
    You must run a TCP server in your local network.
    For example, on Linux you can use this command: nc -v -l 3000
*/

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "ESPap"
#define STAPSK  ""
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.4.1";
const uint16_t port = 80;

struct joystickValues {
  int x1;  int y1; bool btn1;
  int x2;  int y2; bool btn2;
};

const int Joy1x = 13;
const int Joy1y = 12;
const int Joy2x = 14;
const int Joy2y = 16;

unsigned long lastTime = 0;  
unsigned long timerDelay = 200;  // send readings timer

// Use WiFiClient class to create TCP connections
WiFiClient client;

void blink() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(1000);
  blink();
  blink();
  blink();

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  blink();
  blink();

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
//WiFi.setAutoReconnect(true);
//WiFi.persistent(true);

  delay(500);

  Serial.println("Init Pins");
  
  pinMode(Joy1x, OUTPUT);
  pinMode(Joy1y, OUTPUT);
  pinMode(Joy2x, OUTPUT);
  pinMode(Joy2y, OUTPUT);
  digitalWrite(Joy1x, LOW);
  digitalWrite(Joy1y, LOW);
  digitalWrite(Joy2x, LOW);
  digitalWrite(Joy2y, LOW);


}

int readAnalogValue(int pin) {
  digitalWrite(pin, HIGH);
  int a = analogRead(A0);
  //  delay(5);
  digitalWrite(pin, LOW);
  return a;
}

struct joystickValues read_joystick()
{
  struct joystickValues values;

  /////////////////////////////////////////////////////////////////////
  values.x1 = readAnalogValue(Joy1x);
  values.y1 = readAnalogValue(Joy1y);
  values.btn1 = false; // digitalRead()
  /////////////////////////////////////////////////////////////////////
  values.x2 = readAnalogValue(Joy2x);
  values.y2 = readAnalogValue(Joy2y);
  values.btn2 = false; // digitalRead()
  /////////////////////////////////////////////////////////////////////

  return values;

}


void loop() {
  if ((millis() - lastTime) > timerDelay) {
  // Read value of command
  joystickValues values;

  values = read_joystick();


  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
    }
    digitalWrite(LED_BUILTIN, HIGH);

    // We now create a URI for the request. Something like /data/?sensor_reading=123
    String url = "/data/";
    url += "?x1=";
    url += values.x1;
    url += "&y1=";
    url += values.y1;
    url += "&x2=";
    url += values.x2;
    url += "&y2=";
    url += values.y2;

    // This will send the request to the server
    /*client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n");*/
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
  
  unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 500) {
        Serial.println(">>> Client Timeout !");
        digitalWrite(LED_BUILTIN, LOW);
        client.stop();
         while(true);
        return;
      }
    }

  lastTime = millis();
  }

}
