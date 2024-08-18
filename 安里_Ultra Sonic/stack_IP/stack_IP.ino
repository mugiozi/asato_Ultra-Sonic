#include <M5Stack.h>
#include <WiFi.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

void setup() {
  // Initialize M5Stack
  M5.begin();

  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set up the access point
  WiFi.softAP(ssid, password);

  // Get IP address of the access point
  IPAddress ip = WiFi.softAPIP();

  // Print IP address to Serial Monitor
  Serial.print("AP IP address: ");
  Serial.println(ip);

  // Display IP address on M5Stack screen
  M5.Lcd.clear();
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("AP IP: %s", ip.toString().c_str());
}

void loop() {
  // Nothing to do here
}
