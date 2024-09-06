// 引用函式庫
#include<WiFi.h>
#include<WiFiClient.h>
#include<ESP32Servo.h>
#include<TridentTD_LineNotify.h>
#include "Adafruit_MPU6050.h"

// 定義 LineTOKEN 和 WiFi SSID, password
#define LINE_TOKEN "YOUR LINE TOKEN"
const char ssid[] = "YOUR WIFI SSID"; //WiFi 名稱
const char pass[] = "YOUR WIFI PASSWORD";    //WiFi 密碼

// 網路連線物件
WiFiClient client;

// MPU6050 的加速度三軸
float accelX;
float accelY;
float accelZ;

// 定義腳位
int LEDR = 5;
int LEDG = 18;
int LEDB = 19;
int buzzer = 23;
Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin (115200);    // 傳輸速率115200ps
  pinMode(LEDR, OUTPUT);    // RGBLED燈的紅色
  pinMode(LEDG, OUTPUT);    // RGBLED燈的綠色
  pinMode(LEDB, OUTPUT);    // RGBLED燈的藍色
  pinMode(buzzer, OUTPUT);  // 蜂鳴器

  // 進行無線網路連接並顯示黃燈
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, LOW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // 等待WiFi連線
    Serial.print(" .");
  }
  Serial.println("");
  Serial.print("【系統】已連線至：");
  Serial.println(ssid);
  Serial.print("【系統】已分配至IP位置：");
  Serial.println(WiFi.localIP()); // 顯示ESP32裝置的IP位址

  // 連線成功蜂鳴器提醒
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
  mpu.begin();
  delay(2500);

  // Line 連線成功後將轉為綠燈表示正常運作
  LINE.setToken(LINE_TOKEN); //設定Line Token權杖
  LINE.notify("ESP32連線成功"); //發送訊息到Line上面
  Serial.println("【Line Notify】ESP32與Line連線成功");
  Serial.println();
  digitalWrite(LEDG, HIGH);
}

void loop() {
  // 取得 MPU6050 的相關數值
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // MPU6050校正
  accelX = accel.acceleration.x - 10;
  accelY = accel.acceleration.y - 0.5;
  accelZ = accel.acceleration.z - 0.9;

  // 序列視窗顯示加速度
  Serial.print("【MPU6050】加速度: ");
  Serial.print(accelX);
  Serial.print(" ");
  Serial.print(accelY);
  Serial.print(" ");
  Serial.println(accelZ);
  Serial.print("【MPU6050】陀螺儀: ");
  Serial.print(gyro.gyro.x);
  Serial.print(" ");
  Serial.print(gyro.gyro.y);
  Serial.print(" ");
  Serial.println(gyro.gyro.z);
  Serial.print("【MPU6050】溫度: ");
  Serial.println(temp.temperature);
  Serial.println();

  // 判斷是否有震動或移動
  if (accelX > 1.5 || accelY > 1.5 || accelZ > 1.5 ||
      accelX < -1.5 || accelY < -1.5 || accelZ < -1.5) {
    // 震動發生時，蜂鳴器發出逼逼叫和閃紅燈
    digitalWrite(LEDG, LOW);
    tone(LEDR, 10);
    tone(buzzer, 10, 1500);

    // 在LineNotify發送通知
    LINE.notify("您的車輛遭到震動或移動，請確保安全。");
    Serial.println("【Line Notify】您的車輛遭到震動或移動，請確保安全。");
    delay(2000);

    noTone(buzzer);
    noTone(LEDR);
  }

  // 判斷是否有溫度過高的問題
  if (temp.temperature > 35) {
    // 溫度過高時，蜂鳴器發出逼逼叫和閃藍燈
    digitalWrite(LEDG, LOW);
    tone(LEDB, 10);
    tone(buzzer, 262, 1500);

    // 在LineNotify發送通知
    LINE.notify("您的車輛溫度過高，請確保安全。");
    Serial.println("【Line Notify】您的車輛溫度過高，請確保安全。");
    delay(2000);

    noTone(buzzer);
    noTone(LEDB);
  }
  digitalWrite(LEDG, HIGH);

  delay(1000);
}
