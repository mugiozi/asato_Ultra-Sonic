#include <WiFi.h>

const char* ssid = "M5Stack-AP";
const char* password = "12345678";
const char* host = "192.168.4.1";  // M5StackのIPアドレス

void setup() {
  Serial.begin(115200);

  // Wi-Fiに接続
  WiFi.begin(ssid, password);

  Serial.print("WiFiに接続中");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFiに接続完了");
  Serial.print("IPアドレス: ");
  Serial.println(WiFi.localIP());

  // データ送信のテスト
  sendData("Hello, M5Stack!");
}

void loop() {
  // ループ内で定期的にデータを送信しても良い
  delay(10000);
  sendData("Ping!");
}

void sendData(const char* data) {
  WiFiClient client;

  if (!client.connect(host, 80)) {
    Serial.println("接続に失敗しました");
    return;
  }

  client.println(data);  // データを送信
  client.stop();
  Serial.println("データを送信しました: " + String(data));
}
