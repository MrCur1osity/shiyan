#include <WiFi.h>
#include <WebServer.h>

#define TOUCH_PIN 4


WebServer server(80);

// 返回网页主页
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 实时传感器仪表盘</title>

  <style>
    body {
      font-family: Arial, "Microsoft YaHei", sans-serif;
      text-align: center;
      margin-top: 70px;
      background-color: #f4f6f8;
    }

    .box {
      width: 80%;
      max-width: 420px;
      margin: auto;
      padding: 30px;
      background-color: white;
      border-radius: 15px;
      box-shadow: 0 3px 12px rgba(0, 0, 0, 0.15);
    }

    #touchValue {
      font-size: 72px;
      font-weight: bold;
      color: #1565c0;
      margin: 25px 0;
    }

    .unit {
      font-size: 18px;
      color: #666;
    }

    .tip {
      color: #777;
      margin-top: 25px;
    }
  </style>
</head>

<body>
  <div class="box">
    <h1>ESP32 实时触摸仪表盘</h1>

    <p>当前触摸传感器数值</p>

    <div id="touchValue">--</div>
    <div class="unit">Touch Value</div>

    <p class="tip">
      手靠近触摸引脚时，数值通常会减小。
    </p>
  </div>

  <script>
    function updateTouchValue() {
      // 添加时间参数，避免浏览器使用缓存数据
      fetch("/touch?t=" + new Date().getTime())
        .then(function(response) {
          return response.text();
        })
        .then(function(data) {
          document.getElementById("touchValue").innerHTML = data;
        })
        .catch(function(error) {
          document.getElementById("touchValue").innerHTML = "连接失败";
        });
    }

    // 页面打开时立即读取一次
    updateTouchValue();

    // 每200 ms读取一次触摸数据
    setInterval(updateTouchValue, 200);
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html; charset=UTF-8", html);
}

// 返回实时触摸传感器数值
void handleTouch() {
  int touchValue = touchRead(TOUCH_PIN);

  // 返回纯文本数值给网页
  server.send(200, "text/plain", String(touchValue));
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  const char* ap_ssid = "ESP32-LAB151";
  const char* ap_pass = "12345678"; // 至少8位

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);

  Serial.println("AP已开启");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP()); // 通常 192.168.4.1

  // 注册网页主页
  server.on("/", handleRoot);

  // 注册触摸数据接口
  server.on("/touch", handleTouch);

  server.begin();
  Serial.println("实时传感器Web服务器已启动");
}

void loop() {
  // 持续处理网页请求
  server.handleClient();
}