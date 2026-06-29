#include <WiFi.h>
#include <WebServer.h>

#define TOUCH_PIN 4
#define LED_PIN   2

#define THRESHOLD 100          // 根据实际触摸数值调整
#define TOUCH_DEBOUNCE 80      // 触摸防抖时间，单位：ms
#define BLINK_INTERVAL 100     // 报警闪烁间隔，单位：ms

const char* ssid = "Curious";
const char* password = "Phq120621!";

WebServer server(80);

// 系统状态变量
bool armedState = false;       // 是否布防
bool alarmState = false;       // 是否报警锁定
bool rawTouchState = false;    // 原始触摸状态
bool stableTouchState = false; // 防抖后的触摸状态
bool lastTouchState = false;   // 上一次稳定触摸状态

bool ledState = false;
unsigned long lastTouchChangeTime = 0;
unsigned long lastBlinkTime = 0;

// 网页主页
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 安防报警器</title>
  <style>
    body {
      font-family: Arial, "Microsoft YaHei";
      text-align: center;
      margin-top: 60px;
      background-color: #f4f4f4;
    }

    .box {
      width: 80%;
      max-width: 420px;
      margin: auto;
      padding: 25px;
      background: white;
      border-radius: 12px;
      box-shadow: 0 2px 12px #aaa;
    }

    button {
      width: 140px;
      height: 52px;
      margin: 12px;
      border: none;
      border-radius: 8px;
      font-size: 18px;
      color: white;
      box-sizing: border-box;
    }


    .arm {
      background-color: #e53935;
    }

    .disarm {
      background-color: #43a047;
    }

    #status {
      font-size: 24px;
      font-weight: bold;
      margin: 25px;
    }
  </style>
</head>

<body>
  <div class="box">
    <h1>ESP32 物联网安防报警器</h1>

    <p id="status">系统状态：未布防</p>

    <button class="arm" onclick="sendCommand('/arm')">布防 Arm</button>
    <button class="disarm" onclick="sendCommand('/disarm')">撤防 Disarm</button>

    <p>布防后，触摸引脚将触发报警。</p>
  </div>

  <script>
    function sendCommand(url) {
      fetch(url)
        .then(function() {
          updateStatus();
        });
    }

    function updateStatus() {
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          let text;

          if (data.alarm) {
            text = "系统状态：报警中！";
          } else if (data.armed) {
            text = "系统状态：已布防";
          } else {
            text = "系统状态：未布防";
          }

          document.getElementById("status").innerHTML = text;
        });
    }

    updateStatus();
    setInterval(updateStatus, 1000);
  </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html; charset=UTF-8", html);
}

// 点击“布防”
void handleArm() {
  armedState = true;
  alarmState = false;

  // 重置触摸状态，避免旧状态干扰
  rawTouchState = false;
  stableTouchState = false;
  lastTouchState = false;

  ledState = false;
  digitalWrite(LED_PIN, LOW);

  Serial.println("系统已布防");
  server.send(200, "text/plain; charset=UTF-8", "Armed");
}

// 点击“撤防”
void handleDisarm() {
  armedState = false;
  alarmState = false;

  ledState = false;
  digitalWrite(LED_PIN, LOW);

  Serial.println("系统已撤防，报警解除");
  server.send(200, "text/plain; charset=UTF-8", "Disarmed");
}

// 网页查询当前系统状态
void handleStatus() {
  String json;

  if (alarmState) {
    json = "{\"armed\":true,\"alarm\":true}";
  } else if (armedState) {
    json = "{\"armed\":true,\"alarm\":false}";
  } else {
    json = "{\"armed\":false,\"alarm\":false}";
  }

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);

  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.on("/status", handleStatus);

  server.begin();
  Serial.println("安防Web服务器已启动");
}

void loop() {
  server.handleClient();

  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouchState = (touchValue < THRESHOLD);

  // 原始触摸状态变化时，重新计时
  if (currentTouchState != rawTouchState) {
    rawTouchState = currentTouchState;
    lastTouchChangeTime = millis();
  }

  // 软件防抖：状态稳定一段时间后才接受
  if (millis() - lastTouchChangeTime >= TOUCH_DEBOUNCE) {
    stableTouchState = rawTouchState;
  }

  // 边缘检测：未触摸 → 触摸
  if (!lastTouchState && stableTouchState) {
    if (armedState && !alarmState) {
      alarmState = true;  // 锁定报警状态
      Serial.println("警报触发！");
    }
  }

  lastTouchState = stableTouchState;

  // 报警时高频闪烁；未报警时LED熄灭
  if (alarmState) {
    if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}