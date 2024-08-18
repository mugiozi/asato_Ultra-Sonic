#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiUdp.h>
#include <stdlib.h>
 
#define BUFSIZE 1024
 
const char* ssid     = "M5Stack_AP"; // Wi-FiのSSID
const char* password = "12345678"; // Wi-Fiのパスワード
const int localPort = 1234;

WiFiUDP udp;

void print_wifi_state(){
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(3, 3);
  M5.Lcd.println("WiFi connect");
  M5.Lcd.print("IP address: ");
  M5.Lcd.println(WiFi.softAPIP());
  M5.Lcd.print("localPort: ");
  M5.Lcd.println(localPort);
  
}

void display(){
  M5.Lcd.fillScreen(BLACK); // 背景色
  M5.Lcd.setTextSize(1);    // 文字サイズ（整数倍率）
  M5.Lcd.setTextFont(4);    // フォント 1(8px), 2(16px), 4(26px)

  M5.Lcd.drawFastHLine(0, 0, 320, WHITE);           // 指定座標から平行線
  M5.Lcd.fillRect(0, 1, 320, 27, (uint16_t)0x098a); // タイトルエリア背景
  M5.Lcd.drawFastHLine(0, 29, 320, WHITE);          // 指定座標から平行線
  M5.Lcd.setTextColor(WHITE);                       // 文字色
  M5.Lcd.drawCentreString("Distance sensor", 160, 4, 4);  //　指定座標を中心に文字表示（x, y, font）

  // M5.Lcd.drawFastVLine(202, 29, 43, WHITE);   // 指定座標から垂直線（x, y, 長さ, 色）
  M5.Lcd.drawFastHLine(0, 115, 320, WHITE);    // 指定座標から平行線（x, y, 長さ, 色）
  M5.Lcd.drawFastHLine(0, 200, 320, WHITE);    // 指定座標から平行線（x, y, 長さ, 色）

  M5.Lcd.setTextFont(2);      // フォント
  M5.Lcd.setCursor(10, 35);    
  M5.Lcd.print("Sensor_1");   // シリアルモニタ項目表示
  M5.Lcd.setCursor(10, 125);    
  M5.Lcd.print("Sensor_2");   // シリアルモニタ項目表示
  // M5.Lcd.fillRect(205, 52, 115, 18, WHITE);   // 塗りつぶし四角
}
 
void setup_wifi(){
 
  M5.Lcd.setTextColor(RED);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(3, 3);
  M5.Lcd.print("Connecting to ");
  M5.Lcd.println(ssid);
 
  WiFi.mode(WIFI_STA);
  WiFi.softAP(ssid, password);  //APモード開始
 
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(100);
//    M5.Lcd.print(".");
//  }

  int numStations = WiFi.softAPgetStationNum(); // 接続されているクライアントの数を取得

//  while (numStations == 0) {
//    delay(100);
//    M5.Lcd.print(".");
//  }
 
  print_wifi_state();
  
  udp.begin(localPort);
}


void setup() {
  M5.begin();

  Serial.begin(115200);

  M5.Lcd.setRotation(1);  //"1"で正常、"3"で180°反転
  
  setup_wifi();

  M5.Lcd.setTextSize(1);

  display();

}

void split(String data, String *lines){
  int count = 0;
  int datalength = data.length();
  for (int i = 0; i < datalength; i++) {
    char onec = data.charAt(i);
    if ( onec == ',' ) {
      count++;   
      if ( count == 2) {
        break ;
      }  
    }
    else {
      lines[count] += onec;
    }
  }
}

void loop() {
  char packetBuffer[BUFSIZE];
  String lines[2] ;

  char receivedValue1[BUFSIZE];
  char receivedValue2[BUFSIZE];

  char floatStr_1[16];
  char floatStr_2[16];

  // M5.Lcd.fillScreen(BLACK);
  M5.Lcd.fillRect(30, 55, 225, 60, BLACK);
  M5.Lcd.fillRect(30, 140, 225, 60, BLACK);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextFont(4);  

  // M5.update();
  int packetSize = udp.parsePacket();
  Serial.println(packetSize);

  if (packetSize == 32) {
  // if (packetSize >= sizeof(receivedValue1) + sizeof(receivedValue2)) {

    // int len = udp.read(packetBuffer, packetSize);

    int len_1 = udp.read(receivedValue1, 16); // value1を受信
    int len_2 = udp.read(receivedValue2, 16); // value2を受信

    Serial.println(receivedValue1);
    int len = len_1;

    if (len > 0){
      receivedValue1[len] = '\0';
    }
    split(receivedValue1, lines) ;

    Serial.println("Distanse=");
    Serial.println(lines[0]);

  
    M5.Lcd.setCursor(80, 60, 7);
    M5.Lcd.setTextDatum(2); //数値テキスト　ボックスの始点を右上に変更
    // M5.Lcd.printf("%07.2f", lines[0].toInt());
    // M5.Lcd.printf("%s", lines[0].c_str()); // 小数点以下の情報を保持したまま表示する
    M5.Lcd.drawString(lines[0], 250, 60, 7);  //数値表示


    Serial.println(receivedValue2);
    String lines[2] = { "", "" }; // 空の文字列で初期化
    len = 0;
    len = len_2;
    
    if (len > 0){
      receivedValue2[len] = '\0';
    }
    split(receivedValue2, lines) ;

    Serial.println("Distanse=");
    Serial.println(lines[0]);

  
    M5.Lcd.setCursor(80, 145, 7);
    M5.Lcd.setTextDatum(2); //数値テキスト　ボックスの始点を右上に変更
    // M5.Lcd.printf("%07.2f", lines[0].toInt());
    // M5.Lcd.printf("%s", lines[0].c_str()); // 小数点以下の情報を保持したまま表示する
    M5.Lcd.drawString(lines[0], 250, 145, 7);  //数値表示ダミー

    // dtostrf(receivedValue1, 7, 2, floatStr_1);
    // dtostrf(receivedValue2, 7, 2, floatStr_2);

    // Serial.println(receivedValue2);

    // if (len > 0){
    //   packetBuffer[len] = '\0';
    // }
    // split(packetBuffer, lines) ;

  
    M5.Lcd.setCursor(260, 90, 4);
    M5.Lcd.print("cm");

    M5.Lcd.setCursor(260, 175, 4); //1個目から85下げてる
    M5.Lcd.print("cm");


  }    
  delay(500);
}




