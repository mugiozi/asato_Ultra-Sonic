#include <M5Atom.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char* ssid     = "M5Stack_AP"; // Wi-FiのSSID
const char* password = "12345678";  // Wi-Fiのパスワード

const int m5StackPort = 1234;  // M5Stackのポート
const int atomPort = 60001;  //Atomのポート
const char *m5StackIPAddress = "192.168.4.1"; // M5StackのIPアドレス
WiFiUDP udp;

#define A_SEND 21 // 音波の送信_1
#define A_RECEIVE 25 // 音波の受信_1
#define B_SEND 19 // 音波の送信_2
#define B_RECEIVE 22 // 音波の受信_2

float A_counts;
float A_distance;
float B_counts;
float B_distance;

int analog33;
int analog34;
// float analogconf33;
// float analogconf34;

void print_wifi_state(){
  delay(100);

  Serial.println("Wi-Fi接続完了");
  Serial.println(WiFi.localIP());

  delay(100);
 
}

void setup_wifi(){
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  Serial.println("\nWi-Fi 接続開始");

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
 
  print_wifi_state();

}

float generateRandomValue() {
    return (rand() % 99 + 1) / 100.0f;
}

void setup() {
  Serial.begin(115200);
  pinMode(34, ANALOG);

  M5.begin();

  delay(100);
  
  setup_wifi();

  udp.begin(atomPort);

  pinMode(A_SEND, OUTPUT);
  pinMode(A_RECEIVE, INPUT);

  pinMode(B_SEND, OUTPUT);
  pinMode(B_RECEIVE, INPUT);

}

void loop() {
  //アナログ値を読み取り、格納、値の確認、VR感度調整式
  analog33 = analogRead(33);
  analog34 = analogRead(34);
  // Serial.printf("Up:%d Down:%d \n", 5000, 0);
  Serial.printf("GPIO33:%d GPIO34:%d \n", analog33, analog34);  //2155が中央値（補正なし）

  //Aセンサーの取得
  digitalWrite(A_SEND, HIGH);
  delayMicroseconds(10);
  digitalWrite(A_SEND, LOW);

  A_counts = pulseIn(A_RECEIVE, HIGH);
  A_distance = (A_counts * 0.017);
  A_distance = (A_distance + (analog33 - 215 - 1940) / 194);
  // A_distance = (A_distance + analog33);

  //Bセンサーの取得
  digitalWrite(B_SEND, HIGH);
  delayMicroseconds(10);
  digitalWrite(B_SEND, LOW);

  B_counts = pulseIn(B_RECEIVE, HIGH);
  B_distance = (B_counts * 0.017);
  B_distance = (B_distance + (analog34 - 215 - 1940) / 194);
  // B_distance = (B_distance + analog34);
  // srand(time(NULL));

  // float randomValue = generateRandomValue();  // 0.01から0.99までのランダムな値を生成、とりあえず小数点つけとけばいい
  // distance = distance + randomValue;
  // Serial.println(A_counts);
  // Serial.println(B_counts);


  char A_distancestr[16] ;
  char B_distancestr[16] ;
  uint8_t A_distancebytes[16] ;
  uint8_t B_distancebytes[16] ;



  sprintf(A_distancestr, "%03.2f", A_distance) ;
  memcpy(A_distancebytes, A_distancestr, 16) ;

  sprintf(B_distancestr, "%03.2f", B_distance) ;
  memcpy(B_distancebytes, B_distancestr, 16) ;

  Serial.println("Distanse=");
  Serial.println(A_distancestr);
  Serial.println(B_distancestr);
  // Serial.println(A_counts);
  // Serial.println(B_counts);

  udp.beginPacket(m5StackIPAddress, m5StackPort);
  udp.write(A_distancebytes, sizeof(A_distancebytes));
  udp.write(B_distancebytes, sizeof(B_distancebytes));
  udp.endPacket();

  // Serial.println(sizeof(A_distancebytes));
  // Serial.println(sizeof(B_distancebytes));
  // Serial.println(sizeof(float));

  delay(500);
}