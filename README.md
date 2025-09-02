Tổng quan đề tài

Đề tài của bạn là: Hệ thống IoT cảnh báo ngập nước sử dụng ESP32 và Blynk.
Mục tiêu chính:
Giám sát nhiệt độ, độ ẩm môi trường, tình trạng mưa và mực nước theo thời gian thực.
Khi có nguy cơ ngập nước → hệ thống tự động cảnh báo bằng còi, relay, đồng thời gửi thông báo lên ứng dụng Blynk.
Người dùng có thể điều khiển relay và còi từ xa qua smartphone.

Ý nghĩa:
Giúp cảnh báo sớm tình trạng mưa lớn, nước dâng cao.
Bảo vệ tài sản trong nhà trọ, nhà riêng, hoặc khu vực dễ ngập.
Ứng dụng mô hình IoT + FreeRTOS để tạo hệ thống đa nhiệm, ổn định, realtime.

Mô tả đoạn code
Đoạn code được viết cho ESP32 và chia thành 2 phần chính:

 Khai báo & khởi tạo
Thư viện: WiFi, Blynk, DHT (đọc nhiệt độ/độ ẩm).
Chân kết nối:
DHT11 → chân 23
RAIN_PIN (cảm biến mưa) → chân 34
FLOAT_PIN (cảm biến phao nước) → chân 32
RELAY_PIN (relay) → chân 25
BUZZER_PIN (còi) → chân 5
WiFi + Blynk: kết nối tới mạng và server Blynk Cloud.
Biến lưu trạng thái relay và còi để đồng bộ với ứng dụng.

 Điều khiển từ xa qua Blynk
Nút Relay (V4): bật/tắt relay từ app.
Nút Còi (V5): bật/tắt còi từ app.
BLYNK_CONNECTED(): đồng bộ trạng thái khi ESP32 kết nối lại.

 Task FreeRTOS (đa luồng)
Tạo một Task riêng (TaskReadSensors) chạy song song với Blynk để:
Đọc DHT11 (nhiệt độ, độ ẩm).
Đọc cảm biến mưa & phao (logic LOW = kích hoạt).
Gửi dữ liệu lên app Blynk (V0–V3).
Kiểm tra mực nước:
Nếu nước dâng (FLOAT_PIN = LOW) → bật relay & còi, gửi cảnh báo Blynk.
Nếu không ngập → relay & còi lấy trạng thái từ app.
Dùng vTaskDelay(2000ms) thay vì delay() để không chặn chương trình.

 Loop chính

Chỉ chạy Blynk.run() → giữ kết nối ổn định.
Nhờ FreeRTOS, ESP32 vừa giữ kết nối WiFi, vừa đọc cảm biến định kỳ mà không bị gián đoạn.

Điểm nổi bật

Ứng dụng IoT: giám sát và cảnh báo qua Internet.
Realtime & đa luồng: nhờ FreeRTOS, ESP32 xử lý song song nhiều tác vụ.
Cảnh báo thông minh: tự động bật còi, relay và gửi thông báo khi nước dâng.
Điều khiển từ xa: người dùng bật/tắt thiết bị ngay trên điện thoại.
