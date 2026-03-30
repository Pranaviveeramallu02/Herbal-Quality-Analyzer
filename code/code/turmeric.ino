
#include <WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "DHT.h"

// -------- WIFI SETTINGS --------
char ssid[] = "Lakshmi4G";
char pass[] = "9059932008";

// -------- THINGSPEAK SETTINGS --------
unsigned long channelID = 3314514;
const char * writeAPIKey = "9FEOAFDJ4CHEL6L5";

WiFiClient client;

// -------- SENSOR PINS --------
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135_PIN 34

// -------- SENSOR OBJECTS --------
DHT dht(DHTPIN, DHTTYPE);

Adafruit_TCS34725 tcs = Adafruit_TCS34725(
TCS34725_INTEGRATIONTIME_50MS,
TCS34725_GAIN_4X
);

void setup() {

Serial.begin(115200);
delay(1000);

Wire.begin();
dht.begin();

Serial.print("Connecting to WiFi");
WiFi.begin(ssid, pass);

while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.print(".");
}

Serial.println(" Connected!");

ThingSpeak.begin(client);

if (!tcs.begin()) {
Serial.println("❌ TCS34725 NOT detected");
while (1);
}

Serial.println("✅ System Ready");
}

void loop() {

// -------- READ RGB SENSOR --------
uint16_t r, g, b, c;
tcs.getRawData(&r, &g, &b, &c);

// -------- READ MQ135 --------
int gasValue = analogRead(MQ135_PIN);

// -------- READ DHT11 --------
float temperature = dht.readTemperature();
float humidity = dht.readHumidity();

if (isnan(temperature) || isnan(humidity)) {
temperature = 0;
humidity = 0;
}

// -------- SAFE RATIOS --------
float ratioGR = 0;
float ratioBR = 0;

if (r != 0) {
ratioGR = (float)g / r;
ratioBR = (float)b / r;
}

// -------- TURMERIC DETECTION --------
String result;
int resultValue;

if (ratioGR > 0.75 && ratioGR < 1.1 && ratioBR < 0.6 && gasValue > 2200) {
result = "PURE TURMERIC";
resultValue = 1;
} else {
result = "ADULTERATED (RICE FLOUR)";
resultValue = 0;
}

// -------- SERIAL OUTPUT --------
Serial.println("----- TURMERIC DATA -----");

Serial.print("R: "); Serial.println(r);
Serial.print("G: "); Serial.println(g);
Serial.print("B: "); Serial.println(b);

Serial.print("Gas Value: "); Serial.println(gasValue);
Serial.print("Temperature: "); Serial.println(temperature);
Serial.print("Humidity: "); Serial.println(humidity);

Serial.print("G/R Ratio: "); Serial.println(ratioGR);
Serial.print("B/R Ratio: "); Serial.println(ratioBR);

Serial.print("Result: "); Serial.println(result);

Serial.println("-------------------------");

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
Serial.println("✅ Data sent to ThingSpeak");
} else {
Serial.print("❌ Error: ");
Serial.println(response);
}

delay(20000); // important
}

