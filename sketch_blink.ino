#include <SPI.h>
#include <WiFiNINA.h>
#include <DHT.h>
#include <ThingSpeak.h>


// WiFi Credentials
char ssid[] = "abc";     // Replace with your WiFi name
char pass[] = "abcdefgh"; // Replace with your WiFi password

// ThingSpeak Credentials
unsigned long myChannelNumber = 2877367; // Corrected: No quotes
const char *myWriteAPIKey = "AK9EHKRJE5WZD4HJ"; // Replace with your API key

// DHT Sensor Setup
#define DHTPIN 2         // DHT11 is connected to digital pin 2
#define DHTTYPE DHT11    // Change to DHT22 if using it
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");

  ThingSpeak.begin(client);
  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature(); // Get temperature in °C
  float humidity = dht.readHumidity();       // Get humidity

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: "); Serial.print(temperature); Serial.print(" °C, ");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  
  int response = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (response == 200) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.print("Error sending data! Code: ");
    Serial.println(response);
  }

  delay(60000); // Send data every 60 seconds
}
