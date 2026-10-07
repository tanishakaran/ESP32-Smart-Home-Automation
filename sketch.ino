#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22

#define LIGHT_PIN 5
#define FAN_PIN 18
#define SERVO_PIN 19

#define TRIG_PIN 25
#define ECHO_PIN 26

#define LDR_PIN 34

const char* ssid = "Wokwi-GUEST";
const char* password = "";

DHT dht(DHTPIN, DHTTYPE);
Servo doorServo;

WebServer server(80);

void setup() {
  Serial.begin(115200);

  dht.begin();

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LDR_PIN, INPUT);

  doorServo.attach(SERVO_PIN);
  doorServo.write(0);

  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);

  Serial.println("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("ESP32 Smart Home Automation");
  Serial.println("----------------------------");
}

void loop() {

  // DHT22
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Sensor reading failed!");
    delay(2000);
    return;
  }

  // HC-SR04
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  // LDR
  int lightLevel = analogRead(LDR_PIN);

  Serial.println();
  Serial.println("========== SMART HOME ==========");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity    : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Distance    : ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Light Level : ");
  Serial.println(lightLevel);

  // Automatic Fan
  if (temperature > 25) {
    digitalWrite(FAN_PIN, HIGH);
    Serial.println("Fan         : ON");
  } else {
    digitalWrite(FAN_PIN, LOW);
    Serial.println("Fan         : OFF");
  }

  // Automatic Light
  if (lightLevel < 2000) {
    digitalWrite(LIGHT_PIN, HIGH);
    Serial.println("Light       : ON (Dark)");
  } else {
    digitalWrite(LIGHT_PIN, LOW);
    Serial.println("Light       : OFF (Bright)");
  }

  // Smart Door
  if (distance > 0 && distance < 20) {
    doorServo.write(90);

    Serial.println("Object      : DETECTED");
    Serial.println("Door        : OPEN");
  } 
  else {
    doorServo.write(0);

    Serial.println("Object      : NOT DETECTED");
    Serial.println("Door        : LOCKED");
  }

  Serial.println("==============================");

  delay(3000);
}
