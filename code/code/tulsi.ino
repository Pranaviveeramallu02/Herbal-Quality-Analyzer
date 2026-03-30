#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <DHT.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

// -------- WIFI --------
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// -------- THINGSPEAK --------
unsigned long channelID = 3289932;
const char* writeAPIKey = "YOUR_WRITE_API_KEY";

WiFiClient client;

// -------- DHT SENSOR --------
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// -------- MQ135 --------
#define MQ135_PIN A0

// -------- RGB SENSOR --------
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_50MS,
  TCS34725_GAIN_4X
);

void setup() {

  Serial.begin(115200);

  // Start sensors
  dht.begin();

  if (!tcs.begin()) {
    Serial.println("RGB Sensor not detected");
    while (1);
  }

  // Connect WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  ThingSpeak.begin(client);
}

void loop() {

  // -------- RGB SENSOR --------
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  float ratioGR = (float)g / r;
  float ratioGB = (float)g / b;

  // -------- MQ135 SENSOR --------
  int gasValue = analogRead(MQ135_PIN);

  // -------- DHT11 --------
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // -------- RESULT LOGIC --------
  String result;
  int resultValue;

  if (ratioGR > 1.4 && ratioGB > 1.8 && gasValue > 2500) {
    result = "PURE TULSI";
    resultValue = 1;
  }
  else if (gasValue < 1500) {
    result = "LIKELY PURE";
    resultValue = 2;
  }
  else {
    result = "ADULTERATED SAMPLE";
    resultValue = 3;
  }

  // -------- SERIAL OUTPUT --------
  Serial.println("------------------------");

  Serial.print("R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.println(b);

  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  Serial.print("Temperature: ");
  Serial.println(temperature);

  Serial.print("Humidity: ");
  Serial.println(humidity);

  Serial.print("G/R Ratio: ");
  Serial.println(ratioGR);

  Serial.print("G/B Ratio: ");
  Serial.println(ratioGB);

  Serial.println(result);

  Serial.println("------------------------");

  // -------- SEND TO THINGSPEAK --------
  ThingSpeak.setField(1, r);
  ThingSpeak.setField(2, g);
  ThingSpeak.setField(3, b);
  ThingSpeak.setField(4, gasValue);
  ThingSpeak.setField(5, temperature);
  ThingSpeak.setField(6, humidity);
  ThingSpeak.setField(7, ratioGR);
  ThingSpeak.setField(8, resultValue);

  ThingSpeak.setStatus(result);

  int response = ThingSpeak.writeFields(channelID, writeAPIKey);

  if (response == 200) {
    Serial.println("Data uploaded to ThingSpeak");
  }
  else {
    Serial.print("Upload failed. Error: ");
    Serial.println(response);
  }

  delay(20000);  // ThingSpeak limit (15 sec minimum)
}
