/*VCC：3V、SDA：21番ピン、SCL：22番ピン
　裏向けてコネクタ上部の面を机に押し付けた時のセンサ高さを4mmとして校正する
　Aボタン：校正値適用（緑文字）、Bボタン：校正値除外（白文字）*/


#include <Arduino.h>
#include <M5Stack.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X(); // センサーインスタンス
int calibrationOffset = 0;  // 校正用のオフセット値
bool isCalibrated = false;  // 校正が適用されているか

void setup() {
    M5.begin();                 
    Wire.begin(21, 22);         

    M5.Lcd.setTextSize(2);      
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("VL53L0X Test");

    if (!lox.begin()) {         
        M5.Lcd.setCursor(10, 50);
        M5.Lcd.println("Failed to boot VL53L0X");
        while (1);
    }
}

void loop() {
    M5.update(); // ボタン状態の更新

    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);

    // 測距結果の取得
    int measuredDistance = (measure.RangeStatus != 4) ? measure.RangeMilliMeter : -1;

    // Aボタンで校正値を設定（4mmになるようにオフセットを調整）
    if (M5.BtnA.wasPressed()) {
        if (measuredDistance > 4) {
            calibrationOffset = measuredDistance - 4;
        } else {
            calibrationOffset = 0;
        }
        isCalibrated = true;
    }

    // Bボタンで校正値をリセット
    if (M5.BtnB.wasPressed()) {
        calibrationOffset = 0;
        isCalibrated = false;
    }

    // 校正値を適用した距離
    int displayedDistance = (measuredDistance == -1) ? -1 : measuredDistance - calibrationOffset;

    // 画面の更新
    M5.Lcd.fillRect(0, 50, 320, 40, BLACK); // 前回の値を消去
    M5.Lcd.setCursor(10, 50);

    if (displayedDistance >= 0) {
        if (isCalibrated) {
            M5.Lcd.setTextColor(GREEN, BLACK); // 校正値適用時は緑
        } else {
            M5.Lcd.setTextColor(WHITE, BLACK); // デフォルトは白
        }
        M5.Lcd.printf("Distance: %d mm\n", displayedDistance);
    } else {
        M5.Lcd.setTextColor(WHITE, BLACK);
        M5.Lcd.println("Out of range");
    }

    delay(100);
}
