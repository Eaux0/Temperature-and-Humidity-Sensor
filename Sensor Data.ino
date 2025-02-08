#include "DHT.h"

DHT dht(D2, DHT11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print("\t");
  Serial.print("Humidity: ");
  Serial.println(h);
  delay(1000);
}
