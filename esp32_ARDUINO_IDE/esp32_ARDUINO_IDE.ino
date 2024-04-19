#include <WiFi.h>
#include <DHT.h>
#include <FirebaseESP32.h>
#define FIREBASE_HOST "https://canhbaoluiot-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "1PGsaifijhVXn08y4QkEIV6y7jYYTiQOHoY6KWwy"
FirebaseData fbdb;

#define WIFI_SSID "HOANG DUY"
#define WIFI_PASSWORD "20082006"

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define TRIG_PIN 13

#define ECHO_PIN 12
#define LED_RED 27  
#define LED_BLUE 26
#define LED_GREEN 25
#define BUZZER 33
unsigned long previousMillis = 0;

const float alpha = 0.2;
float avg_distance = 0, distance = 0;

float temperature, humidity;
int danger_level;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  distance = round((pulseIn(ECHO_PIN, HIGH) / 58.0) * 10) / 10.0;
  avg_distance = round((alpha * distance + (1 - alpha) * avg_distance) * 10) / 10.0;
  temperature = round(dht.readTemperature() * 10) / 10.0;
  humidity = round(dht.readHumidity() * 10) / 10.0;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 500) {
    previousMillis = currentMillis;
    Firebase.setInt(fbdb, "IoT/Muc_canh_bao", danger_level);
    Firebase.setFloat(fbdb, "IoT/Khoang_cach", distance);
    Firebase.setFloat(fbdb, "IoT/Trung_binh", avg_distance);
    Firebase.setFloat(fbdb, "IoT/Nhiet_do", temperature);
    Firebase.setFloat(fbdb, "IoT/Do_am", humidity);
  }
  Serial.println("------------------------------------------");
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Average distance: ");
  Serial.print(avg_distance);
  Serial.println(" cm");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C, Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  if (avg_distance < 30) {
    danger_level = 3;
    digitalWrite(LED_RED, HIGH);// đèn đỏ
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(BUZZER, HIGH);
    Serial.println("Danger level 3");
  } else if (avg_distance < 60) {
    danger_level = 2;
    digitalWrite(LED_RED, HIGH);// đèn vàng
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(BUZZER, LOW);
    Serial.println("High water level warning");
  } else {
    danger_level = 1;
    digitalWrite(LED_RED, LOW); // đèn xanh
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(BUZZER, LOW);
    Serial.println("Safe");
  }

}
