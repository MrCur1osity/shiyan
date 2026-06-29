#include <WiFi.h>
#include <WebServer.h>

#define LED_PIN 2

const char* ssid = "Curious";
const char* password = "Phq120621!";

const int freq = 5000;       // PWM频率
const int resolution = 8;    // 8位PWM，占空比0~255

WebServer server(80);

int brightness = 0;

// 网页主页
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 无极调光器</title>

  <style>
    body {
      font-family: Arial, "Microsoft YaHei";
      text-align: center;
      margin-top: 50px;
    }

    input[type=range] {
      width: 80%;
      max-width: 400px;
    }

    #value {
      font-size: 30px;
      color: blue;
    }
  </style>
</head>

<body>
  <h1>ESP32 Web 无级调光器</h1>
  <h2>当前亮度：<span id="value">0</span></h2>

  <input type="range" id="slider" min="0" max="255" value="0">

  <p>向右拖动变亮，向左拖动变暗</p>

  <script>
    const slider = document.getElementById("slider");
    const value = document.getElementById("value");

    slider.addEventListener("input", function() {
      value.innerHTML = slider.value;

      // 将滑动条数值发送给ESP32
      fetch("/set?value=" + slider.value);
    });
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html; charset=UTF-8", html);
}

// 接收网页发送的亮度数值
void handleSet() {
  if (server.hasArg("value")) {
    brightness = server.arg("value").toInt();

    // 限制亮度范围为0~255
    brightness = constrain(brightness, 0, 255);

    // 设置PWM占空比
    ledcWrite(LED_PIN, brightness);

    Serial.print("当前亮度：");
    Serial.println(brightness);

    server.send(200, "text/plain", "OK");
  } 
  else {
    server.send(400, "text/plain", "Missing value");
  }
}

void setup() {
  Serial.begin(115200);

  // 初始化PWM输出
  ledcAttach(LED_PIN, freq, resolution);
  ledcWrite(LED_PIN, 0);

  // 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  // 注册网页路由
  server.on("/", handleRoot);

  // 注册亮度控制路由
  server.on("/set", handleSet);

  server.begin();
  Serial.println("Web服务器已启动");
}

void loop() {
  server.handleClient();
}