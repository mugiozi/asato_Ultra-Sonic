#include <M5Stack.h>
#include <WiFi.h>

const char* ssid = "M5Stack-AP";
const char* password = "12345678";
WiFiServer server(80);

void setup() {
  M5.begin();
  Serial.begin(115200);

  // Wi-Fiアクセスポイントとして設定
  WiFi.softAP(ssid, password);
  server.begin();  // サーバーを開始

  Serial.println("APモードでWiFiを起動中...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IPアドレス: ");
  Serial.println(WiFi.softAPIP());

  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Waiting for connection...");
}

void loop() {
  WiFiClient client = server.available();  // クライアントの接続を待つ

  if (client) {
    Serial.println("クライアントが接続されました。");

    while (client.connected()) {
      if (client.available()) {
        String receivedData = client.readStringUntil('\n');
        Serial.print("受信データ: ");
        Serial.println(receivedData);

        M5.Lcd.clear();
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.print("Received: ");
        M5.Lcd.println(receivedData);
      }
    }
    client.stop();
    Serial.println("クライアントが切断されました。");
  }
}
