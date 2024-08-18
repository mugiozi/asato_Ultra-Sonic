#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiUdp.h>
#include <stdlib.h>
 
#define BUFSIZE 1024
#define threshold 0.1  //外れ値除去の閾値、値の変動がより大きい場合は表示値は更新されない
 
const char* ssid     = "M5Stack_AP"; // Wi-FiのSSID
const char* password = "12345678"; // Wi-Fiのパスワード
const int localPort = 1234;

bool Onlyflag1 = true;  //値1の初回処理フラグ
bool Onlyflag2 = true;  //値2の初回処理フラグ
float distance1 = 0;  //値1外れ値除去前の距離変数
float distance2 = 0;  //値2外れ値除去前の距離変数
float previousDistance1 = 0;  //値1の外れ値除去距離変数
float previousDistance2 = 0;  //値2の外れ値除去距離変数

float calibrationValue1 = 0;  //値1のキャリブレーション用
float calibrationValue2 = 0;  //値2のキャリブレーション用
float calibDistance1 = 0;  //値1のキャリブレーション後
float calibDistance2 = 0;  //値2のキャリブレーション後

bool ADJflag = false;  //調整ボタンBのスイッチフラグ
bool Lightflag = true;  //バックライトボタンCのスイッチフラグ

unsigned long previousMillis = 0; // 前回の時間を記録
const long interval = 500; // 0.5秒（500ミリ秒）、Atom側と相関をとっているのでこれを変えたらAtom側も変える

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

void calibration(){
  calibrationValue1 = previousDistance1;
  calibrationValue2 = previousDistance2;
}

void Brightness(){
  if(Lightflag == true){
    M5.Lcd.setBrightness(255);
  }else{
    M5.Lcd.setBrightness(0);
  }
}

void display(){
  if(ADJflag == false){
    M5.Lcd.fillScreen(BLACK); // 背景色
    M5.Lcd.setTextSize(1);    // 文字サイズ（整数倍率）
    M5.Lcd.setTextFont(4);    // フォント 1(8px), 2(16px), 4(26px)
    M5.Lcd.setTextColor(WHITE);                       // 文字色

      //ボタン名称の表示
    M5.Lcd.setCursor(220, 210);    
    M5.Lcd.print("LIGHT");   // シリアルモニタ項目表示
    M5.Lcd.setCursor(40, 210);    
    M5.Lcd.print("CALB");   // シリアルモニタ項目表示
    M5.Lcd.setCursor(140, 210);    
    M5.Lcd.print("ADJ");   // シリアルモニタ項目表示
    
      //キャプションの表示
    M5.Lcd.drawFastHLine(0, 0, 320, WHITE);           // 指定座標から平行線
    M5.Lcd.fillRect(0, 1, 320, 27, (uint16_t)0x098a); // タイトルエリア背景
    M5.Lcd.drawFastHLine(0, 29, 320, WHITE);          // 指定座標から平行線
    M5.Lcd.drawCentreString("Distance sensor", 160, 4, 4);  //　指定座標を中心に文字表示（x, y, font）

      //センサーラベルの表示
    // M5.Lcd.drawFastVLine(202, 29, 43, WHITE);   // 指定座標から垂直線（x, y, 長さ, 色）
    M5.Lcd.drawFastHLine(0, 115, 320, WHITE);    // 指定座標から平行線（x, y, 長さ, 色）
    M5.Lcd.drawFastHLine(0, 200, 320, WHITE);    // 指定座標から平行線（x, y, 長さ, 色）
    M5.Lcd.setTextFont(2);      // フォント
    M5.Lcd.setCursor(10, 35);    
    M5.Lcd.print("Sensor_1");   // シリアルモニタ項目表示
    M5.Lcd.setCursor(10, 125);    
    M5.Lcd.print("Sensor_2");   // シリアルモニタ項目表示
    // M5.Lcd.fillRect(205, 52, 115, 18, WHITE);   // 塗りつぶし四角
  }else{
    M5.Lcd.fillScreen(BLACK); // 背景色
    M5.Lcd.setTextSize(1);    // 文字サイズ（整数倍率）
    M5.Lcd.setTextFont(4);    // フォント 1(8px), 2(16px), 4(26px)
    M5.Lcd.setTextColor(WHITE);                       // 文字色

      //ボタン名称の表示
    M5.Lcd.setCursor(220, 210);    
    M5.Lcd.print("LIGHT");   // シリアルモニタ項目表示
    M5.Lcd.setCursor(40, 210);    
    M5.Lcd.print("CALB");   // シリアルモニタ項目表示
    M5.Lcd.setTextColor(0xFDA0);
    M5.Lcd.setCursor(140, 210);    
    M5.Lcd.print("ADJ");   // シリアルモニタ項目表示
    
      //キャプションの表示
    M5.Lcd.setTextColor(BLACK);                       // 文字色
    M5.Lcd.drawFastHLine(0, 0, 320, WHITE);           // 指定座標から平行線
    M5.Lcd.fillRect(0, 1, 320, 27, (uint16_t)0xFDA0); // タイトルエリア背景
    M5.Lcd.drawFastHLine(0, 29, 320, WHITE);          // 指定座標から平行線
    M5.Lcd.drawCentreString("Adjustment  MODE", 160, 4, 4);  //　指定座標を中心に文字表示（x, y, font）

      //センサーラベルの表示
    // M5.Lcd.drawFastVLine(202, 29, 43, WHITE);   // 指定座標から垂直線（x, y, 長さ, 色）
    M5.Lcd.setTextColor(WHITE);                       // 文字色
    M5.Lcd.drawFastHLine(0, 115, 320, WHITE);    // 指定座標から平行線（x, y, 長さ, 色）
    M5.Lcd.drawFastHLine(0, 200, 320, WHITE);    // 指定座標から平行線（x, y, 長さ, 色）
    M5.Lcd.setTextFont(2);      // フォント
    M5.Lcd.setCursor(10, 35);    
    M5.Lcd.print("Sensor_1");   // シリアルモニタ項目表示
    M5.Lcd.setCursor(10, 125);    
    M5.Lcd.print("Sensor_2");   // シリアルモニタ項目表示
    // M5.Lcd.fillRect(205, 52, 115, 18, WHITE);   // 塗りつぶし四角
  }
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
  M5.update(); // ボタンの状態を更新
  unsigned long currentMillis = millis(); // 現在の時間を取得

  // 0.5秒（500ミリ秒）経過したか確認
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // 前回の時間を更新

    char packetBuffer[BUFSIZE];
    String lines[2] ;

    char receivedValue1[BUFSIZE];
    char receivedValue2[BUFSIZE];

    char floatStr_1[16];
    char floatStr_2[16];

    // M5.Lcd.fillScreen(BLACK);
    M5.Lcd.fillRect(30, 55, 225, 60, BLACK);
    M5.Lcd.fillRect(30, 140, 225, 60, BLACK);
    M5.Lcd.setTextFont(4);  
    // M5.Lcd.setTextColor(GREEN);

    // M5.update();
    int packetSize = udp.parsePacket();
    Serial.println(packetSize);

    if (packetSize == 32) {
    // if (packetSize >= sizeof(receivedValue1) + sizeof(receivedValue2)) {

      // int len = udp.read(packetBuffer, packetSize);

      int len_1 = udp.read(receivedValue1, 16); // value1を受信
      int len_2 = udp.read(receivedValue2, 16); // value2を受信

//─────────────────────────────値_1の処理───────────────────────────────────
      Serial.println(receivedValue1);
      int len = len_1;

      if (len > 0){
        receivedValue1[len] = '\0';
      }
      split(receivedValue1, lines) ;

      distance1 = lines[0].toFloat();
      if(ADJflag == false){
        M5.Lcd.setTextColor(GREEN); //文字色　＝　緑
            //初回のみ外れ値除去閾値を0にする処理
        if (Onlyflag1 == true && distance1 > 0){
          previousDistance1 = distance1;
          Onlyflag1 = false;
        }
            // 外れ値除去
        if (abs(distance1 - previousDistance1) < threshold) {
          previousDistance1 = distance1;
        }
      }else{
        M5.Lcd.setTextColor(0xFDA0);  //文字色　＝　黄色
        previousDistance1 = distance1;
        Onlyflag1 = true;
      }
          //キャリブレーション
      calibDistance1 = previousDistance1 - calibrationValue1;

      Serial.println("Distanse_1=");
      // Serial.println(lines[0]);
      // Serial.println(distance1);
      Serial.println(calibDistance1);

      
      M5.Lcd.setCursor(80, 60, 7);
      M5.Lcd.setTextDatum(2); //数値テキスト　ボックスの始点を右上に変更
      // M5.Lcd.printf("%07.2f", lines[0].toInt());
      // M5.Lcd.printf("%s", lines[0].c_str()); // 小数点以下の情報を保持したまま表示する
      // M5.Lcd.drawString(lines[0], 250, 60, 7);  //数値表示
      // M5.Lcd.drawString(String(distance1), 250, 60, 7);  //数値表示
      M5.Lcd.drawString(String(calibDistance1), 250, 60, 7);  //数値表示

      M5.Lcd.setCursor(260, 90, 4);
      M5.Lcd.print("cm");

//─────────────────────────────値_2の処理───────────────────────────────────
      Serial.println(receivedValue2);
      String lines[2] = { "", "" }; // 空の文字列で初期化
      len = 0;
      len = len_2;
      
      if (len > 0){
        receivedValue2[len] = '\0';
      }
      split(receivedValue2, lines) ;

      distance2 = lines[0].toFloat();
      if(ADJflag == false){
        M5.Lcd.setTextColor(GREEN); //文字色　＝　緑
                //初回のみ外れ値除去閾値を0にする処理
        if (Onlyflag2 == true && distance2 > 0){
          previousDistance2 = distance2;
          Onlyflag2 = false;
        }
            // 外れ値除去
        if (abs(distance2 - previousDistance2) < threshold) {
          previousDistance2 = distance2;
        }
      }else{
        M5.Lcd.setTextColor(0xFDA0);  //文字色　＝　黄色
        previousDistance2 = distance2;
        Onlyflag2 = true;
      }

          //キャリブレーション
      calibDistance2 = previousDistance2 - calibrationValue2;

      Serial.println("Distanse_2=");
      // Serial.println(lines[0]);
      Serial.println(calibDistance2);

      
      M5.Lcd.setCursor(80, 145, 7);
      M5.Lcd.setTextDatum(2); //数値テキスト　ボックスの始点を右上に変更
      // M5.Lcd.printf("%07.2f", lines[0].toInt());
      // M5.Lcd.printf("%s", lines[0].c_str()); // 小数点以下の情報を保持したまま表示する
      // M5.Lcd.drawString(lines[0], 250, 145, 7);  //数値表示ダミー
      M5.Lcd.drawString(String(calibDistance2), 250, 145, 7);  //数値表示

      M5.Lcd.setCursor(260, 175, 4); //1個目から85下げてる
      M5.Lcd.print("cm");

      // dtostrf(receivedValue1, 7, 2, floatStr_1);
      // dtostrf(receivedValue2, 7, 2, floatStr_2);

      // Serial.println(receivedValue2);

      // if (len > 0){
      //   packetBuffer[len] = '\0';
      // }
      // split(packetBuffer, lines) ;
    }    
  }

//─────────────────────────────ボタン処理───────────────────────────────────

  if (M5.BtnA.wasPressed()) {
    // ボタンAが押された場合の処理
    calibration();
  }

  if (M5.BtnB.wasPressed()) {
    // ボタンBが押された場合の処理
    ADJflag = !ADJflag; //ADJflagの値を反転させる
    display();
  }

  if (M5.BtnC.wasPressed()) {
    // ボタンCが押された場合の処理
    Lightflag = !Lightflag; //Lightflagの値を反転させる
    Brightness();
  }
}