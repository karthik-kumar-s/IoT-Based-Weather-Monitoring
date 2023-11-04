#include <ESP8266WiFi.h>
#include <DHT.h>

// Replace these with your ThingSpeak Channel ID and Write API Key
char ssid[] = "YOUR_WIFI_SSID";
char password[] = "YOUR_WIFI_PASSWORD";
unsigned long channelID = YOUR_THINGSPEAK_CHANNEL_ID;
const char* apiKey = "YOUR_THINGSPEAK_API_KEY";

// Pin connected to the DHT11 sensor
#define DHTPIN 2
DHT dht(DHTPIN, DHT11);

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  delay(2000);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C\tHumidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  String postData = "field1=" + String(temperature) + "&field2=" + String(humidity);
  sendToThingSpeak(postData);
  delay(15000); // Delay for 15 seconds (ThingSpeak allows an update every 15 seconds)
}

void sendToThingSpeak(String data) {
  WiFiClient client;

  if (client.connect("api.thingspeak.com", 80)) {
    String postRequest = "POST /update HTTP/1.1\n";
    postRequest += "Host: api.thingspeak.com\n";
    postRequest += "Connection: close\n";
    postRequest += "X-THINGSPEAKAPIKEY: " + String(apiKey) + "\n";
    postRequest += "Content-Type: application/x-www-form-urlencoded\n";
    postRequest += "Content-Length: " + String(data.length()) + "\n\n";
    postRequest += data;
    
    client.print(postRequest);

    delay(500); // Wait for the server to process the request

    Serial.println("Data sent to ThingSpeak!");
  } else {
    Serial.println("Failed to connect to ThingSpeak server!");
  }

  client.stop();
}
