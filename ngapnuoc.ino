#define BLYNK_TEMPLATE_ID "TMPL6Pt2uk_PT"
#define BLYNK_TEMPLATE_NAME "NgapNuoc"
#define BLYNK_AUTH_TOKEN "BIEwY1tIzkO7VotRNmg_cj0KLyguurNN"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// ====== WiFi ======
char ssid[] = "Fablap 2.4G";      
char pass[] = "Fira@2024"; 

// ====== Cảm biến & thiết bị ======
#define DHTPIN    23
#define DHTTYPE   DHT11
#define RAIN_PIN  34   // Cảm biến mưa (digital) - ACTIVE LOW
#define FLOAT_PIN 32   // Cảm biến phao (digital) - ACTIVE LOW
#define RELAY_PIN 25   // Relay
#define BUZZER_PIN 5   // Còi (buzzer)

DHT dht(DHTPIN, DHTTYPE);

// Biến lưu trạng thái
int relayState = LOW;
int buzzerState = LOW;

// ====== Blynk điều khiển ======
// Nút Relay (V4)
BLYNK_WRITE(V4) {  
  relayState = param.asInt();  
  digitalWrite(RELAY_PIN, relayState);  
}

// Nút Còi (V5)
BLYNK_WRITE(V5) {  
  buzzerState = param.asInt();
  digitalWrite(BUZZER_PIN, buzzerState);  
}

// Khi ESP kết nối lại Blynk
BLYNK_CONNECTED() {
  Blynk.syncAll();  // Đồng bộ các nút từ dashboard
}

// ====== TASK FreeRTOS ======
void TaskReadSensors(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    // Đọc cảm biến DHT11
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Đọc cảm biến mưa & phao (logic ngược)
    int rainRaw = digitalRead(RAIN_PIN);
    int waterRaw = digitalRead(FLOAT_PIN);

    int rain = (rainRaw == LOW) ? 1 : 0;     // LOW = có mưa
    int water = (waterRaw == LOW) ? 1 : 0;   // LOW = ngập

    // Gửi dữ liệu lên Blynk
    Blynk.virtualWrite(V0, t);     // Nhiệt độ
    Blynk.virtualWrite(V1, h);     // Độ ẩm
    Blynk.virtualWrite(V2, rain);  // 1 = có mưa
    Blynk.virtualWrite(V3, water); // 1 = ngập

    // ====== Tự động xử lý ngập ======
    if (water == 1) {  // nếu ngập
      digitalWrite(RELAY_PIN, HIGH);  
      digitalWrite(BUZZER_PIN, HIGH);  
      Blynk.virtualWrite(V4, 1);  // cập nhật trạng thái relay
      Blynk.virtualWrite(V5, 1);  // cập nhật trạng thái còi
      Blynk.logEvent("canh_bao_ngap", "⚠️ Nước dâng cao, nguy cơ ngập!");
    } else {
      // Nếu không ngập thì lấy trạng thái từ app
      digitalWrite(RELAY_PIN, relayState);
      digitalWrite(BUZZER_PIN, buzzerState);
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS); // delay 2s (FreeRTOS)
  }
}

void setup() {
  Serial.begin(19200);

  pinMode(RAIN_PIN, INPUT);
  pinMode(FLOAT_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Tạo task đọc sensor chạy song song với Blynk
  xTaskCreatePinnedToCore(
    TaskReadSensors,    // Task function
    "TaskReadSensors",  // Tên task
    4096,               // Stack size
    NULL,               // Tham số truyền vào
    1,                  // Priority
    NULL,               // Handle
    1                   // Chạy trên core 1 (WiFi core là 0)
  );
}

void loop() {
  Blynk.run(); // chạy Blynk riêng, không bị chặn bởi delay
}
