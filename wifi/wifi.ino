#include <WiFi.h>
#include <WiFiUdp.h>

// ---------- Wi-Fi settings ----------
const char* ssid = "TP-Link";
const char* password = "ESP32 WiFi Server";

// ---------- Static IP ----------
// Adjust these according to your network settings (based on the 192.168.1.x range)
IPAddress local_IP(192, 168, 1, 150);   // The static IP address that always remains the same
IPAddress gateway(192, 168, 1, 1);      // Your router's IP address (usually this)
IPAddress subnet(255, 255, 255, 0);

WiFiUDP udp;
unsigned int localPort = 4210;
char incomingPacket[64];

// ---------- L298N driver pins ----------
#define IN1 13
#define IN2 12
#define IN3 14
#define IN4 27

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopMotors();

  // Configure the static IP before connecting
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Error configuring static IP");
  }

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print(">>> Connected! Static IP: ");
  Serial.println(WiFi.localIP());

  udp.begin(localPort);
  Serial.printf(">>> Listening on port %d\n", localPort);
}

void loop() {
  // If Wi-Fi is disconnected, reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(">>> Connection lost! Retrying...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(">>> Reconnected!");
  }

  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1);
    if (len > 0) incomingPacket[len] = 0;

    int code = atoi(incomingPacket);
    Serial.print("Received code: ");
    Serial.println(code);

    handleCommand(code);
  }
}

void handleCommand(int code) {
  if (code == 10 || code == 9 || code == 11) {
    moveForward();
  }
  else if (code == 15 || code == 16 ) {
    moveBackward();
  }
  else if (code == 6) {
    stopMotors();
  }
  else if (code == 0 || code == 1 || code == 2 || code == 3) {
    moveLeft();
  }
  else if (code == 12 || code == 13 || code == 14) {
    moveRight();
  }
}

void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void moveLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}