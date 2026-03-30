#include <WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "DHT.h"

// -------- WIFI SETTINGS --------
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// -------- THINGSPEAK SETTINGS --------
unsigned long channelID = YOUR_CHANNEL_ID;
const char * writeAPIKey = "YOUR_WRITE_API_KEY";

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
  Wire.begin();
  dht.begin();

  // ---- WiFi Connection ----
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println(" Connected");

  // ---- ThingSpeak ----
  ThingSpeak.begin(client);

  // ---- RGB Sensor ----
  if (!tcs.begin()) {
    Serial.println("TCS34725 NOT detected");
    while (1);
  }

  Serial.println("System Ready for GINGER Detection");
}

void loop() {

  // -------- READ RGB --------
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

  // -------- CALCULATE RATIOS --------
  float ratioGR = 0;
  float ratioGB = 0;

  if (r != 0) ratioGR = (float)g / r;
  if (b != 0) ratioGB = (float)g / b;

  // -------- GINGER DETECTION LOGIC --------
  String result;
  int resultValue;

  /*
    PURE GINGER:
    - R and G almost equal (yellowish)
    - Moderate G/B ratio
    - Strong smell (gas high)
  */

  if (ratioGR > 0.8 && ratioGR < 1.2 && ratioGB > 1.2 && gasValue > 2000) {
    result = "PURE GINGER";
    resultValue = 1;
  }
  else {
    result = "ADULTERATED GINGER";
    resultValue = 0;
  }

  // -------- PRINT OUTPUT --------
  Serial.println("------ GINGER ANALYSIS ------");

  Serial.print("R: "); Serial.println(r);
  Serial.print("G: "); Serial.println(g);
  Serial.print("B: "); Serial.println(b);

  Serial.print("Gas: "); Serial.println(gasValue);

  Serial.print("Temp: "); Serial.println(temperature);
  Serial.print("Humidity: "); Serial.println(humidity);

  Serial.print("G/R Ratio: "); Serial.println(ratioGR);
  Serial.print("G/B Ratio: "); Serial.println(ratioGB);

  Serial.print("Result: "); Serial.println(result);

  Serial.println("------------------------------");

  // -------- SEND TO THINGSPEAK --------
  ThingSpeak.setField(1, r);
  ThingSpeak.setField(2, g);
  ThingSpeak.setField(3, b);
  ThingSpeak.setField(4, gasValue);
  ThingSpeak.setField(5, temperature);
  ThingSpeak.setField(6, humidity);
  ThingSpeak.setField(7, ratioGR);
  ThingSpeak.setField(8, resultValue);

  int response = ThingSpeak.writeFields(channelID, writeAPIKey);

  if (response == 200) {
    Serial.println("Data uploaded to ThingSpeak");
  } else {
    Serial.print("Error uploading: ");
    Serial.println(response);
  }

  delay(20000); // ThingSpeak delay
}
